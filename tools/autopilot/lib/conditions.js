// Read the side-effect-free portion of stages.py's GCL conditions.
// Script_ReadTypedVariable (0821B20C) scales the array index by the type,
// except for bit fields, where it adds the encoded starting bit.
const variable = /^(?:(progress|entrance)|(r708|r700|gs)\[(0x[\da-f]+)(?:\.(\d+))?\]:t([1-9])(?:\(\s*\d+\s*,\s*(-?\d+)\s*\))?)/i;
const number = /^-?(?:0x[\da-f]+|\d+)/i;
const word = /^(not\b|&&|\|\||==|!=|<=|>=|[<>()])/;
const compare = {
  '==': (a, b) => a === b, '!=': (a, b) => a !== b,
  '<': (a, b) => a < b, '<=': (a, b) => a <= b,
  '>': (a, b) => a > b, '>=': (a, b) => a >= b,
};

function readVariable(match) {
  const [, alias, region, offset, bit, type, index = '0'] = match;
  const addr = alias || region === 'r708' ? g.types.globals.gGameVariables.addr
    : region === 'gs' ? g.types.globals.gGameState.addr : 0x02000700;
  const base = g.u32(addr);
  if (!base) return undefined;
  if (alias) return g.s16(base + (alias === 'progress' ? 0x3c : 0x70));
  const p = base + Number(offset), i = Number(index);
  switch (Number(type)) {
    case 1: case 6: return g.s16(p + i * 2);
    case 2: case 3: return g.u8(p + i);
    case 4: {
      const b = Number(bit ?? 0) + i;
      return (g.u8(p + (b >> 3)) >> (b & 7)) & 1;
    }
    case 8: return g.u16(p + i * 4);
    case 9: return g.u32(p + i * 4) | 0;
    default: return undefined;
  }
}

function condition(input) {
  const tokens = [];
  while ((input = input.trimStart())) {
    const v = variable.exec(input);
    const op = v || number.exec(input) || word.exec(input) || /^\{[^{}]*\}/.exec(input);
    if (!op) return undefined;
    const token = op[0];
    tokens.push(v ? { value: readVariable(v) } :
      number.test(token) ? { value: parseInt(token) } : token.startsWith('{') ? { value: undefined } : token);
    input = input.slice(token.length);
  }
  let cursor = 0;
  function atom() {
    const t = tokens[cursor++];
    if (t === '(') {
      const a = expression();
      if (tokens[cursor++] !== ')') throw new SyntaxError();
      return a;
    }
    if (t === 'not') {
      const a = atom();
      return a === undefined ? a : !a;
    }
    if (!t || typeof t !== 'object') throw new SyntaxError();
    return t.value;
  }
  function relation() {
    const a = atom();
    const test = compare[tokens[cursor]];
    if (!test) return a;
    cursor++;
    const b = atom();
    return a === undefined || b === undefined ? undefined : test(a, b);
  }
  function and() {
    let a = relation();
    while (tokens[cursor] === '&&') {
      cursor++;
      const b = relation();
      a = a === undefined || b === undefined
        ? (a === false || a === 0 || b === false || b === 0 ? false : undefined)
        : Boolean(a && b);
    }
    return a;
  }
  function expression() {
    let a = and();
    while (tokens[cursor] === '||') {
      cursor++;
      const b = and();
      a = a === undefined || b === undefined ? (a || b ? true : undefined) : Boolean(a || b);
    }
    return a;
  }
  try {
    const value = expression();
    return cursor === tokens.length && value !== undefined ? Boolean(value) : undefined;
  } catch (error) {
    if (!(error instanceof SyntaxError)) throw error;
    return undefined;
  }
}

exports.conditionsAllow = (conditions) => conditions.every((input) => condition(input) !== false);

// Keep physical doors even if all extracted branches are false at the current
// tick. The scripts may have created their traps under earlier conditions.
exports.doorBranches = (doors, key) => {
  const choices = new Map();
  for (const door of doors) {
    const flags = door.cond.map(condition);
    const rank = flags.includes(false) ? -1 : flags.includes(undefined) ? 0 : 1;
    const id = key(door);
    if (!choices.has(id) || choices.get(id).rank < rank) choices.set(id, { door, rank });
  }
  return Array.from(choices.values(), ({ door }) => door);
};
