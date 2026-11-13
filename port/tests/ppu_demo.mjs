// Exercise the exact browser renderer bridge and bundled module in Node.
import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';
import '../demo/ppu-module.js';
import '../demo/renderer.js';
const module = Buffer.from(globalThis.PPU_WASM_BASE64, 'base64');
if (!module.equals(fs.readFileSync('port/demo/ppu.wasm'))) throw Error('Demo module copies differ');
const renderer = await createPpuDemo(module);
let count = 0;
for (const folder of process.argv.slice(2)) {
  for (const name of fs.readdirSync(folder).filter(n => n.endsWith('.ppu.gz')).sort()) {
    const dump = zlib.gunzipSync(fs.readFileSync(path.join(folder, name)));
    const rgba = renderer.render(dump);
    const expected = fs.readFileSync(path.join(folder, name.replace('.ppu.gz', '.ppm')));
    const header = Buffer.from('P6\n240 160\n255\n');
    if (expected.length !== header.length + 38400 * 3 || !expected.subarray(0, header.length).equals(header)) throw Error('Bad reference');
    for (let i = 0; i < 38400; ++i)
      for (let c = 0; c < 3; ++c)
        if (rgba[i * 4 + c] !== expected[header.length + i * 3 + c]) throw Error(`${name}: pixel ${i} differs`);
    // The browser accepts untrusted local captures: malformed input must fail.
    const invalid = [dump.subarray(0, 7), dump.subarray(0, dump.length - 1), Buffer.concat([dump, Buffer.from([0])])];
    if (dump.subarray(0, 8).toString() === 'PPUDUMP2') {
      for (const mask of [0, 16, 255]) {
        const bad = Buffer.from(dump); bad[24] = mask; invalid.push(bad);
      }
    }
    for (const bad of invalid) {
      let rejected = false;
      try { renderer.render(bad); } catch { rejected = true; }
      if (!rejected) throw Error('Malformed capture accepted');
    }
    ++count;
  }
}
if (!count) throw Error('Pass one or more capture directories');
console.log(`Browser bridge: ${count} exact frames; malformed captures rejected`);
