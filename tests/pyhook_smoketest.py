#!/usr/bin/env python
from __future__ import print_function

import sys
import os
import sysconfig as sc
import distutils.sysconfig as d_sc

print("pysys paths", sys.path)

schemes = sc.get_scheme_names()
print("MX SCHEMES: ", schemes)

for s in schemes:
	print("MX CURRENT SCHEME: ", s)
	print("sysconfig paths", sc.get_paths(s))
	print("sysconfig path templates", sc.get_paths(s, expand=False))

print("distutils sysconfig paths", d_sc.get_python_lib(prefix='', plat_specific=True,standard_lib=False))

import fontforge
import psMat
import pickle

print("fontforge module path", os.path.abspath(fontforge.__file__))
print("psMat module path", os.path.abspath(psMat.__file__))

print(fontforge.__version__, fontforge.version())

fontforge.font()

print(pickle.loads(pickle.dumps(fontforge.point())))
