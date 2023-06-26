#!/usr/bin/env python3
import argparse
import os
try:
    import orjson as json
except ImportError:
    import json as json
from inner_utils import *


class Runner:
    def __init__(self):
        self.compile_commands_path = "compile_commands.json"
        parser = argparse.ArgumentParser(description="Compile source file by `{}`".format(self.compile_commands_path),
                                         formatter_class=argparse.ArgumentDefaultsHelpFormatter)
        parser.add_argument(
            '--file', help='absolute path of file to compile', required=True)
        parser.add_argument(
            '--type', help='build type', required=True, choices=('syntax', 'tidy', 'build', 'objdump', 'preprocess'))
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
        elif self.args.type == 'preprocess':
            self.run_recompile(False, True)
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

    def get_obj_file_path(self, file, ):
        cmd: str = self.find_compile_block(file).get("command")
        return self.get_obj_file_path_by_cmd(cmd)

    def get_obj_file_path_by_cmd(self, cmd: str):
        cmd: list = cmd.split(' ')
        obj_index = cmd.index('-o')
        obj_file = None
        for i in range(obj_index+1, len(cmd)):
            if cmd[i]:
                obj_file = cmd[i]
                break
        assert obj_file.endswith(".o")
        return os.path.join(self.work_dir, obj_file)

    def run_objdump(self):
        obj_file = self.get_obj_file_path(self.args.file)
        saved_file = "/tmp/.tzg_objdump_{}.txt".format(
            obj_file.replace('/', '.')
        )
        cmd = "objdump -C -r -d {} > {}".format(obj_file, saved_file)

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

    def run_recompile(self, syntax_only, preprocess=False, ):
        suffix = " -fsyntax-only" if syntax_only else ""
        if self.args.time_trace:
            suffix += ' -ftime-trace'
        if preprocess:
            suffix += ' -E'

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
        logger.info(
            "... Object file: `{}`".format(self.get_obj_file_path_by_cmd(cmd)))


def main():
    try:
        Runner().run()
    except Exception as e:
        logger.error(e, exc_info=True)
        exit(-1)


if __name__ == '__main__':
    main()
