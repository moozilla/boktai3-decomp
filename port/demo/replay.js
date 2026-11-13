/* Shared by the local browser demo and Node comparison test. No ROM data. */
(function (root) {
  'use strict';
  const banks = [[0x04000000, 0x400], [0x05000000, 0x400],
                 [0x06000000, 0x18000], [0x07000000, 0x400]];
  function render(api, dump) {
    const magic = new TextDecoder().decode(dump.subarray(0, 8));
    if (!['PPUDUMP1', 'PPUDUMP2'].includes(magic)) throw Error('Expected a PPUDUMP1/2 capture');
    const delta = magic === 'PPUDUMP2';
    const source = new DataView(dump.buffer, dump.byteOffset, dump.byteLength);
    const memory = new Uint8Array(api.memory.buffer);
    let cursor = 8;
    function take(size) {
      if (cursor + size > dump.length) throw Error('Truncated PPU capture');
      const start = cursor; cursor += size; return start;
    }
    for (let y = 0; y < 160; y++) {
      const affine = take(16);
      const mask = delta ? dump[take(1)] : 15;
      if (mask & ~15 || (!y && mask !== 15)) throw Error('Invalid PPU bank mask');
      banks.forEach(([address, size], index) => {
        if (mask & (1 << index)) {
          const start = take(size);
          memory.set(dump.subarray(start, start + size), address);
        }
      });
      const values = [0, 4, 8, 12].map(offset => source.getInt32(affine + offset, true));
      if (api.port_video_capture_line(y, ...values)) throw Error('C renderer rejected scanline');
    }
    if (cursor !== dump.length) throw Error('Trailing PPU capture bytes');
    const colors = new Uint32Array(api.memory.buffer, api.port_video_capture_framebuffer(), 38400);
    const rgba = new Uint8ClampedArray(38400 * 4);
    colors.forEach((color, i) => {
      rgba[i * 4] = color & 255; rgba[i * 4 + 1] = (color >>> 8) & 255;
      rgba[i * 4 + 2] = (color >>> 16) & 255; rgba[i * 4 + 3] = 255;
    });
    return rgba;
  }
  root.PortReplay = {render};
  if (typeof module !== 'undefined') module.exports = root.PortReplay;
})(globalThis);
