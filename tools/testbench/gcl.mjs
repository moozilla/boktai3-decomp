// The small subset of the GCL encoding needed to invoke registry commands.
// The packet matches tools/showcase/gclasm.py Cmd; the game decodes it itself.
function le(value, bytes) {
  return Array.from({ length: bytes }, (_, i) => (value >>> (i * 8)) & 255);
}

function operand(value) {
  if (Number.isInteger(value) && value >= -0x80000000 && value <= 0xffffffff) {
    if (value >= -1 && value <= 62) return [0xc0 | (value + 1)];
    if (value >= -0x8000 && value <= 0x7fff) return [1, ...le(value, 2)];
    return [9, ...le(value, 4)];
  }
  for (const [kind, code] of [['hash', 6], ['proc', 8], ['text', 14]]) {
    if (Number.isInteger(value?.[kind]) && value[kind] >= 0 && value[kind] <= 0xffff)
      return [code, ...le(value[kind], 2)];
  }
  throw new TypeError(`Invalid GCL operand ${JSON.stringify(value)}`);
}

function packet(kind, bytes) {
  if (bytes.length <= 12) return [kind | bytes.length, ...bytes];
  if (bytes.length < 0x100) return [kind | 13, bytes.length, ...bytes];
  return [kind | 14, ...le(bytes.length, 2), ...bytes];
}

export function registryBlock(mode, key, args = [], options = {}) {
  if (!/^[\da-f]{4}$/i.test(key)) throw new Error(`Invalid registry key ${key}`);
  if (mode !== 'chara' && mode !== 'command') throw new Error(`Invalid registry mode ${mode}`);
  const params = [{ hash: parseInt(key, 16) }, ...args].flatMap(operand);
  if (params.length > 0x7fff) throw new Error('GCL argument list is too long');
  const length = params.length < 128 ? [params.length] : [0x80 | (params.length >> 8), params.length & 255];
  const body = [...le(mode === 'chara' ? 0x9906 : 0xb745, 2), ...length, ...params];
  for (const [letter, value] of Object.entries(options)) {
    if (!/^[!-~]$/.test(letter)) throw new Error(`Invalid GCL keyword ${letter}`);
    body.push(...packet(0x50, [letter.charCodeAt(0), ...[value].flat().flatMap(operand)]));
  }
  body.push(0);
  if (body.length > 0x1000) throw new Error('Registry command exceeds testbench scratch space');
  return Uint8Array.from([...packet(0x60, body), 0]);
}

export function scriptVariable(key) {
  const m = /^(r708|r700|gs):(0x[0-9a-f]+|\d+)(?:\.(\d+))?(?::t([1234689]))?$/i.exec(key);
  if (!m) throw new Error(`Invalid script variable ${key}; use r708:0x3c:t1 or r708:0x6e.1:t4`);
  const [, name, start, bit, type] = m;
  const bank = name.toLowerCase();
  const field = Number(type ?? (bit === undefined ? 1 : 4));
  const size = field === 9 ? 4 : [1, 6, 8].includes(field) ? 2 : 1;
  const limit = bank === 'gs' ? 0x10000 : 0x400;
  const bitIndex = Number(bit ?? 0);
  const offset = Number(start) + (bitIndex >> 3);
  if (offset + size > limit || (bit !== undefined && (field !== 4 || bitIndex > 15)))
    throw new Error(`Out of range script variable ${key}`);
  return { bank, offset, bit: bitIndex & 7, type: field, size };
}
