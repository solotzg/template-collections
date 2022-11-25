#!/usr/bin/python3
import argparse
import os
import subprocess
import sys
import time
import logging
import types
try:
    import orjson as json
except ImportError:
    import json as json

logger = None


def get_tz_offset():
    import datetime
    now_stamp = time.time()
    local_time = datetime.datetime.fromtimestamp(now_stamp)
    utc_time = datetime.datetime.utcfromtimestamp(now_stamp)
    offset = local_time - utc_time
    total_seconds = offset.total_seconds()
    flag = '+'
    if total_seconds < 0:
        flag = '-'
        total_seconds = -total_seconds
    mm, ss = divmod(total_seconds, 60)
    hh, mm = divmod(mm, 60)
    tz_offset = "%s%02d:%02d" % (flag, hh, mm)
    return tz_offset


def init_logger():
    global logger

    tz_offset = get_tz_offset()

    orig_record_factory = logging.getLogRecordFactory()
    log_colors = {
        logging.DEBUG: "\033[1;34m",  # blue
        logging.INFO: "\033[1;32m",  # green
        logging.WARNING: "\033[1;35m",  # magenta
        logging.ERROR: "\033[1;31m",  # red
        logging.CRITICAL: "\033[1;41m",  # red reverted
    }

    def get_message(ori):
        msg = str(ori.msg)
        if ori.args:
            msg = msg % ori.args
        msg = "{}{}{}".format(log_colors[ori.levelno], msg, "\033[0m")
        return msg

    def record_factory(*args, **kwargs):
        record = orig_record_factory(*args, **kwargs)
        record.getMessage = types.MethodType(get_message, record)
        return record

    logging.setLogRecordFactory(record_factory)

    root = logging.getLogger()
    root.setLevel(logging.DEBUG)
    handler = logging.StreamHandler(sys.stdout)
    handler.setLevel(logging.DEBUG)
    handler.setFormatter(
        fmt=logging.Formatter('[%(asctime)s.%(msecs)03d {}][%(levelname)s][%(message)s]'.format(tz_offset),
                              datefmt='%Y/%m/%d %H:%M:%S'))
    root.addHandler(handler)
    logger = root


init_logger()


def wrap_run_time(func):
    def wrap_func(*args, **kwargs):
        bg = time.time()
        r = func(*args, **kwargs)
        logger.info('Time cost {:.3f}s'.format(time.time() - bg))
        return r

    return wrap_func


@wrap_run_time
def run_cmd(cmd, show_stdout=False):
    logger.debug("RUN CMD:\n\t{}\n".format(cmd))
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    stdout = None
    if show_stdout:
        stdout = bytes()
        for line in proc.stdout:
            stdout += (line)
            logger.debug(line.decode('utf-8').rstrip())
    _stdout, stderr = proc.communicate()
    stdout = stdout if show_stdout else _stdout
    return stdout.decode('utf-8'), stderr.decode('utf-8'), proc.returncode


class Runner:
    def __init__(self):
        self.compile_commands_path = "compile_commands.json"
        parser = argparse.ArgumentParser(description="Compile source file by `{}`".format(self.compile_commands_path),
                                         formatter_class=argparse.ArgumentDefaultsHelpFormatter)
        parser.add_argument(
            '--file', help='absolute path of file to compile', required=True)
        parser.add_argument(
            '--type', help='build type', required=True, choices=('syntax', 'tidy', 'build', 'objdump'))
        parser.add_argument(
            '--time-trace', help='enable time trace, will generate json profile', action="store_true",)
        parser.add_argument(
            '--work-dir', help='current work directory', required=True)
        parser.add_argument(
            '--clang-tidy-path', help='clang-tidy path',)
        parser.add_argument(
            '--release-build', help='use release build compile flags', action="store_true")
        self.args = parser.parse_args()
        self.work_dir = self.args.work_dir
        os.chdir(self.work_dir)
        with open("{}/{}".format(self.work_dir, self.compile_commands_path), "r") as f:
            self.compile_commands_set = json.loads(f.read())

    def run(self):
        if self.args.type == 'syntax':
            self.run_recompile(True,)
        elif self.args.type == 'tidy':
            self.run_clang_tidy()
        elif self.args.type == 'build':
            self.run_recompile(False,)
        elif self.args.type == 'objdump':
            self.run_objdump()
        else:
            exit(-1)

    def run_clang_tidy(self):
        assert self.args.clang_tidy_path
        cmd = "{} -p {}/{} {}".format(
            self.args.clang_tidy_path,
            self.work_dir, self.compile_commands_path, self.args.file)

        stdout, stderr, status = run_cmd(cmd)
        if status:
            logger.info(stdout)
            logger.error(stderr)
            exit(status)

    def find_compile_block(self, file_path):
        for a in self.compile_commands_set:
            f = a.get("file")
            if f == file_path:
                return a
        raise Exception("{} not found in {}".format(
            file_path, self.compile_commands_path))

    def run_objdump(self):
        cmd: str = self.find_compile_block(self.args.file).get("command")
        cmd: list = cmd.split(' ')
        obj_index = cmd.index('-o')
        obj_file = None
        for i in range(obj_index+1, len(cmd)):
            if cmd[i]:
                obj_file = cmd[i]
                break
        assert obj_file.endswith(".o")

        saved_file = "/tmp/.tzg_objdump_{}.{}.txt".format(
            self.work_dir.replace('/', '.'),
            obj_file.replace('/', '.')
        )
        cmd = "objdump -C -r -d {}/{} > {}".format(
            self.work_dir, obj_file, saved_file)

        logger.info(".")
        logger.info("..")
        logger.info("...")
        logger.info("... Objdump `{}` ...".format(obj_file))
        _, stderr, status = run_cmd(cmd)
        if status:
            logger.error(stderr)
            exit(status)
        logger.info(
            "... Objdump of '{}' finished ...".format(obj_file))
        logger.info(
            "... Please open file: `{}`".format(saved_file))

    def run_recompile(self, syntax_only=True, ):
        suffix = " -fsyntax-only" if syntax_only else ""
        if self.args.time_trace:
            suffix += ' -ftime-trace'

        cmd: str = self.find_compile_block(self.args.file).get("command")

        if self.args.release_build:
            suffix += " -O3 -DNDEBUG -fno-verbose-asm -finline-functions"
            cmd = cmd.replace("-fprofile-instr-generate", "")
            cmd = cmd.replace("-fcoverage-mapping", "")

        logger.info(".")
        logger.info("..")
        logger.info("...")
        logger.info("... Compiling `{}` ...".format(self.args.file))
        _, stderr, status = run_cmd(cmd + suffix, )
        if status:
            logger.error(stderr)
            exit(-1)
        logger.info(
            "... Compilation of '{}' finished ...".format(self.args.file))


def main():
    try:
        Runner().run()
    except Exception as e:
        logger.error(e, exc_info=True)
        exit(-1)


if __name__ == '__main__':
    main()
