#!/usr/bin/env python3
"""List function prefixes that differ from the owner named by their source directory."""
import argparse
from collections import defaultdict
from pathlib import Path
import re

from source_layout import ROOT, manifest, source_paths, symbol_names


TOKEN = re.compile(r'\b[A-Za-z_][A-Za-z_0-9]*\b')
NONCODE = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\x27(?:\\.|[^\x27\\])*\x27', re.S)


# Ignore ordinary C prototypes so an outside declaration adds no caller.
PROTOTYPE = re.compile(
    r'^[ \t]*(?:extern[ \t]+)?(?!(?:return|if|while|for|switch)\b)'
    r'(?:const[ \t]+)?(?:(?:struct|union|enum)[ \t]+\w+|[A-Za-z_]\w*)'
    r'[ \t*]+[A-Za-z_]\w*\s*\([^;{}]*\)\s*;', re.M)


def compact(name):
    return re.sub(r'[^a-z0-9]', '', name.lower())


def owner_matches(path, prefix):
    parts = Path(path).parent.parts[1:]
    if not parts:
        return compact(prefix).startswith(compact(Path(path).stem))
    unit = parts[-1]
    owners = [unit, ''.join(parts[-2:])]
    if parts[0] == 'player':
        owners.append('player')
    if parts[:2] == ('link', 'wireless'):
        owners.append('wireless' + unit)
    if parts[:2] == ('ui', 'pause'):
        owners.append('pause')
    if parts == ('ui', 'books', 'monster_book'):
        owners.append('bestiary')
    if len(parts) == 1:
        owners.extend(Path(path).stem.split('_'))
    family = compact(prefix)
    return any(family.startswith(compact(owner)) for owner in owners if owner)


def inventory():
    layout = manifest()
    names = symbol_names()
    groups = defaultdict(dict)
    by_name = {}
    for address, (path, _) in layout.files.items():
        name = names.get(address)
        if not name:
            continue
        prefix, sep, _ = name.partition('_')
        if not sep or not prefix or owner_matches(path, prefix):
            continue
        owner = str(Path(path).parent.relative_to('src'))
        groups[owner].setdefault(prefix, set()).add(name)
        by_name[name] = owner

    external = defaultdict(set)
    for path in source_paths():
        owner = str(path.parent.relative_to(Path(ROOT) / 'src'))
        text = NONCODE.sub(' ', path.read_text(errors='replace'))
        references = {word.removesuffix('__addr') for word in TOKEN.findall(PROTOTYPE.sub(' ', text))}
        for name in references & by_name.keys():
            if by_name[name] != owner:
                external[name].add(owner)
    return groups, external


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--directory', action='append', metavar='DIR',
                        help='report only a selected directory relative to src (repeatable)')
    args = parser.parse_args()
    groups, external = inventory()
    ordered = sorted(groups, key=lambda owner: (-sum(map(len, groups[owner].values())), owner))
    print('directory\tprefix\tfunctions\twith outside references\toutside directories')
    for owner in ordered:
        if args.directory and owner not in args.directory:
            continue
        families = groups[owner]
        for prefix, names in sorted(families.items(), key=lambda pair: (-len(pair[1]), pair[0])):
            users = set().union(*(external.get(name, set()) for name in names))
            print(owner, prefix, len(names), sum(name in external for name in names),
                  ','.join(sorted(users)) or '-', sep='\t')


if __name__ == '__main__':
    main()
