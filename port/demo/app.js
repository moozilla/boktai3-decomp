'use strict';
(async () => {
  const element = id => document.getElementById(id);
  const status = element('status'), select = element('scene'), play = element('play');
  const actual = element('port').getContext('2d'), reference = element('reference').getContext('2d');
  let scenes = [], timer = null, generation = 0;
  function stop() { clearInterval(timer); timer = null; play.textContent = 'Play sequence'; }
  function readPpm(data) {
    const header = new TextEncoder().encode('P6\n240 160\n255\n');
    if (data.length !== header.length + 240 * 160 * 3 || header.some((b, i) => b !== data[i]))
      throw Error('Reference must be a 240 × 160 harness PPM.');
    const rgba = new Uint8ClampedArray(240 * 160 * 4);
    for (let i = 0; i < 38400; ++i) {
      rgba.set(data.subarray(header.length + i * 3, header.length + i * 3 + 3), i * 4);
      rgba[i * 4 + 3] = 255;
    }
    return rgba;
  }
  try {
    const module = Uint8Array.from(atob(globalThis.PPU_WASM_BASE64), c => c.charCodeAt(0));
    const renderer = await createPpuDemo(module);
    status.textContent = 'Ready. Open captures to render a scene.';
    function show() {
      const scene = scenes[Number(select.value)];
      if (!scene) return;
      try {
        const start = performance.now(), rgba = renderer.render(scene.dump);
        const elapsed = performance.now() - start;
        actual.putImageData(new ImageData(rgba, 240, 160), 0, 0);
        reference.clearRect(0, 0, 240, 160);
        let comparison = 'No reference selected';
        if (scene.reference) {
          reference.putImageData(new ImageData(scene.reference, 240, 160), 0, 0);
          let count = 0;
          for (let i = 0; i < rgba.length; i += 4)
            if (rgba[i] !== scene.reference[i] || rgba[i + 1] !== scene.reference[i + 1] || rgba[i + 2] !== scene.reference[i + 2]) ++count;
          comparison = `${count.toLocaleString()} / 38,400 pixel mismatches`;
        }
        status.textContent = `${scene.name} · ${elapsed.toFixed(1)} ms · ${comparison}`;
      } catch (error) { stop(); status.textContent = error.message; }
    }
    element('files').addEventListener('change', async event => {
      stop(); const request = ++generation;
      select.disabled = play.disabled = true;
      status.textContent = 'Reading local captures…';
      try {
        const files = [...event.target.files], loaded = [];
        const references = new Map(files.filter(f => f.name.endsWith('.ppm') && !f.name.endsWith('.port.ppm')).map(f => [f.name.slice(0, -4), f]));
        for (const file of files.filter(f => /\.ppu(\.gz)?$/.test(f.name)).sort((a, b) => a.name.localeCompare(b.name))) {
          let dump = new Uint8Array(await file.arrayBuffer());
          if (file.name.endsWith('.gz')) {
            if (!globalThis.DecompressionStream) throw Error('This browser needs uncompressed .ppu files.');
            dump = new Uint8Array(await new Response(new Blob([dump]).stream().pipeThrough(new DecompressionStream('gzip'))).arrayBuffer());
          }
          const name = file.name.replace(/\.ppu(\.gz)?$/, ''), ref = references.get(name);
          loaded.push({name, dump, reference: ref ? readPpm(new Uint8Array(await ref.arrayBuffer())) : null});
        }
        if (request !== generation) return;
        if (!loaded.length) throw Error('Select at least one .ppu or .ppu.gz capture.');
        scenes = loaded;
        select.replaceChildren(...scenes.map((scene, i) => new Option(scene.name, i)));
        select.disabled = false; play.disabled = scenes.length < 2; show();
      } catch (error) { if (request === generation) status.textContent = error.message; }
    });
    select.addEventListener('change', show);
    play.addEventListener('click', () => {
      if (timer) { stop(); return; }
      play.textContent = 'Pause';
      timer = setInterval(() => { select.value = (Number(select.value) + 1) % scenes.length; show(); }, 1000 / Number(element('speed').value));
    });
    element('speed').addEventListener('change', () => { if (timer) { stop(); play.click(); } });
  } catch (error) { status.textContent = `Renderer could not start: ${error.message}`; }
})();
