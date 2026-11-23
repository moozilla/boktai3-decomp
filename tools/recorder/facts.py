"""Static facts of the recorder build: sites, functions, C lines and their source text.

port/build_game.py --record writes, per unit, build/port/game-rec/rec/UNIT.facts.json
(tools/recorder/instrument.py): every instrumented function's blocks and instructions,
with the C line and column clang attached and the site id of each logged operation.
A site id is kind (3 bits) | unit (13) | local (16).

DILocation lines count lines of the *prepared* source (build/port/game-rec/prepared/,
after its `#line 1` header). Preparation inserts a few lines (hoisted prototypes,
forward declarations), so they are mapped back to the original file by aligning
the two texts; a line that only exists in the prepared copy maps to None.
"""
import difflib
import functools
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GAME = ROOT / 'build/port/game-rec'
REC = GAME / 'rec'

K_BB, K_LOAD, K_STORE, K_WIDE, K_CPY, K_SET, K_AGG, K_FN = range(8)
KIND_NAMES = ['bb', 'load', 'store', 'wide', 'copy', 'fill', 'aggregate', 'function']


def kind_of(site):
    return site >> 29


def unit_of(site):
    return (site >> 16) & 0x1FFF


class Inst:
    __slots__ = ('fn', 'block', 'index', 'dest', 'text', 'line', 'col', 'site')

    def __init__(self, fn, block, index, row):
        self.fn, self.block, self.index = fn, block, index
        self.dest, self.text, self.line, self.col, self.site = row

    @property
    def opcode(self):
        body = self.text.split(' = ', 1)[1] if self.dest and ' = ' in self.text else self.text
        return body.split(' ', 1)[0]

    def __repr__(self):
        return f'<{self.fn.name}:{self.block.label}#{self.index} {self.text}>'


class Block:
    __slots__ = ('fn', 'index', 'label', 'site', 'insts')

    def __init__(self, fn, index, raw):
        self.fn, self.index, self.label, self.site = fn, index, raw['label'], raw['site']
        self.insts = [Inst(fn, self, i, row) for i, row in enumerate(raw['insts'])]


class Function:
    def __init__(self, unit, raw):
        self.unit = unit
        self.name = raw['name']
        self.line = raw['line']
        self.params = raw['params']          # [[type, ssa name], ...]
        self.vars = raw['vars']              # ssa name -> C variable
        self.site = raw['site']
        self.blocks = [Block(self, i, b) for i, b in enumerate(raw['blocks'])]
        self.by_label = {b.label: b for b in self.blocks}
        self.defs = {}
        for b in self.blocks:
            for inst in b.insts:
                if inst.dest:
                    self.defs[inst.dest] = inst

    @property
    def file(self):
        return self.unit.name

    def __repr__(self):
        return f'<fn {self.name} ({self.unit.name})>'


class Unit:
    def __init__(self, index, name, raw):
        self.index, self.name = index, name
        self.functions = [Function(self, f) for f in raw['functions']]
        self.sites = {}
        for fn in self.functions:
            for b in fn.blocks:
                self.sites[b.site & 0xFFFF] = b
                for inst in b.insts:
                    if inst.site is not None:
                        self.sites[inst.site & 0xFFFF] = inst
            self.sites[fn.site & 0xFFFF] = fn   # the entry block shares it; FN events mean the function


class Facts:
    def __init__(self, rec=REC):
        self.rec = Path(rec)
        self.units = json.loads((self.rec / 'units.json').read_text())
        self._units = {}

    def unit(self, index):
        u = self._units.get(index)
        if u is None:
            name = self.units[index]
            path = self.rec / (name.rsplit('.', 1)[0] + '.facts.json') if name.endswith('.c') else self.rec / (name + '.facts.json')
            if not path.exists():
                path = self.rec / (name + '.facts.json')
            raw = json.loads(path.read_text()) if path.exists() else {'functions': []}
            u = self._units[index] = Unit(index, name, raw)
        return u

    def site(self, sid):
        """The Function (FN site), Block (BB site) or Inst (memory site) of SID."""
        return self.unit(unit_of(sid)).sites.get(sid & 0xFFFF)

    def function_of(self, sid):
        s = self.site(sid)
        if s is None:
            return None
        if isinstance(s, Function):
            return s
        return s.fn

    # ---- source text ------------------------------------------------------
    @functools.lru_cache(maxsize=512)
    def line_map(self, unit_name):
        """prepared line number (1-based, after the #line header) -> original line, or None."""
        prepared = GAME / 'prepared' / unit_name
        original = ROOT / unit_name
        if not prepared.exists() or not original.exists():
            return None
        p = prepared.read_text().splitlines()[1:]
        o = original.read_text().splitlines()
        mapping = {}
        sm = difflib.SequenceMatcher(None, p, o, autojunk=False)
        for a, b, size in sm.get_matching_blocks():
            for k in range(size):
                mapping[a + k + 1] = b + k + 1
        return mapping

    @functools.lru_cache(maxsize=512)
    def source_lines(self, unit_name):
        path = ROOT / unit_name
        return path.read_text().splitlines() if path.exists() else []

    def original_line(self, unit_name, line):
        if not line:
            return None
        m = self.line_map(unit_name)
        if m is None:
            return line
        return m.get(line)

    def source(self, inst):
        """(file, original line, source text) of an instruction."""
        unit = inst.fn.unit.name
        line = self.original_line(unit, inst.line)
        text = None
        if line:
            lines = self.source_lines(unit)
            if 0 < line <= len(lines):
                text = lines[line - 1]
        return unit, line, text


LVALUE_END = re.compile(r'\s*(?:[-+*/%&|^]|<<|>>)?=(?!=)|\s*(?:\+\+|--)')


def lvalue_text(text, col):
    """The C lvalue left of the assignment operator at COL (1-based) in TEXT, best effort."""
    if not text or not col or col > len(text) + 1:
        return None
    i = col - 1
    # the store's column is the operator (`=`, `+=`, `++`); for `x++` it may be the operand
    j = i
    while j > 0 and text[j - 1] == ' ':
        j -= 1
    depth = 0
    k = j
    while k > 0:
        ch = text[k - 1]
        if ch in ')]':
            depth += 1
        elif ch in '([':
            if depth == 0:
                break
            depth -= 1
        elif depth == 0 and (ch in ';{},?:=!<>&|+%^~' or (ch == '-' and text[k:k + 1] != '>') or ch == '/'):
            if ch == '-' and k >= 1 and text[k:k + 1] == '>':
                pass
            elif ch == '>' and k >= 2 and text[k - 2] == '-':
                k -= 2
                continue
            else:
                break
        k -= 1
    lv = text[k:j].strip()
    lv = re.sub(r'^(?:return|if|while|else)\b\s*', '', lv)
    return lv or None
