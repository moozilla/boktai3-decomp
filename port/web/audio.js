// AudioWorklet: plays the game's PCM from the shared ring the worker fills,
// resampling from the MP2K mixer rate to the context rate.
class GameAudio extends AudioWorkletProcessor {
  constructor() {
    super();
    this.head = null;
    this.pos = 0;
    this.port.onmessage = (e) => {
      this.head = new Int32Array(e.data, 0, 4);
      this.data = new Float32Array(e.data, 16);
      this.frames = this.data.length / 2;
    };
  }

  process(inputs, outputs) {
    const out = outputs[0];
    const left = out[0], right = out[1] || out[0];
    if (!this.head) return true;
    const rate = Atomics.load(this.head, 2) || 13379;
    const step = rate / sampleRate;
    let read = Atomics.load(this.head, 1);
    const write = Atomics.load(this.head, 0);
    let available = (write - read + this.frames) % this.frames;
    // Keep latency bounded: skip ahead if the worker got far in front.
    if (available > rate / 4) {
      read = (write - Math.floor(rate / 10) + this.frames) % this.frames;
      available = Math.floor(rate / 10);
    }
    for (let i = 0; i < left.length; i++) {
      if (available < 2) { left[i] = right[i] = 0; continue; }
      const a = read, b = (read + 1) % this.frames, f = this.pos;
      left[i] = this.data[a * 2] * (1 - f) + this.data[b * 2] * f;
      right[i] = this.data[a * 2 + 1] * (1 - f) + this.data[b * 2 + 1] * f;
      this.pos += step;
      while (this.pos >= 1) {
        this.pos -= 1;
        read = (read + 1) % this.frames;
        available--;
      }
    }
    Atomics.store(this.head, 1, read);
    return true;
  }
}
registerProcessor('game-audio', GameAudio);
