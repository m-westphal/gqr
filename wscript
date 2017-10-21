#! /usr/bin/env python
# encoding: utf-8
# Stefan Woelfl, 2008
# Matthias Westphal, 2009

import os.path
import os
from subprocess import Popen, PIPE
import Options

def get_version():
      # content of 'VERSION' always overrides git info
      if os.path.exists('VERSION'):
             version = file("VERSION").read().strip()
             return version

      try:
            p1 = Popen(["git", "log", "-1", "--pretty=format:%h"], stdout=PIPE, stderr=PIPE).communicate()[0]
            version = p1.strip()
      except:
            print "Version undefined."
            version = "undefined"
      return version

APPNAME='gqr'
VERSION=get_version()


## These two variables are mandatory ('/' are converted automatically)
srcdir      = '.'
blddir      = './_build_'

## Self-defined global variables
## will be selectable by options ...
default_install_data_dir = 'share/gqr/data'

def set_options(ctx):

      ctx.add_option('--devel',
		     help='build the debug and profile variants',
		     default = False, action="store_true", dest='build_devel')

      ctx.add_option('--enable-libgqr',
		     help='build a library with GQR\'s core functionality',
		     default = False, action="store_true", dest='build_library')

      ctx_optgroup = ctx.get_option_group('-b')
      # print ctx_optgroup
      ctx_optgroup.add_option('--data-dir',
                     type=str,
                     default=default_install_data_dir,
                     help='configure: data dir of gqr (relative to prefix) [Default: %s]' % default_install_data_dir,
                     dest='data_dir')


      ctx_optgroup.add_option('--relation-size',
		     type=int,
		     help='configure: support relation type with given number of base relations',
		     dest='relation_size')

      ctx_optgroup.add_option('--enable-xml',
                     default = False, action="store_true", dest = 'enable_xml', help='configure: enable XML support')
      ctx_optgroup.add_option('--enable-libgqr',
		     help='configure: build the gqr library',
		     default = False, action="store_true", dest='build_library')



      # ctx_optgroup = ctx.get_option_group('-h')

      ctx.sub_options('gqr')


def configure(conf):
      conf.env['GQR_VERSION'] = VERSION
      conf.env['PRG_DOXYGEN_PATH'] = conf.find_program("doxygen")
      conf.env['PRG_INSTALL_PATH'] = conf.find_program("install")

      conf.sub_config('gqr')
 

def build(conf):
      conf.add_subdirs('gqr data')


def check(ctx):
      import unittestw

      ut = unittestw.unit_test()
      ut.change_to_testfile_dir = False
      ut.want_to_see_test_output = False
      ut.run()
      ut.print_results()

def dist():
      if len(VERSION) == 0:
          raise SystemExit("Need version to build tarball")
      import Scripting
      # Excludes for "waf dist"
      Scripting.excludes.append('benchmarks')
      Scripting.excludes.append('doxygen')
      Scripting.excludes.append('results')
      Scripting.excludes.append('src-utils')
      Scripting.excludes.append('tools')
      Scripting.excludes.append('_build_')
      Scripting.excludes.append('gmon.out')
      Scripting.excludes.append('opra8')
      Scripting.excludes.append('opra8.spec')
      Scripting.excludes.append('libgqr.so')
      Scripting.excludes.append('generators') # data/rcc8/generators/rcc8comp.pl

      # global VERSION
      Scripting.dist(APPNAME, VERSION)


def distcheck():
      # global VERSION
      import Scripting
      Scripting.distcheck(APPNAME, VERSION)

def dist_hook():
      file = open('VERSION', 'w')
      file.write(VERSION)
      file.close()


def doxygen(ctx):
      "generates doxygen documentation"
      doxygen_conf = os.path.join('gqr', 'doxygen.cfg')
      try:
            Popen(['doxygen', doxygen_conf]).wait()
      except: 
            raise SystemExit('Install doxygen first.')
