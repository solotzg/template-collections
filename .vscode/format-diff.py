#!/usr/bin/python3

import argparse
import os
import subprocess
from inner_utils import *

REPO_DIR = os.path.realpath(os.path.join(
    os.path.join(__file__, os.pardir), os.pardir))


def main():
    default_suffix = ['.cpp', '.h', '.cc', '.hpp']
    parser = argparse.ArgumentParser(description='Code Format',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '--repo_path', help='path of repository', default=REPO_DIR)
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
        if not os.path.exists(file_path):
            continue
        if ' ' in file_path:
            logger.warn('file {} can not be formatted'.format(file_path))
            continue
        files_to_format.append(file_path)

    if files_to_format:
        logger.info('Files to format:\n  {}\n'.format(
            '\n  '.join(files_to_format)))
        for file in files_to_format:
            cmd = 'clang-format -i {}'.format(file)
            if subprocess.Popen(cmd, shell=True, cwd=repo_path).wait():
                exit(-1)

        logger.info("Finish code format")
    else:
        logger.info('No file to format')


if __name__ == '__main__':
    main()
