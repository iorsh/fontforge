#!/usr/bin/env python
import sys
import sysconfig as sc
import distutils.sysconfig as d_sc

print("pysys paths", sys.path)
print("sysconfig paths", sc.get_paths())
print("distutils sysconfig paths", d_sc.get_python_lib(prefix='', plat_specific=True,standard_lib=False))

from __future__ import print_function
import fontforge
import psMat
import pickle

print(fontforge.__version__, fontforge.version())

fontforge.font()

print(pickle.loads(pickle.dumps(fontforge.point())))
