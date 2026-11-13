// Replay synthetic banks and HBlank events through the linked C renderer.
import fs from 'node:fs';
import path from 'node:path';
const {instance} = await WebAssembly.instantiate(fs.readFileSync(process.argv[2]), {});
const api = instance.exports, directory = process.argv[3];
const base = Number(api.__heap_base.value), state = base, view = base + 32, io = base + 64;
const sizes = [0x400, 0x400, 0x18000, 0x400], bankSize = sizes.reduce((a, b) => a + b);
const output = io + bankSize;
const required = output + 240 * 4;
if (api.memory.buffer.byteLength < required)
  api.memory.grow(Math.ceil((required - api.memory.buffer.byteLength) / 65536));
const memory = new Uint8Array(api.memory.buffer), words = new DataView(api.memory.buffer);
let address = io;
sizes.forEach((size, i) => { words.setUint32(view + i * 4, address, true); address += size; });
const rows = JSON.parse(fs.readFileSync(path.join(directory, 'results.json'), 'utf8'));
const header = Buffer.from('P6\n240 160\n255\n');
let failures = 0;
for (const row of rows) {
  const fixture = fs.readFileSync(path.join(directory, row.scene + '.bin'));
  const expected = fs.readFileSync(path.join(directory, row.scene + '.ppm'));
  if (fixture.length < bankSize || (fixture.length - bankSize) % 6) throw Error('Bad synthetic fixture');
  if (expected.length !== header.length + 38400 * 3 || !expected.subarray(0, header.length).equals(header)) throw Error('Bad reference');
  memory.set(fixture.subarray(0, bankSize), io);
  api.ppu_begin_frame(state, view);
  let next = bankSize, mismatches = 0;
  for (let y = 0; y < 160; ++y) {
    while (next < fixture.length && fixture.readUInt16LE(next) === y) {
      const offset = fixture.readUInt16LE(next + 2), value = fixture.readUInt16LE(next + 4);
      if (offset >= 0x56 || (offset & 1)) throw Error('Bad display register event');
      words.setUint16(io + offset, value, true);
      api.ppu_write_register(state, view, offset, 2);
      next += 6;
    }
    api.ppu_render_scanline_rgb888(state, view, y, output);
    for (let x = 0; x < 240; ++x) {
      const c = words.getUint32(output + x * 4, true), at = header.length + (y * 240 + x) * 3;
      if ((c & 255) !== expected[at] || ((c >>> 8) & 255) !== expected[at + 1] ||
          ((c >>> 16) & 255) !== expected[at + 2]) ++mismatches;
    }
  }
  if (next !== fixture.length) throw Error('Unconsumed register event');
  row.wasm_mismatches = mismatches;
  failures += mismatches;
  if (mismatches) console.error(`${row.scene}: ${mismatches} wasm pixel mismatches`);
}
fs.writeFileSync(path.join(directory, 'wasm-results.json'), JSON.stringify(rows, null, 2) + '\n');
console.log(`WebAssembly: ${rows.filter(r => !r.wasm_mismatches).length}/${rows.length} synthetic fixtures exact`);
process.exitCode = failures ? 1 : 0;
