"""
create cc_test if exist scrs
"""

load("@rules_cc//cc:defs.bzl", "cc_test")

def cc_test_if_exists(srcs, **kwargs):
    if native.glob(srcs):
        cc_test(srcs = srcs, **kwargs)
