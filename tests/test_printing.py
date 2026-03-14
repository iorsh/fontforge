"""Regression test for printing output across all print modes.

Needs: fonts/Ambrosia.sfd

This test generates both PostScript and PDF output for each print mode:
  - fontsample
  - multisize
  - fontdisplay
  - chars

It compares generated outputs against reference files and asserts byte-for-byte
identity.

To (re)generate references intentionally, set:
  FF_UPDATE_PRINTING_REFS=1
"""

import filecmp
import os
import shutil
import sys
import tempfile

import fontforge


def _make_deterministic_environment() -> None:
    os.environ["SOURCE_DATE_EPOCH"] = "1700000000"
    os.environ["USER"] = "fontforge-test"
    os.environ["TZ"] = "UTC"
    if hasattr(__import__("time"), "tzset"):
        __import__("time").tzset()


def _generate_outputs(font_path: str, out_dir: str) -> list[str]:
    font = fontforge.open(font_path)

    lorem_ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed" \
    " do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad" \
    " minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex" \
    " ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate" \
    " velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat" \
    " cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id" \
    " est laborum."

    jobs = [
        ("fontsample", 72, lorem_ipsum),
        ("waterfall", (24, 18, 12, 10), ""),
        ("fontdisplay", 24, ""),
        ("chars", 12, ""),
    ]

    generated = []
    for mode, pointsize, sample in jobs:
        ps_path = os.path.join(out_dir, f"{mode}.ps")
        pdf_path = os.path.join(out_dir, f"{mode}.pdf")

        fontforge.printSetup("ps-file")
        font.selection.select(("ranges",None),"A","Z")
        font.printSample(mode, pointsize, sample, ps_path)
        generated.append(ps_path)

        fontforge.printSetup("pdf-file")
        font.printSample(mode, pointsize, sample, pdf_path)
        generated.append(pdf_path)

    font.close()
    return generated


def _assert_identical_outputs(generated_files: list[str], refs_dir: str) -> None:
    missing_refs = []
    mismatches = []

    for generated in generated_files:
        basename = os.path.basename(generated)
        reference = os.path.join(refs_dir, basename)

        if not os.path.exists(reference):
            missing_refs.append(reference)
            continue

        if not filecmp.cmp(generated, reference, shallow=False):
            mismatches.append((generated, reference))

    if missing_refs:
        raise ValueError(
            "Missing printing reference files:\n"
            + "\n".join(missing_refs)
            + "\n\n"
            + "Generate references by setting FF_UPDATE_PRINTING_REFS=1 and rerunning test_printing.py."
        )

    if mismatches:
        details = [f"{gen} != {ref}" for gen, ref in mismatches]
        raise ValueError("Printing output differs from reference files:\n" + "\n".join(details))


def main() -> None:
    if len(sys.argv) < 2:
        raise ValueError("Expected input font path argument")

    font_path = sys.argv[1]
    script_dir = os.path.dirname(os.path.abspath(__file__))
    refs_dir = os.path.join(script_dir, "fonts", "printing_refs")
    update_refs = os.environ.get("FF_UPDATE_PRINTING_REFS") == "1"

    _make_deterministic_environment()

    with tempfile.TemporaryDirectory(prefix="ff-printing-test-") as temp_dir:
        generated_files = _generate_outputs(font_path, temp_dir)

        if update_refs:
            os.makedirs(refs_dir, exist_ok=True)
            for generated in generated_files:
                shutil.copyfile(generated, os.path.join(refs_dir, os.path.basename(generated)))

        _assert_identical_outputs(generated_files, refs_dir)


main()
