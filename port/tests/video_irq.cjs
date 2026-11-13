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
  e.port_devices_init(0, 0);
  e.Clock_CheckRtcDate();
  e.port_vblank_irq_init();
  // Valid, inactive sound state: the real m4aSoundVSync returns on its ident.
  // Wireless remains uncreated, so the real VBlank handler skips RFU work.
  words.setUint32(0x03007ff0, 0x030054e0, true);
  words.setUint32(0x0300426c, 1, true);
  words.setUint32(0x03003a48, 37, true);
  words.setUint32(0x03004280, 2, true);
  for (let i = 0; i < 8; i++) words.setUint16(0x03004c20 + i * 2, 100 + i, true);
  for (let frame = 1; frame <= 3; frame++) {
    assert.equal(e.port_run_frame(), 0);
    assert.equal(e.port_vblank_irq_calls(), frame);
    assert.equal(words.getUint32(0x03004264, true), frame);
    assert.equal(bytes[0x03005437], frame, 'original SunClock_TickFrame runs');
  }
  assert.equal(words.getUint32(0x03003a44, true), 1);
  assert.equal(words.getUint32(0x03004260, true), 1);
  assert.equal(words.getUint32(0x03004268, true), 37);
  assert.equal(words.getUint32(0x03003a4c, true), 2);
  for (let i = 0; i < 8; i++) assert.equal(words.getUint16(0x04000010 + i * 2, true), 100 + i);
  assert.equal(words.getUint16(0x03007ff8, true), 1);
  assert.equal(words.getUint16(0x04000202, true) & 1, 0);
  assert.equal(words.getUint16(0x04000208, true), 1);
  console.log('Original VBlank C: ROM handler resolution, BG scroll/latches, upload flag, clock ticks and BIOS latch PASS');
})().catch(error => { console.error(error); process.exitCode = 1; });
