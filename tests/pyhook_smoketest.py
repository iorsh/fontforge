#!/usr/bin/env python
from __future__ import print_function

import sys
import os
print("SMOKE ENV", os.environ, flush=True)
print("SMOKE ORIG_ARG", sys.orig_argv, flush=True)
print("SMOKE VER", sys.version, flush=True)
print("SMOKE PATH", sys.path, flush=True)

import fontforge
import psMat
import pickle

print(fontforge.__version__, fontforge.version())

fontforge.font()

print(pickle.loads(pickle.dumps(fontforge.point())))
