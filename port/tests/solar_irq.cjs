'use strict';
const fs = require('node:fs');
const assert = require('node:assert/strict');
(async () => {
  const {instance} = await WebAssembly.instantiate(fs.readFileSync(process.argv[2]), {});
  const e = instance.exports;
  assert.equal(e.port_memory_init(), 0);
  const words = new DataView(e.memory.buffer);
  e.port_solar_irq_init();
  assert.equal(words.getUint32(0x03006a4c, true), 0x3128);
  assert.equal(words.getInt32(0x03006a48, true), -1);
  assert.notEqual(words.getUint32(0x03003a18, true), 0, 'game C must install its portable callback');
  assert.equal(words.getUint16(0x04000200, true), 0x40);
  words.setUint32(0x03006a38, 1, true); // Start measurement, as the game does.
  assert.equal(e.port_run_frame(), 0);
  const calls = e.port_solar_irq_calls();
  assert(calls > 10 && calls < 30, `unexpected timer dispatch count: ${calls}`);
  assert.equal(words.getUint32(0x030035b4, true), calls, 'original handler advances the measurement counter');
  assert.equal(words.getUint16(0x04000202, true) & 0x40, 0, 'timer IF is acknowledged');
  // Exercise the other representation: a handler address read from ROM data.
  words.setUint32(0x03003a18, 0x08243d29, true);
  e.port_irq_raise(0x40);
  assert.equal(e.port_irq_dispatch(), 1);
  assert.equal(e.port_solar_irq_calls(), calls + 1);
  assert.equal(words.getUint32(0x030035b4, true), calls + 1);
  assert.equal(words.getUint16(0x04000208, true), 1, 'IME is restored');
  console.log(`Original Solar_InitTimer and Solar_OnTimerInterrupt: ${calls} timed calls/frame; portable and ROM IRQ resolution PASS`);
})().catch(error => { console.error(error); process.exitCode = 1; });
