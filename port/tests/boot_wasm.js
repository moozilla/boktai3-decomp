'use strict';
const fs = require('fs');
const assert = require('assert');
(async () => {
  const [modulePath, romPath, dumpPath] = process.argv.slice(2);
  const module = await WebAssembly.compile(fs.readFileSync(modulePath));
  assert.deepStrictEqual(WebAssembly.Module.imports(module), [], 'slice must have no unresolved imports');
  const instance = await WebAssembly.instantiate(module, {});
  const e = instance.exports;
  assert.strictEqual(e.memory.buffer.byteLength, 0x0a000000);
  assert.strictEqual(e.port_memory_init(), 0);
  const rom = fs.readFileSync(romPath);
  const bytes = new Uint8Array(e.memory.buffer);
  bytes.set(rom, 0x08000000);
  assert.strictEqual(e.port_load_rom(0x08000000, rom.length), 0);
  bytes.fill(0xa5, 0x03000000, 0x03008000);
  const words = new DataView(e.memory.buffer);
  words.setUint32(0x0201f400, 0x12345678, true);
  words.setUint32(0x0201f404, 0x87654321, true);
  assert.strictEqual(e.port_boot_early(), 0);
  assert.strictEqual(e.port_boot_call_count(), 2);
  assert.strictEqual(words.getUint32(0x0201f400, true), 0x12345678);
  assert.strictEqual(words.getUint32(0x0201f404, true), 0x87654321);
  assert.strictEqual(words.getUint32(0x03007d00, true), 0xa5a5a5a5);
  assert.strictEqual(words.getUint16(0x080000c8, true), 1);
  assert.strictEqual(bytes[0x030035c6], 0);
  const clock = Buffer.from(bytes.slice(0x03005430, 0x03005454));
  assert(clock.every(x => x === 0));
  fs.writeFileSync(dumpPath, clock);
  assert.strictEqual(e.port_boot_copy_iwram(), 0);
  assert.deepStrictEqual(bytes.slice(0x03000000, 0x0300110c), bytes.slice(0x08000350, 0x0800145c));
  if (e.port_devices_init) {
    e.port_devices_init(0, 0);
    bytes.fill(0x5a, 0x05000000, 0x05000400);
    bytes.fill(0x5a, 0x06000000, 0x06018000);
    bytes.fill(0x5a, 0x07000000, 0x07000400);
    e.InterruptsAndVideo_Initialize();
    assert(bytes.slice(0x05000000, 0x05000400).every(x => x === 0));
    assert(bytes.slice(0x06000000, 0x06018000).every(x => x === 0));
    for (let address = 0x07000000; address < 0x07000400; address += 4)
      assert.strictEqual(words.getUint32(address, true), 0xa0);
    assert.deepStrictEqual(bytes.slice(0x03003a10, 0x03003a44), bytes.slice(0x0824ffa8, 0x0824ffdc));
    assert.deepStrictEqual(bytes.slice(0x03003a60, 0x03004260), bytes.slice(0x080001f0, 0x080009f0));
    assert.strictEqual(words.getUint16(0x04000200, true), 0x2001);
    assert.strictEqual(words.getUint16(0x04000208, true), 1);
    e.port_devices_init(0, 0); // Reset IRQ enables before the handler-free timing check.
    console.log('wasm video init: original game C DMA clears and interrupt-table copy PASS');
  }
  e.SceneLifecycle_InitFrameCounterPointer();
  assert.strictEqual(words.getUint32(0x030053f8, true), 0x030025c0);
  if (e.port_video_init) {
    assert.strictEqual(e.port_video_init(), 0);
    words.setUint16(0x04000000, 0x80, true); // BIOS forced blank at frame 1.
  }
  e.port_timing_reset();
  assert.strictEqual(e.port_run_frame(), 0);
  assert.strictEqual(e.port_frame_count(), 1n);
  if (e.port_video_framebuffer) {
    const address = e.port_video_framebuffer();
    const frame = Buffer.from(bytes.slice(address, address + 240 * 160 * 2));
    for (let offset = 0; offset < frame.length; offset += 2)
      assert.strictEqual(frame.readUInt16LE(offset), 0x7fff);
    fs.writeFileSync(dumpPath + '.rgb555', frame);
    console.log('wasm PPU: 160 visible lines rendered through core/device hooks PASS');
  }
  console.log('wasm boot: original game RTC/clock/memory C, IWRAM copy and full frame PASS');
})().catch(error => { console.error(error); process.exit(1); });
