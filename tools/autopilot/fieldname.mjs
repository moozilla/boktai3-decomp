#!/usr/bin/env node
// Name a byte offset inside a struct from types.json: node fieldname.mjs 'struct RoomActor' 0x60 0xe9 ...
import fs from 'node:fs';
import path from 'node:path';
const ROOT = path.resolve(path.dirname(new URL(import.meta.url).pathname), '../..');
const T = JSON.parse(fs.readFileSync(path.join(ROOT, 'build/port/agent/types.json'), 'utf8')).types;
const PRIM = { u8: 1, s8: 1, u16: 2, s16: 2, u32: 4, s32: 4, ptr: 4 };
const sizeOf = (t) => (typeof t === 'string' ? (PRIM[t] || (T[t] ? T[t].size : 4)) : t.k === 'ptr' ? 4 : t.k === 'array' ? t.n * sizeOf(t.of) : t.size || 4);
export function fieldName(t, off) {
  if (typeof t === 'string' && T[t] && T[t].fields) {
    let best = null;
    for (const f of T[t].fields) if (f.off <= off && off < f.off + sizeOf(f.type)) best = f;
    if (!best) return `+0x${off.toString(16)}`;
    const rest = fieldName(best.type, off - best.off);
    return `.${best.name}${rest}`;
  }
  if (t && t.k === 'array') { const es = sizeOf(t.of), i = Math.floor(off / es); return `[${i}]${fieldName(t.of, off - i * es)}`; }
  return off ? `+${off}` : '';
}
if (process.argv[1].endsWith('fieldname.mjs')) {
  const [t, ...offs] = process.argv.slice(2);
  for (const o of offs) console.log(o, fieldName(t, parseInt(o)));
}
