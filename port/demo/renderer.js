/* Browser/Node bridge to the portable C PPU. No game data is included. */
globalThis.createPpuDemo = async function (moduleBytes) {
  const {instance} = await WebAssembly.instantiate(moduleBytes, {});
  const api = instance.exports;
  const base = Number(api.__heap_base.value);
  const state = base, view = base + 32, io = base + 64;
  const sizes = [0x400, 0x400, 0x18000, 0x400];
  const output = io + sizes.reduce((a, b) => a + b);
  const required = output + 240 * 4;
  if (required > api.memory.buffer.byteLength)
    api.memory.grow(Math.ceil((required - api.memory.buffer.byteLength) / 65536));
  const bytes = new Uint8Array(api.memory.buffer), words = new DataView(api.memory.buffer);
  let address = io;
  sizes.forEach((size, i) => { words.setUint32(view + i * 4, address, true); address += size; });
  return {
    render(dump) {
      const data = dump instanceof Uint8Array ? dump : new Uint8Array(dump);
      const input = new DataView(data.buffer, data.byteOffset, data.byteLength);
      const magic = new TextDecoder().decode(data.subarray(0, 8));
      const delta = magic === 'PPUDUMP2';
      if (!delta && magic !== 'PPUDUMP1') throw Error('Not a PPU capture (expected PPUDUMP1 or PPUDUMP2).');
      const rgba = new Uint8ClampedArray(240 * 160 * 4);
      let cursor = 8;
      function need(count) { if (cursor + count > data.length) throw Error('Truncated PPU capture.'); }
      for (let y = 0; y < 160; ++y) {
        need(16);
        const start = cursor;
        cursor += 16;
        let mask = 15;
        if (delta) { need(1); mask = data[cursor++]; }
        if ((mask & ~15) || (y === 0 && mask !== 15)) throw Error('Invalid PPU bank mask.');
        let address = io;
        sizes.forEach((size, i) => {
          if (mask & (1 << i)) { need(size); bytes.set(data.subarray(cursor, cursor + size), address); cursor += size; }
          address += size;
        });
        if (y === 0) api.ppu_begin_frame(state, view);
        [0, 8, 4, 12].forEach((offset, i) => words.setInt32(state + i * 4, input.getInt32(start + offset, true), true));
        api.ppu_render_scanline_rgb888(state, view, y, output);
        for (let x = 0; x < 240; ++x) {
          const color = words.getUint32(output + x * 4, true), at = (y * 240 + x) * 4;
          rgba[at] = color & 255; rgba[at + 1] = (color >>> 8) & 255;
          rgba[at + 2] = (color >>> 16) & 255; rgba[at + 3] = 255;
        }
      }
      if (cursor !== data.length) throw Error('Unexpected trailing PPU capture data.');
      return rgba;
    }
  };
};
