'use strict';
(() => {
  const $ = id => document.getElementById(id);
  let api, frames = [], index = 0, timer, expected;
  const canvas = $('port'), context = canvas.getContext('2d');
  function status(message) { $('status').textContent = message; }
  function stop() { clearInterval(timer); timer = null; $('play').textContent = 'Play captures'; }
  function guard(action) { return async event => { try { await action(event); } catch (error) { stop(); status(error.message); } }; }
  async function loadModule(bytes) {
    stop();
    const module = await WebAssembly.compile(bytes);
    if (WebAssembly.Module.imports(module).length) throw Error('Module needs imports; choose the self-contained boot slice');
    const instance = await WebAssembly.instantiate(module, {});
    const candidate = instance.exports;
    if (!candidate.port_video_capture_line || candidate.port_memory_init() || candidate.port_video_init())
      throw Error('Expected the boot slice built with the PPU');
    api = candidate;
    status('C module ready. 160 MiB linear memory; no imports. Choose captures or a ROM.');
    $('next').disabled = $('play').disabled = !frames.length;
  }
  function show() {
    if (!api || !frames.length) return;
    const item = frames[index];
    const rgba = PortReplay.render(api, item.bytes);
    context.putImageData(new ImageData(rgba, 240, 160), 0, 0);
    $('frame').textContent = `${index + 1} / ${frames.length} · ${item.name}`;
    let message = '160 captured scanlines rendered by portable C in WebAssembly.';
    if (expected) {
      let changed = 0;
      for (let p = 0; p < 38400; p++) {
        if ([0, 1, 2].some(c => rgba[p * 4 + c] !== expected[p * 4 + c])) changed++;
      }
      message += `\nCurrent reference: ${changed.toLocaleString()} / 38,400 pixels differ.`;
    }
    status(message);
  }
  $('wasm').onchange = guard(async event => {
    if (event.target.files[0]) await loadModule(await event.target.files[0].arrayBuffer());
  });
  $('captures').onchange = guard(async event => {
    stop();
    const loaded = [];
    const files = [...event.target.files].sort((a, b) => a.name.localeCompare(b.name, undefined, {numeric:true}));
    let total = 0;
    for (const file of files) {
      if (file.size > 32 * 1024 * 1024) throw Error('Capture exceeds 32 MiB');
      let blob = file;
      if (file.name.endsWith('.gz')) {
        // Consume in bounded chunks so an invalid gzip cannot fill browser memory.
        const reader = file.stream().pipeThrough(new DecompressionStream('gzip')).getReader();
        const chunks = []; let size = 0;
        for (;;) {
          const {value, done} = await reader.read(); if (done) break;
          size += value.length;
          if (size > 17 * 1024 * 1024) { await reader.cancel(); throw Error('Expanded capture exceeds 17 MiB'); }
          chunks.push(value);
        }
        blob = new Blob(chunks);
      }
      total += blob.size;
      if (total > 256 * 1024 * 1024) throw Error('Select at most 256 MiB of expanded captures');
      loaded.push({name:file.name, bytes:new Uint8Array(await blob.arrayBuffer())});
    }
    frames = loaded; index = 0;
    $('next').disabled = $('play').disabled = !api || !frames.length;
    show();
  });
  $('next').onclick = guard(() => { stop(); index = (index + 1) % frames.length; show(); });
  $('play').onclick = guard(() => {
    if (timer) { stop(); return; }
    $('play').textContent = 'Pause';
    timer = setInterval(() => { try { index = (index + 1) % frames.length; show(); } catch (error) { stop(); status(error.message); } }, 1000 / 15);
  });
  $('reference').onchange = guard(async event => {
    const file = event.target.files[0]; if (!file) return;
    const target = $('mgba').getContext('2d');
    if (file.name.endsWith('.ppm')) {
      const bytes = new Uint8Array(await file.arrayBuffer());
      const header = 'P6\n240 160\n255\n';
      if (new TextDecoder().decode(bytes.subarray(0, header.length)) !== header || bytes.length !== header.length + 115200)
        throw Error('Expected a 240×160 binary mGBA PPM');
      expected = new Uint8ClampedArray(153600);
      for (let p = 0; p < 38400; p++) {
        expected.set(bytes.subarray(header.length + p * 3, header.length + p * 3 + 3), p * 4); expected[p * 4 + 3] = 255;
      }
      target.putImageData(new ImageData(expected, 240, 160), 0, 0);
    } else {
      const bitmap = await createImageBitmap(file);
      if (bitmap.width !== 240 || bitmap.height !== 160) { bitmap.close(); throw Error('Reference must be 240×160'); }
      target.drawImage(bitmap, 0, 0); bitmap.close();
      expected = target.getImageData(0, 0, 240, 160).data;
    }
    show();
  });
  $('rom').onchange = guard(async event => {
    stop(); if (!api) throw Error('Choose the compiled module first');
    const file = event.target.files[0]; if (!file) return;
    if (file.size < 0x145c || file.size > 0x02000000) throw Error('ROM must be between 0x145c bytes and 32 MiB');
    api.port_memory_reset();
    const bytes = new Uint8Array(api.memory.buffer);
    bytes.set(new Uint8Array(await file.arrayBuffer()), 0x08000000);
    if (api.port_load_rom(0x08000000, file.size) || api.port_boot_early() || api.port_boot_copy_iwram()) throw Error('Early boot failed');
    api.port_devices_init(0, 0);
    api.InterruptsAndVideo_Initialize();
    api.port_devices_init(0, 0);
    api.port_video_init();
    const view = new DataView(api.memory.buffer);
    view.setUint16(0x04000000, 0x80, true);
    api.port_timing_reset();
    if (api.port_run_frame()) throw Error('Frame timing failed');
    const colors = new Uint16Array(api.memory.buffer, api.port_video_framebuffer(), 38400);
    const rgba = new Uint8ClampedArray(153600);
    colors.forEach((c, i) => { for (let k = 0; k < 3; k++) { const v = (c >>> (k * 5)) & 31; rgba[i * 4 + k] = (v << 3) | (v >>> 2); } rgba[i * 4 + 3] = 255; });
    context.putImageData(new ImageData(rgba, 240, 160), 0, 0);
    $('frame').textContent = 'Early boot · forced blank';
    status('Original game C executed: RTC initialization → clock clear → memory clear.\nIWRAM payload copied; original video initialization performed DMA clears.\n228 scanlines advanced; PPU rendered the initial white frame. Gameplay boot remains in development.');
  });
  if (globalThis.PORT_WASM_BASE64) guard(async () => {
    const bytes = Uint8Array.from(atob(globalThis.PORT_WASM_BASE64), c => c.charCodeAt(0));
    await loadModule(bytes);
  })();
})();
