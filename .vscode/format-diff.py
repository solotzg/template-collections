#!/usr/bin/python3
# Copyright 2022 PingCAP, Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import os
import subprocess
from os import path
import json

from inner_utils import *


def main():
    default_suffix = ['.cpp', '.h', '.cc', '.hpp']
    parser = argparse.ArgumentParser(description='Code Format',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--repo_path', help='path of repository',
                        default=path.realpath(path.join(__file__, '../..')))
    parser.add_argument('--suffix',
                        help='suffix of files to format, split by space', default=' '.join(default_suffix))
    parser.add_argument('--ignore_suffix',
                        help='ignore files with suffix, split by space')
    parser.add_argument(
        '--diff_from', help='commit hash/branch to check git diff', default='HEAD')

    args = parser.parse_args()
    default_suffix = args.suffix.strip().split(' ') if args.suffix else []
    ignore_suffix = args.ignore_suffix.strip().split(
        ' ') if args.ignore_suffix else []
    repo_path = args.repo_path
    if not os.path.isabs(repo_path):
        raise Exception("path of repo should be absolute")
    assert repo_path[-1] != '/'

    os.chdir(repo_path)
    files_to_check, _, status = run_cmd('git diff HEAD --name-only') if args.diff_from == 'HEAD' else run_cmd(
        'git diff {} --name-only'.format(args.diff_from))
    assert status == 0
    files_to_check = [os.path.join(repo_path, s.strip())
                      for s in files_to_check.split('\n') if s]
    files_to_format = []
    for f in files_to_check:
        if not any([f.endswith(e) for e in default_suffix]):
            continue
        if any([f.endswith(e) for e in ignore_suffix]):
            continue
        file_path = f
        if not path.exists(file_path):
            continue
        if ' ' in file_path:
            print('file {} can not be formatted'.format(file_path))
            continue
        files_to_format.append(file_path)

    if files_to_format:
        print('Files to format:\n  {}'.format('\n  '.join(files_to_format)))
        for file in files_to_format:
            cmd = 'clang-format -i {}'.format(file)
            if subprocess.Popen(cmd, shell=True, cwd=repo_path).wait():
                exit(-1)

        print("Finish code format")
    else:
        print('No file to format')


if __name__ == '__main__':
    main()
