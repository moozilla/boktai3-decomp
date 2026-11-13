'use strict';
const fs = require('node:fs');
const assert = require('node:assert/strict');
(async () => {
  const {instance:{exports:e}} = await WebAssembly.instantiate(fs.readFileSync(process.argv[2]), {});
  assert.equal(e.port_memory_init(), 0);
  e.port_devices_init(0, 0);
  assert.equal(e.port_video_init(), 0);
  const view = new DataView(e.memory.buffer);
  const put16 = (address, value) => e.port_bus_store_i16(address, value);
  const put32 = (address, value) => { put16(address, value & 65535); put16(address + 2, value >>> 16); };
  put16(0x04000000, 0x0403); // Mode 3, BG2 enabled.
  put16(0x04000020, 0x100); // PA: one source pixel per output pixel.
  put16(0x04000026, 0x100); // PD: one source row per output row.
  put32(0x04000028, 0);
  put32(0x0400002c, 0);
  for (let x = 0; x < 240; x++) {
    view.setUint16(0x06000000 + x * 2, 0x001f, true);
    view.setUint16(0x06000000 + (240 + x) * 2, 0x03e0, true);
    view.setUint16(0x06000000 + (480 + x) * 2, 0x7c00, true);
  }
  view.setUint32(0x02000000, 0, true);
  put32(0x040000b0, 0x02000000);
  put32(0x040000b4, 0x0400002c);
  put16(0x040000b8, 1);
  put16(0x040000ba, 0xa740); // HBlank, repeat, word, fixed source/destination.
  assert.equal(e.port_run_frame(), 0);
  const frame = new Uint16Array(e.memory.buffer, e.port_video_framebuffer(), 38400);
  assert(frame.every(color => color === 0x001f), 'each identical DMA write must reload BG2Y before the next line');
  assert.equal(view.getUint32(0x0400002c, true), 0, 'DMA register value remains zero throughout');
  put16(0x040000ba, 0);
  assert.equal(e.port_run_frame(), 0);
  assert.equal(frame[0], 0x001f);
  assert.equal(frame[240], 0x03e0);
  assert.equal(frame[480], 0x7c00);
  console.log('Affine DMA: repeated identical HBlank writes reload PPU BG2Y; normal row stepping resumes when disabled PASS');
})().catch(error => { console.error(error); process.exitCode = 1; });
