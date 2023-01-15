#!/usr/bin/python3
import logging
import sys
import time
import types
import datetime

logger = None


def _get_tz_offset():
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


def _init_logger():
    global logger

    tz_offset = _get_tz_offset()

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


_init_logger()
