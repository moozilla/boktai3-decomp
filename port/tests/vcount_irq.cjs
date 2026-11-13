'use strict';
const fs = require('node:fs');
const assert = require('node:assert/strict');
(async () => {
  const [modulePath, romPath] = process.argv.slice(2);
  const {instance:{exports:e}} = await WebAssembly.instantiate(fs.readFileSync(modulePath), {});
  assert.equal(e.port_memory_init(), 0);
  const bytes = new Uint8Array(e.memory.buffer), words = new DataView(e.memory.buffer);
  const rom = fs.readFileSync(romPath);
  bytes.set(rom, 0x08000000);
  assert.equal(e.port_load_rom(0x08000000, rom.length), 0);
  assert.equal(e.port_boot_early(), 0);
  e.port_vblank_irq_init();
  words.setUint32(0x03007ff0, 0x030054e0, true); // Valid inactive sound state.
  words.setUint32(0x03003a4c, 2, true); // Dialogue mode enables two-phase VCount.
  words.setUint32(0x03004268, 37, true);
  e.port_bus_store_i16(0x04000010, 100);
  e.port_bus_store_i16(0x04000200, 5); // VBlank and VCount.
  e.port_bus_store_i16(0x04000004, 0x4028); // Compare 64, VCount/VBlank enabled.
  for (let line = 0; line < 64; line++) assert.equal(e.port_run_scanline(), 0);
  assert.equal(e.port_vcount_irq_calls(), 0);
  assert.equal(e.port_run_scanline(), 0); // Line 64: switch comparison to 144.
  assert.equal(e.port_vcount_irq_calls(), 1);
  assert.equal(words.getUint32(0x03001110, true), 1);
  assert.equal(words.getUint16(0x04000004, true) >>> 8, 144);
  assert.equal(words.getUint16(0x04000010, true), 100);
  for (let line = 65; line < 144; line++) assert.equal(e.port_run_scanline(), 0);
  assert.equal(e.port_run_scanline(), 0); // Line 144: latch dialogue scroll.
  assert.equal(e.port_vcount_irq_calls(), 2);
  assert.equal(words.getUint32(0x03001110, true), 0);
  assert.equal(words.getUint16(0x04000004, true) >>> 8, 64);
  assert.equal(words.getUint16(0x04000010, true), 37);
  for (let line = 145; line < 228; line++) assert.equal(e.port_run_scanline(), 0);
  assert.equal(e.port_vblank_irq_calls(), 1);
  assert.equal(e.port_vcount_irq_calls(), 2, 'device HBlank must not duplicate VCount');
  assert.equal(words.getUint16(0x04000200, true), 5);
  assert.equal(words.getUint16(0x04000208, true), 1);
  assert.equal(words.getUint16(0x04000202, true) & 5, 0);
  console.log('Original VCount C: line 64/144 phase transitions, dialogue scroll and IRQ restoration PASS');
})().catch(error => { console.error(error); process.exitCode = 1; });
