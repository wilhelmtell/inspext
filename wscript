#! /usr/bin/env python
# encoding: utf-8

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='INSPext'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'

def set_options(opt):
    opt.tool_options('compiler_cc')
    pass

def configure(conf):
    conf.check_tool('compiler_cc')
    # conf.check_tool('gcc')

def build(bld):
    bld.new_task_gen(
        features = 'cc cprogram',
        source = bld.path.ant_glob('**/*.c'),
        ccflags = ['-Wall', '-ansi', '-pedantic', '-g3'],
        target = 'inspc')
