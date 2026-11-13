#!/usr/bin/env python3
"""Measure a unit's local struct/union views with agbcc before unifying them.

  unit_layouts.py FILE.c... [--json]

Uses layoutmerge's compiler probes, including nested fields and bitfields.
Only file-scope types defined in the supplied source files are selected; types
from shared headers are already available through scope's fields command.
"""
import argparse
import json
from pathlib import Path

import layoutmerge


def inspect(path):
    text = layoutmerge.preprocess(str(path))
    if text is None:
        raise ValueError(f"cannot preprocess {path}")
    body = layoutmerge.strip_bodies(text)
    ast = layoutmerge.pycparser.CParser().parse(
        layoutmerge.pycparser_text(layoutmerge.strip_gnu(body)), str(path)
    )
    unit = layoutmerge.TU(ast)
    selected = []
    for (kind, name), node in sorted(unit.tags.items()):
        if node.coord and Path(node.coord.file).resolve() == Path(path).resolve():
            selected.append(f"{kind} {name}")
    # Add declarations that expose these local types to the existing probe engine.
    names = {f"__unit_layout_{index}" for index in range(len(selected))}
    declarations = "\n".join(
        f"extern {spelling} *__unit_layout_{index};" for index, spelling in enumerate(selected)
    )
    result = layoutmerge._scan_tu(str(path), text + "\n" + declarations + "\n", names)
    if "error" in result:
        raise ValueError(f"{path}: {result['error']}")
    return sorted(result["structs"].values(), key=lambda item: item["spell"])


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("files", nargs="+")
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()
    results = {path: inspect(path) for path in args.files}
    if args.json:
        print(json.dumps(results, indent=2))
    else:
        for path, structures in results.items():
            print(path)
            for structure in structures:
                size = structure["size"]
                print(f"  {structure['spell']} size={size:#x}" if size is not None else
                      f"  {structure['spell']} size=unavailable")
                for field in structure["leaves"]:
                    offset = field["off"]
                    at = f"{offset:04x}" if offset is not None else "????"
                    print(f"    {at} {field['name']} : {field['ctype']}")


if __name__ == "__main__":
    main()
