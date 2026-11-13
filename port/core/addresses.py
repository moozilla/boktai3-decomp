"""Bind GBA extern data addresses before wasm linking (wasm has no ABS symbols)."""
import csv
import re
import subprocess


def read_addresses(root):
    addresses = {}
    # The existing ROM ELF is read-only. No ROM bytes are emitted or embedded.
    elf = root / 'build/boktai3.elf'
    if elf.exists():
        result = subprocess.run(['arm-none-eabi-nm', '--defined-only', str(elf)],
                                text=True, capture_output=True, check=True)
        for line in result.stdout.splitlines():
            parts = line.split()
            if len(parts) == 3:
                try:
                    addresses[parts[2]] = int(parts[0], 16)
                except ValueError:
                    pass
    for filename in ('ram.csv', 'data.csv', 'functions.csv'):
        with (root / 'symbols' / filename).open() as stream:
            for row in csv.DictReader(stream):
                if row.get('name') and row.get('addr'):
                    addresses[row['name']] = int(row['addr'], 16)
    return addresses


def bind_ir(text, addresses):
    """Resolve only external data declarations; function symbols stay callable.

    A function declared as a byte array is intentionally a ROM-byte reference
    (e.g. the IWRAM copy), whereas an LLVM function declaration denotes C code.
    This is equivalent to the absolute-symbol binding in the GBA linker.
    """
    names = {}
    unresolved = []
    pattern = re.compile(r'^@([\w.$]+) = external\b[^\n]*$', re.M)
    for match in pattern.finditer(text):
        name = match[1]
        address = addresses.get(name)
        if address is None:
            raw = re.fullmatch(r'gUnk_(0[2389][0-9A-Fa-f]{6})', name)
            if raw:
                address = int(raw[1], 16)
        if address is not None and (0x02000000 <= address < 0x0a000000 or name in {'gNumMusicPlayers', 'gMaxLines'}):
            names[name] = address
        else:
            unresolved.append(name)
    text = pattern.sub(lambda m: '' if m[1] in names else m[0], text)
    text = re.sub(r'@([\w.$]+)',
                  lambda m: f'inttoptr (i32 {names[m[1]]} to ptr)' if m[1] in names else m[0], text)
    return text, names, unresolved
