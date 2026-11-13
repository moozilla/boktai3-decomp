// Measure the browser bridge (capture parsing, memory copy and C rendering).
// This is a local diagnostic, not a hardware-independent performance promise.
import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';
import '../demo/renderer.js';
const capture = process.argv[2], samples = Number(process.argv[3] ?? 300);
const modulePath = process.argv[4] ?? 'port/demo/ppu.wasm';
if (!capture || !Number.isInteger(samples) || samples < 1 || samples > 10000) {
  console.error('usage: node port/tests/ppu_benchmark.mjs capture.ppu[.gz] [samples] [module.wasm]');
  process.exit(1);
}
let dump = fs.readFileSync(capture);
if (capture.endsWith('.gz')) dump = zlib.gunzipSync(dump);
const renderer = await createPpuDemo(fs.readFileSync(modulePath));
for (let i = 0; i < 30; ++i) renderer.render(dump);
const times = [];
let checksum = 0;
for (let i = 0; i < samples; ++i) {
  const start = performance.now(), pixels = renderer.render(dump);
  times.push(performance.now() - start);
  checksum = (checksum + pixels[(i % 38400) * 4]) >>> 0;
}
times.sort((a, b) => a - b);
console.log(JSON.stringify({
  scene: path.basename(capture), target: 'Node WebAssembly browser bridge', samples,
  median_ms: times[Math.floor(samples * 0.5)], p95_ms: times[Math.floor(samples * 0.95)],
  mean_ms: times.reduce((a, b) => a + b, 0) / samples, checksum,
  includes: 'scanline capture parsing, memory copies, C rendering, and JS output allocation; excludes canvas display'
}, null, 2));
