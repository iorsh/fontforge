#Needs: fonts/StrokeTests.sfd
import sys
import os
import time
import math
print("TEST1003 ENV", os.environ, flush=True)
print("TEST1003 ORIG_ARG", sys.orig_argv, flush=True)
print("TEST1003 VER", sys.version, flush=True)
print("TEST1003 PATH", sys.path, flush=True)

# del sys.path[0]
sys.path.insert(0, 'D:/a/fontforge/fontforge/repo/build/bin')
sys.path.insert(0, 'D:/a/fontforge/fontforge/repo/build/lib')
print("TEST1003 PATH2", sys.path, flush=True)
# for p in sys.path[0:1]:
for p in ["D:/a/fontforge/fontforge/repo/build/lib", "D:/a/fontforge/fontforge/repo/build/bin"]:   
   print("TEST1003 PATH LIST ", p, flush=True)
   time.sleep(3)
   print(os.system("ls -l {}".format(p)), flush=True)
   # print(os.system("find {} | grep -i 'fontforge\.'".format(p)))
   # print(os.system("find {} | grep -i 'psmat\.'".format(p)))
   time.sleep(3)

import psMat
import fontforge
from collections import OrderedDict

stroketests = sys.argv[1]
shortlist = ['D', 'Q', 'asciicircum']
convex = fontforge.unitShape(7).transform(psMat.scale(25))

sys.exit(1)

def do_stroke_test(short, *args, **kwargs):
    font=fontforge.open(stroketests)
    if short:
        font.selection.select(*shortlist)
    else:
        font.selection.all()
    font.stroke(*args, **kwargs)
    font.close()

# Full font tests
shapes = OrderedDict()
shapes['triangle'] = fontforge.unitShape(3).transform(psMat.scale(25))
shapes['square'] = fontforge.unitShape(4).transform(psMat.scale(25))
shapes['pentagon'] = fontforge.unitShape(5).transform(psMat.scale(25))
shapes['circular'] = (25,)
shapes['calligraphic'] = (25, 2, 45)

for sn, s in shapes.items():
    print(sn)
    if isinstance(s, tuple):
        do_stroke_test(False, sn, *s)
    else:
        do_stroke_test(False, 'polygonal', s)

# Argument tests
do_stroke_test(True, 'circular', 70)
do_stroke_test(True, 'circular', 30, "butt", "bevel")
do_stroke_test(True, 'elliptical', 30, 40, math.radians(45), "butt", "bevel")
do_stroke_test(True, 'elliptical', 50, 20, math.radians(25), "butt", "miter")
do_stroke_test(True, 'elliptical', 10, 100, 0, "butt", "miterclip")
do_stroke_test(True, 'eliptical', 40, 42, math.radians(20), "butt", "nib")
do_stroke_test(True, 'caligraphic', 30, 40, math.radians(45), "butt", "bevel")
do_stroke_test(True, 'rectangular', 50, 20, math.radians(25), "nib", "miter")
do_stroke_test(True, 'rectangular', 50, 20, math.radians(25), "nib", "round")
do_stroke_test(True, 'rectangular', 10, 100, 0, "round", "miterclip")
do_stroke_test(True, 'caligraphic', 40, 42, math.radians(20), "butt", "nib")
do_stroke_test(True, 'convex', convex, math.radians(45), "butt", "bevel")
do_stroke_test(True, 'polygonal', convex, math.radians(25), "nib", "miter")
do_stroke_test(True, 'convex', convex, 0, "round", "miterclip")
do_stroke_test(True, 'polygonal', convex, math.radians(20), "butt", "nib")

do_stroke_test(True, 'calligraphic', 20, 100)
do_stroke_test(True, 'calligraphic', 20, 100, angle=math.radians(30))
do_stroke_test(True, 'calligraphic', 20, 100, join="miterclip")
do_stroke_test(True, 'calligraphic', 20, 100, join="bevel", cap="round")
do_stroke_test(True, 'calligraphic', 20, 100, join="bevel", cap="round", removeinternal=True)
do_stroke_test(True, 'calligraphic', 20, 100, join="bevel", cap="round", removeexternal=True)
do_stroke_test(True, 'circular', 60, join="nib", cap="nib", extrema=False, angle=45)
do_stroke_test(True, 'calligraphic', 20, 100, join="bevel", cap="round", simplify=False)
do_stroke_test(True, 'calligraphic', 20, 100, join="bevel", cap="round", accuracy=3)
do_stroke_test(True, 'calligraphic', 20, 100, join="bevel", cap="round", removeoverlap="contour")
do_stroke_test(True, 'calligraphic', 20, 100, join="bevel", cap="round", removeoverlap="none")
do_stroke_test(True, 'calligraphic', 20, 100, join="miter", cap="round", joinlimit=4)
do_stroke_test(True, 'calligraphic', 20, 100, join="miterclip", cap="round", joinlimit=4)
do_stroke_test(True, 'calligraphic', 20, 100, join="miterclip", cap="round", joinlimit=180, jlrelative=False)
do_stroke_test(True, 'calligraphic', 20, 100, join="miterclip", cap="round", extendcap=3)
do_stroke_test(True, 'calligraphic', 20, 100, join="miterclip", cap="round", extendcap=100, ecrelative=False)
do_stroke_test(True, 'elliptical', 20, 20, join="arcs", cap="butt", joinlimit=4)
do_stroke_test(True, 'elliptical', 20, 100, join="arcs", cap="round", joinlimit=2, arcsclip="ratio")
