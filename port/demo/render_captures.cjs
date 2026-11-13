/* Render with the same module and parser used by the browser; write PPM files. */
'use strict';
const fs = require('node:fs');
const path = require('node:path');
const zlib = require('node:zlib');
const assert = require('node:assert/strict');
const {render} = require('./replay.js');
(async () => {
  const [modulePath, folder, destination] = process.argv.slice(2);
  if (!destination) throw Error('usage: node port/demo/render_captures.cjs MODULE CAPTURE_FOLDER OUTPUT_FOLDER');
  const module = await WebAssembly.compile(fs.readFileSync(modulePath));
  assert.deepEqual(WebAssembly.Module.imports(module), []);
  const api = (await WebAssembly.instantiate(module, {})).exports;
  assert.equal(api.port_memory_init(), 0);
  assert.equal(api.port_video_init(), 0);
  fs.mkdirSync(destination, {recursive:true});
  const names = fs.readdirSync(folder).filter(n => /\.ppu(\.gz)?$/.test(n)).sort();
  assert(names.length, 'no captures found');
  let different = 0;
  const results = [];
  for (const name of names) {
    let dump = fs.readFileSync(path.join(folder, name));
    if (name.endsWith('.gz')) dump = zlib.gunzipSync(dump);
    const rgba = render(api, dump), rgb = Buffer.alloc(115200);
    for (let p = 0; p < 38400; p++) rgb.set(rgba.subarray(p * 4, p * 4 + 3), p * 3);
    const stem = name.split('.ppu')[0];
    const output = Buffer.concat([Buffer.from('P6\n240 160\n255\n'), rgb]);
    fs.writeFileSync(path.join(destination, stem + '.wasm.ppm'), output);
    const reference = fs.readFileSync(path.join(folder, stem + '.ppm'));
    assert.equal(reference.length, output.length);
    let pixels = 0;
    for (let p = output.length - rgb.length; p < output.length; p += 3)
      if ([0, 1, 2].some(c => output[p + c] !== reference[p + c])) pixels++;
    results.push({scene:stem, mismatched_pixels:pixels}); different += pixels;
    console.log(`${stem}: ${pixels} / 38400 pixels differ from mGBA`);
  }
  // Parser guards must reject malformed input before it can be used as a frame.
  assert.throws(() => render(api, Buffer.from('PPUDUMP2')), /Truncated/);
  assert.throws(() => render(api, Buffer.from('NOTADUMP')), /Expected/);
  fs.writeFileSync(path.join(destination, 'comparison.json'), JSON.stringify(results, null, 2) + '\n');
  assert.equal(different, 0, 'wasm rendering differs from mGBA');
})().catch(error => { console.error(error); process.exitCode = 1; });
