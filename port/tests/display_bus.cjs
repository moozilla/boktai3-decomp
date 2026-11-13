'use strict';
const fs = require('node:fs');
const assert = require('node:assert/strict');
(async () => {
  const {instance:{exports:e}} = await WebAssembly.instantiate(fs.readFileSync(process.argv[2]), {});
  assert.equal(e.port_memory_init(), 0);
  e.port_test_display_setup();
  for (let line = 0; line <= 64; line++) assert.equal(e.port_run_scanline(), 0);
  assert.equal(e.port_test_display_vcount(), 64, 'VCount handler must read the current line through the bus');
  assert.equal(e.port_test_display_status() & 7, 4, 'VCount comparison is active before HBlank');
  e.port_devices_init(0, 0);
  e.port_bus_store_i16(0x04000100, 0xffff);
  e.port_bus_store_i16(0x04000102, 0xc0); // Timer 0 raises IF during the next advance.
  e.port_irq_raise(4); // Independent core-raised interrupt, delivery disabled.
  assert.equal(e.port_run_scanline(), 0);
  assert.equal(e.port_bus_load_i16(0x04000202), 12, 'device timer must preserve core-raised IF');
  e.port_irq_raise(0x80);
  e.port_bus_store_i16(0x04000202, 8);
  assert.equal(e.port_bus_load_i16(0x04000202), 0x84, 'W1C must preserve other core/device flags');
  console.log('Core/device bus: current-line IRQ reads, merged pending flags and selective W1C PASS');
})().catch(error => { console.error(error); process.exitCode = 1; });
