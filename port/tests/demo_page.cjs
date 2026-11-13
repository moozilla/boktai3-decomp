/* Exercise the page's actual event handlers with the linked C module in Node. */
'use strict';
const fs = require('node:fs');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const {File} = require('node:buffer');
require('../demo/replay.js');
(async () => {
  const [wasmPath, romPath, capturePath, referencePath] = process.argv.slice(2);
  const elements = new Map();
  global.document = {getElementById(id) {
    if (!elements.has(id)) elements.set(id, {textContent:'', disabled:false,
      getContext() { return {putImageData(image) { this.image = image; }, getImageData() { return this.image; }}; }});
    return elements.get(id);
  }};
  global.ImageData = class {constructor(data, width, height) { Object.assign(this, {data, width, height}); }};
  vm.runInThisContext(fs.readFileSync('port/demo/viewer.js', 'utf8'));
  const event = (path, name) => ({target:{files:[new File([fs.readFileSync(path)], name)]}});
  await elements.get('wasm').onchange(event(wasmPath, 'boot.wasm'));
  assert.match(elements.get('status').textContent, /C module ready/);
  await elements.get('captures').onchange(event(capturePath, 'frame.ppu.gz'));
  assert.match(elements.get('status').textContent, /160 captured scanlines/);
  await elements.get('reference').onchange(event(referencePath, 'frame.ppm'));
  assert.match(elements.get('status').textContent, /0 \/ 38,400 pixels differ/);
  await elements.get('rom').onchange(event(romPath, 'game.gba'));
  assert.match(elements.get('status').textContent, /Original game C executed/);
  assert.equal(elements.get('frame').textContent, 'Early boot · forced blank');
  console.log('Page handlers: module, gzip capture, reference comparison and original boot C PASS');
})().catch(error => { console.error(error); process.exitCode = 1; });
