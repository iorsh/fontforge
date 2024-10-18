#Needs: fonts/Ambrosia.sfd

import sys, fontforge

font=fontforge.open(sys.argv[1])

glyph_A = font["A"]
font.close()

# Try to access contents of deleted font
exception_occured = False
try:
    layer_A = glyph_A.layers[1]
    # The execution shall not reach this line
    assert(False)
except ValueError:
    exception_occured = True

assert(exception_occured)
