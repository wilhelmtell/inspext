#! /usr/bin/env python
# encoding: utf-8

import Options, Utils

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='INSPext'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'

def set_options(opt):
    opt.tool_options('compiler_cc')
    opt.add_option('--run-tests', action='store_true', default=False,
            help='Run unit-tests')

def configure(conf):
    conf.check_tool('compiler_cc')
    conf.env.run_tests = Options.options.run_tests
    if conf.env.run_tests: # FIXME: should be in build step
        Utils.exec_command('cd test && ./register_new_tests.sh -q')

def build(bld):
    ftr = 'cc'
    if bld.env.run_tests:
        bld.add_subdirs('test')
        trg = 'objs'
        src = bld.path.ant_glob('*.c').split(' ')
        src.remove('main.c')
        src = ' '.join(src)
    else:
        ftr += ' cprogram'
        trg = 'inspc'
        src = bld.path.ant_glob('*.c')

    bld.new_task_gen(
        features = ftr,
        source = src,
        ccflags = ['-Wall', '-ansi', '-pedantic', '-g3'],
        target = trg)
