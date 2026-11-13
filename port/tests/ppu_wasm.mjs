// Execute the linked renderer in Node. Optional arguments are capture folders.
import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';

const wasm = process.argv[2] ?? 'build/ppu/ppu.wasm';
const {instance} = await WebAssembly.instantiate(fs.readFileSync(wasm), {});
const api = instance.exports;
const base = Number(api.__heap_base.value);
const state = base, view = base + 32, io = base + 64;
const pal = io + 0x400, vram = pal + 0x400, oam = vram + 0x18000;
const output = oam + 0x400;
api.memory.grow(Math.ceil((output + 76800) / 65536));
const bytes = new Uint8Array(api.memory.buffer);
const words = new DataView(api.memory.buffer);
[io, pal, vram, oam].forEach((p, i) => words.setUint32(view + 4 * i, p, true));
words.setUint16(io, 0x80, true);
api.ppu_render_frame(state, view, output);
for (let i = 0; i < 38400; ++i) {
  if (words.getUint16(output + i * 2, true) !== 0x7fff) throw Error('forced blank smoke test failed');
}
console.log('wasm32 linked renderer: forced blank PASS');
let failures = 0;
const recordSize = 16 + 0x400 + 0x400 + 0x18000 + 0x400;
for (const folder of process.argv.slice(3)) {
  const names = new Set(fs.readdirSync(folder).filter(n => /\.ppu(\.gz)?$/.test(n)).map(n => n.split('.ppu')[0]));
  if (!names.size) throw Error(`no captures in ${folder}`);
  for (const name of [...names].sort()) {
    const raw = path.join(folder, name + '.ppu');
    const dump = fs.existsSync(raw) ? fs.readFileSync(raw) : zlib.gunzipSync(fs.readFileSync(raw + '.gz'));
    const magic = dump.subarray(0, 8).toString();
    const delta = magic === 'PPUDUMP2';
    if ((!delta && magic !== 'PPUDUMP1') || (!delta && dump.length !== 8 + 160 * recordSize)) throw Error(`bad dump ${name}`);
    let cursor = 8;
    const expected = fs.readFileSync(path.join(folder, name + '.ppm'));
    const header = Buffer.from('P6\n240 160\n255\n');
    if (!expected.subarray(0, header.length).equals(header)) throw Error(`bad PPM ${name}`);
    let different = 0;
    for (let y = 0; y < 160; ++y) {
      const start = cursor;
      if (cursor + 16 > dump.length) throw Error(`short line ${y}: ${name}`);
      cursor += 16;
      if (delta) {
        if (cursor >= dump.length) throw Error(`short mask ${y}: ${name}`);
        const mask = dump[cursor++];
        if ((mask & ~15) || (y === 0 && mask !== 15)) throw Error(`bad mask ${y}: ${name}`);
        let address = io;
        [0x400, 0x400, 0x18000, 0x400].forEach((size, i) => {
          if (mask & (1 << i)) {
            if (cursor + size > dump.length) throw Error(`short bank ${y}: ${name}`);
            bytes.set(dump.subarray(cursor, cursor + size), address);
            cursor += size;
          }
          address += size;
        });
      } else {
        bytes.set(dump.subarray(cursor, cursor + recordSize - 16), io);
        cursor += recordSize - 16;
      }
      if (y === 0) api.ppu_begin_frame(state, view);
      // File order is x2,y2,x3,y3; the public state groups x2,x3 then y2,y3.
      [0, 8, 4, 12].forEach((offset, i) => words.setInt32(state + i * 4, dump.readInt32LE(start + offset), true));
      api.ppu_render_scanline_rgb888(state, view, y, output);
      for (let x = 0; x < 240; ++x) {
        const color = words.getUint32(output + x * 4, true);
        const at = header.length + (y * 240 + x) * 3;
        if ((color & 255) !== expected[at] || ((color >>> 8) & 255) !== expected[at + 1] ||
            ((color >>> 16) & 255) !== expected[at + 2]) ++different;
      }
    }
    if (cursor !== dump.length) throw Error(`trailing dump data: ${name}`);
    console.log(`wasm32 ${name}: ${different} / 38400 pixel mismatches`);
    failures += different;
  }
}
process.exitCode = failures ? 1 : 0;
