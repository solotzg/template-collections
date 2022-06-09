#!/usr/bin/python3
import argparse
import json
import os
import sys
import time
import logging
import types

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
def run_cmd(cmd, show_cmd=False):
    if show_cmd:
        logger.info("RUN CMD:\n\t{}\n".format(cmd))
    res = os.popen(cmd).readlines()
    return res


class Runner:
    def __init__(self):
        self.compile_commands_path = "compile_commands.json"
        parser = argparse.ArgumentParser(description="Compile source file by `{}`".format(self.compile_commands_path),
                                         formatter_class=argparse.ArgumentDefaultsHelpFormatter)
        parser.add_argument(
            '--file', help='absolute path of file to compile', required=True)
        parser.add_argument(
            '--type', help='build type', required=True, choices=('syntax', 'tidy', 'build'))
        parser.add_argument(
            '--time-trace', help='enable time trace', action='store_true')
        self.args = parser.parse_args()
        self.repo_path = os.getcwd()
        self.file_path = self.args.file
        self.compile_commands_set = json.load(
            open("{}/{}".format(self.repo_path, self.compile_commands_path), "r"))

    def run(self):
        if self.args.type == 'syntax':
            self.run_recompile_syntax(True)
        elif self.args.type == 'tidy':
            self.run_clang_tidy()
        elif self.args.type == 'build':
            self.run_recompile_syntax(False, self.args.time_trace)
        else:
            exit(-1)

    def run_clang_tidy(self):
        cmd = "clang-tidy"
        args = [
            "-p",
            "{}/{}".format(self.repo_path, self.compile_commands_path),
            self.file_path,
        ]
        res = run_cmd("{} {}".format(cmd, ' '.join(args)), show_cmd=True)
        logger.error(''.join(res))

    def run_recompile_syntax(self, syntax_only=True, time_trace=False):
        suffix = " -fsyntax-only" if syntax_only else ""
        if time_trace:
            suffix += " -ftime-trace"
        for a in self.compile_commands_set:
            f = a.get("file")
            if f != self.file_path:
                continue
            cmd = a.get("command")
            logger.info(".")
            logger.info("..")
            logger.info("...")
            logger.info("... Compiling `{}` ...".format(self.file_path))
            run_cmd(cmd + suffix, )
            logger.info(
                "... Compilation of '{}' finished ...".format(self.file_path))
            return
        logger.error("{} not found in {}".format(
            self.file_path, self.compile_commands_path))
        exit(-1)


def main():
    Runner().run()


if __name__ == '__main__':
    main()
