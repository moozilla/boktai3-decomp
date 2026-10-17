// A small MP2K ("Sappy"/m4a) player for the Studio: plays a song from the ROM
// bytes /api/sound/song/N serves (tools/studio/sound.py), following the driver
// in src/lib/m4a.c (pokeemerald's m4a.c): the sequencer runs per frame
// (tempo accumulator, 150 per tick), notes go to DirectSound channels with
// the m4a ADSR envelope, or to CGB square/wave/noise channels.
//
// Approximations: no reverb, CGB envelopes and noise pitch are simplified,
// no channel priority beyond a 12-voice DirectSound limit (m4aSoundInit's
// MAXCHN 12), and linear interpolation instead of the GBA's 21024 Hz mixer.
'use strict';

const MP2K = (() => {
  const FRAME_HZ = 59.7275, MIXER_HZ = 21024, MAX_DS = 12;
  const CLOCK = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    28, 30, 32, 36, 40, 42, 44, 48, 52, 54, 56, 60, 64, 66, 68, 72, 76, 78, 80, 84, 88, 90, 92, 96];
  const DUTY = [0.125, 0.25, 0.5, 0.75];

  // the ROM ranges a song reads, as one sparse memory
  class Mem {
    constructor(chunks) {
      this.c = chunks.map(([a, b64]) => { const s = atob(b64), u = new Uint8Array(s.length); for (let i = 0; i < s.length; i++) u[i] = s.charCodeAt(i); return [parseInt(a, 16), u]; }).sort((x, y) => x[0] - y[0]);
      this.last = this.c[0];
    }
    find(a) {
      const l = this.last; if (l && a >= l[0] && a < l[0] + l[1].length) return l;
      let lo = 0, hi = this.c.length - 1;
      while (lo <= hi) { const m = (lo + hi) >> 1, [s, u] = this.c[m]; if (a < s) hi = m - 1; else if (a >= s + u.length) lo = m + 1; else return this.last = this.c[m]; }
      return null;
    }
    u8(a) { const c = this.find(a); return c ? c[1][a - c[0]] : 0; }
    s8(a) { const v = this.u8(a); return v > 127 ? v - 256 : v; }
    u32(a) { return (this.u8(a) | this.u8(a + 1) << 8 | this.u8(a + 2) << 16 | this.u8(a + 3) << 24) >>> 0; }
    has(a) { return !!this.find(a); }
  }

  function tone(mem, a) {
    return { addr: a, type: mem.u8(a), key: mem.u8(a + 1), length: mem.u8(a + 2), panSweep: mem.u8(a + 3), wav: mem.u32(a + 4), extra: mem.u32(a + 8),
      attack: mem.u8(a + 8), decay: mem.u8(a + 9), sustain: mem.u8(a + 10), release: mem.u8(a + 11) };
  }
  const isRom = v => v >= 0x08000000 && v < 0x0A000000;

  // a DirectSound sample: {rate, loop, loopStart, size, data(Float32Array)}
  const waveCache = new WeakMap();
  function wave(mem, a) {
    let m = waveCache.get(mem); if (!m) waveCache.set(mem, m = new Map());
    if (m.has(a)) return m.get(a);
    let w = null;
    if (mem.has(a) && mem.has(a + 16)) {
      const size = mem.u32(a + 12), loop = !!(mem.u8(a + 3) & 0x40);
      if (size < 0x400000) {
        const d = new Float32Array(size + 1);
        for (let i = 0; i < size; i++) d[i] = mem.s8(a + 16 + i) / 128;
        d[size] = loop ? d[Math.min(mem.u32(a + 8), size - 1)] : 0;
        w = { rate: mem.u32(a + 4) / 1024, loop, loopStart: mem.u32(a + 8), size, data: d };
      }
    }
    m.set(a, w);
    return w;
  }
  function cgbWave(mem, a) {
    const d = new Float32Array(32);
    for (let i = 0; i < 16; i++) { const b = mem.u8(a + i); d[2 * i] = ((b >> 4) - 7.5) / 7.5; d[2 * i + 1] = ((b & 15) - 7.5) / 7.5; }
    return d;
  }

  class Player {
    // data: {song: "08xxxxxx", chunks}; rate: output sample rate
    constructor(data, rate = 48000) {
      this.mem = new Mem(data.chunks);
      this.rate = rate;
      this.head = parseInt(data.song, 16);
      const m = this.mem;
      this.ntracks = m.u8(this.head);
      this.priority = m.u8(this.head + 2);
      this.reverb = m.u8(this.head + 3);
      this.vg = m.u32(this.head + 4);
      this.volume = 1;
      this.loop = true;
      this.reset();
    }
    reset() {
      const m = this.mem;
      this.tracks = [];
      for (let i = 0; i < this.ntracks; i++) this.tracks.push(this.newTrack(m.u32(this.head + 8 + 4 * i), i));
      this.chans = [];
      this.tempoD = 150; this.tempoI = 150; this.tempoC = 0;
      this.frames = 0; this.ticks = 0; this.frac = 0;
      this.looped = 0; this.fade = 0; this.done = false;
      this.onNote = null;
    }
    newTrack(ptr, i) {
      const keep = this.tracks?.[i];
      return { i, ptr, start: ptr, wait: 0, running: 0, stack: [], repN: 0, key: 60, vel: 127, gate: 0, tone: null,
        keyShift: 0, vol: 100, pan: 0, bend: 0, bendRange: 2, tune: 0, lfoSpeed: 22, lfoDelay: 0, lfoDelayC: 0, lfoSpeedC: 0,
        mod: 0, modT: 0, modM: 0, prio: 0, on: true, loops: 0, mute: keep?.mute || false, solo: keep?.solo || false, last: -1 };
    }
    get anySolo() { return this.tracks.some(t => t.solo); }

    // ---- sequencer ----
    tick() {
      for (const c of this.chans) if (c.gate > 0 && --c.gate === 0) c.stop = true;
      for (const t of this.tracks) {
        if (!t.on) continue;
        let guard = 0;
        while (t.wait === 0 && t.on && guard++ < 10000) this.step(t);
        if (t.wait > 0) t.wait--;
        if (t.lfoSpeed && t.mod) {
          if (t.lfoDelayC) t.lfoDelayC--;
          else {
            t.lfoSpeedC = (t.lfoSpeedC + t.lfoSpeed) & 255;
            const s = t.lfoSpeedC - 64 < 0 ? (t.lfoSpeedC << 24 >> 24) : 128 - t.lfoSpeedC;
            t.modM = (t.mod * s) >> 6;
          }
        }
      }
      this.ticks++;
    }
    step(t) {
      const m = this.mem;
      let cmd = m.u8(t.ptr);
      if (cmd < 0x80) cmd = t.running; else { t.ptr++; if (cmd >= 0xBD) t.running = cmd; }
      if (cmd >= 0xCF) return this.note(t, cmd);
      if (cmd <= 0xB0) { if (cmd >= 0x80) t.wait = CLOCK[cmd - 0x80]; return; }
      const a = m.u8(t.ptr);
      switch (cmd) {
        case 0xB1: t.on = false; this.release(t); break;                                  // FINE
        case 0xB2: { const p = m.u32(t.ptr); if (p <= t.ptr) { t.loops++; this.onLoop(t); } t.ptr = p; break; } // GOTO
        case 0xB3: if (t.stack.length < 3) { t.stack.push(t.ptr + 4); t.ptr = m.u32(t.ptr); } else t.ptr += 4; break; // PATT
        case 0xB4: if (t.stack.length) t.ptr = t.stack.pop(); break;                        // PEND
        case 0xB5: {                                                                        // REPT
          if (a === 0) { t.ptr = m.u32(t.ptr + 1); t.loops++; this.onLoop(t); }
          else if (++t.repN < a) t.ptr = m.u32(t.ptr + 1);
          else { t.repN = 0; t.ptr += 5; }
          break;
        }
        case 0xB9: t.ptr += 3; break;                                                       // MEMACC
        case 0xBA: t.prio = a; t.ptr++; break;
        case 0xBB: this.tempoD = a * 2; this.tempoI = this.tempoD; t.ptr++; break;         // TEMPO (bpm/2)
        case 0xBC: t.keyShift = a << 24 >> 24; t.ptr++; break;
        case 0xBD: t.tone = tone(m, this.vg + 12 * a); t.program = a; t.ptr++; break;      // VOICE
        case 0xBE: t.vol = a; t.ptr++; break;
        case 0xBF: t.pan = a - 64; t.ptr++; break;
        case 0xC0: t.bend = a - 64; t.ptr++; break;
        case 0xC1: t.bendRange = a; t.ptr++; break;
        case 0xC2: t.lfoSpeed = a; t.ptr++; break;
        case 0xC3: t.lfoDelay = a; t.ptr++; break;
        case 0xC4: t.mod = a; if (!a) t.modM = 0; t.ptr++; break;
        case 0xC5: t.modT = a; t.ptr++; break;
        case 0xC8: t.tune = a - 64; t.ptr++; break;
        case 0xCD: t.ptr += 2; break;                                                       // XCMD
        case 0xCE: {                                                                        // EOT
          let k = t.key;
          if (m.u8(t.ptr) < 0x80) { k = t.key = m.u8(t.ptr); t.ptr++; }
          for (const c of this.chans) if (c.track === t && c.key === k && !c.stop) c.stop = true;
          break;
        }
        default: break;                                                                     // C6, C7, C9-CC
      }
    }
    onLoop(t) {
      if (t.loops > this.looped) { this.looped = t.loops; if (!this.loop && !this.fade) this.fade = 1; }
    }
    release(t) { for (const c of this.chans) if (c.track === t) c.stop = true; }
    note(t, cmd) {
      const m = this.mem;
      let gate = CLOCK[cmd - 0xCF];
      if (m.u8(t.ptr) < 0x80) { t.key = m.u8(t.ptr++); if (m.u8(t.ptr) < 0x80) { t.vel = m.u8(t.ptr++); if (m.u8(t.ptr) < 0x80) gate += m.u8(t.ptr++); } }
      if (cmd === 0xCF) gate = 0;
      this.play(t, t.tone, t.key, t.vel, gate);
    }
    // start a note on track t with voice tn
    play(t, tn, key, vel, gate) {
      if (!tn) return;
      const m = this.mem;
      let pan = 0, k2 = key;
      if (tn.type & 0x40) {                          // keysplit
        const idx = m.u8(tn.extra + key);
        tn = tone(m, tn.wav + 12 * idx);
      } else if (tn.type & 0x80) {                   // rhythm: one voice per key
        tn = tone(m, tn.wav + 12 * key);
        if (tn.panSweep & 0x80) pan = (tn.panSweep - 0xC0) * 2;
        k2 = tn.key;
      }
      if (tn.type & 0xC0) return;
      const kind = tn.type & 7, fixed = !!(tn.type & 8);
      const c = { track: t, key, midi: k2, vel, gate, tone: tn, kind, fixed, pan, env: 0, state: 0, stop: false, pos: 0, phase: 0, age: this.ticks, lfsr: 0x7FFF };
      if (kind === 0) {
        c.wave = wave(m, tn.wav); if (!c.wave) return;
        const ds = this.chans.filter(x => x.kind === 0);
        if (ds.length >= MAX_DS) {                   // steal: released first, then the oldest
          const v = ds.sort((a, b) => (b.stop - a.stop) || a.age - b.age)[0];
          this.chans.splice(this.chans.indexOf(v), 1);
        }
      } else {
        // one CGB channel of each kind
        const old = this.chans.findIndex(x => x.kind === kind);
        if (old >= 0) this.chans.splice(old, 1);
        if (kind === 3) c.wavetable = cgbWave(m, tn.wav);
        if (kind === 1 || kind === 2) c.duty = DUTY[tn.wav & 3];
        if (kind === 4) c.short = !!(tn.wav & 1);
        c.env = 0; c.cnt = tn.attack;
      }
      this.chans.push(c);
      t.last = key;
      if (this.onNote) { const n = this.onNote(t.i, key, vel); if (n) n.ch = c; }
    }

    // ---- per frame (vblank): tempo, ticks, envelopes ----
    frame() {
      this.tempoC += this.tempoI;
      while (this.tempoC >= 150) { this.tick(); this.tempoC -= 150; }
      this.frames++;
      const out = [];
      for (const c of this.chans) if (this.envelope(c)) out.push(c);
      this.chans = out;
      if (this.fade) { this.fade++; if (this.fade > FRAME_HZ * 4) this.done = true; }
      if (this.tracks.every(t => !t.on) && !this.chans.length) this.done = true;
    }
    envelope(c) {
      const tn = c.tone;
      if (c.kind === 0) {
        if (c.stop && c.state < 3) c.state = 3;
        if (c.state === 0) { c.env += tn.attack; if (c.env >= 255) { c.env = 255; c.state = 1; } }
        else if (c.state === 1) { c.env = (c.env * tn.decay) >> 8; if (c.env <= tn.sustain) { c.env = tn.sustain; c.state = 2; if (!c.env) return false; } }
        else if (c.state === 3) { c.env = (c.env * tn.release) >> 8; if (c.env <= 0) return false; }
        return true;
      }
      // CGB: 0-15 steps, one step every n frames (attack/decay/release 0-7, sustain 0-15)
      const sus = tn.sustain & 15;
      if (c.stop && c.state < 3) { c.state = 3; c.cnt = tn.release & 7; if (!c.cnt) return false; }
      if (c.state === 0) { if (!(tn.attack & 7)) { c.env = 15; c.state = 1; c.cnt = tn.decay & 7; } else if (--c.cnt <= 0) { c.cnt = tn.attack & 7; if (++c.env >= 15) { c.env = 15; c.state = 1; c.cnt = tn.decay & 7; } } }
      else if (c.state === 1) { if (!(tn.decay & 7)) { c.env = sus; c.state = 2; } else if (--c.cnt <= 0) { c.cnt = tn.decay & 7; if (--c.env <= sus) { c.env = sus; c.state = 2; } } if (!c.env && c.state === 2) return false; }
      else if (c.state === 3) { if (--c.cnt <= 0) { c.cnt = tn.release & 7; if (--c.env <= 0) return false; } }
      return true;
    }
    // per-channel volume (left, right 0..1) and frequency, from its track (m4a TrkVolPitSet, ChnVolSetAsm)
    params(c) {
      const t = c.track;
      let x = t.vol * 2;
      if (t.modT === 1) x = (x * (t.modM + 128)) >> 7;
      let y = 2 * t.pan; if (t.modT === 2) y += t.modM;
      y = Math.max(-128, Math.min(127, y));
      const mr = Math.min(255, ((y + 128) * x) >> 8), ml = Math.min(255, ((127 - y) * x) >> 8);
      const r = Math.min(255, (mr * c.vel * (128 + c.pan)) >> 14), l = Math.min(255, (ml * c.vel * (127 - c.pan)) >> 14);
      let pit = (t.tune + t.bend * t.bendRange) * 4 + (t.keyShift << 8);
      if (t.modT === 0) pit += 16 * t.modM;
      const key = c.midi + pit / 256;
      let freq;
      if (c.kind === 0) freq = c.fixed ? MIXER_HZ : c.wave.rate * Math.pow(2, (key - 60) / 12);
      else if (c.kind === 4) freq = Math.min(262144, 4096 * Math.pow(2, (key - 60) / 12) * 4);
      else freq = 440 * Math.pow(2, (key - 69) / 12);
      return [l / 255, r / 255, freq];
    }

    // ---- audio ----
    // fills two Float32Arrays; advances frames as time passes
    render(L, R, n = L.length) {
      L.fill(0, 0, n); R.fill(0, 0, n);
      const spf = this.rate / FRAME_HZ;
      let i = 0;
      while (i < n) {
        if (this.frac <= 0) { if (!this.done) this.frame(); this.frac += spf; }
        const len = Math.min(n - i, Math.ceil(this.frac));
        this.mix(L, R, i, len);
        i += len; this.frac -= len;
      }
      const g = this.volume * (this.fade ? Math.max(0, 1 - this.fade / (FRAME_HZ * 4)) : 1);
      for (let j = 0; j < n; j++) { L[j] = Math.max(-1, Math.min(1, L[j] * g)); R[j] = Math.max(-1, Math.min(1, R[j] * g)); }
    }
    mix(L, R, off, len) {
      const solo = this.anySolo;
      for (const c of this.chans) {
        const t = c.track;
        if (t.mute || (solo && !t.solo)) { this.advance(c, len); continue; }
        const [l, r, freq] = this.params(c);
        if (c.kind === 0) {
          const amp = (c.env / 255) * 0.6, gl = l * amp, gr = r * amp, w = c.wave, d = w.data, step = freq / this.rate;
          let p = c.pos;
          for (let j = off, e = off + len; j < e; j++) {
            if (p >= w.size) { if (w.loop && w.size > w.loopStart) p = w.loopStart + (p - w.size) % (w.size - w.loopStart); else { c.env = 0; c.state = 4; c.stop = true; break; } }
            const k = p | 0, f = p - k, s = d[k] + (d[k + 1] - d[k]) * f;
            L[j] += s * gl; R[j] += s * gr; p += step;
          }
          c.pos = p;
          if (c.state === 4) c.dead = true;
        } else {
          const amp = (c.env / 15) * 0.3, gl = l * amp, gr = r * amp, step = freq / this.rate;
          let ph = c.phase;
          for (let j = off, e = off + len; j < e; j++) {
            let s;
            if (c.kind === 3) s = c.wavetable[(ph * 32 | 0) & 31];
            else if (c.kind === 4) {
              ph += step; while (ph >= 1) { ph -= 1; const b = (c.lfsr ^ (c.lfsr >> 1)) & 1; c.lfsr = (c.lfsr >> 1) | (b << 14); if (c.short) c.lfsr = (c.lfsr & ~0x40) | (b << 6); }
              s = c.lfsr & 1 ? -1 : 1; L[j] += s * gl; R[j] += s * gr; continue;
            } else s = (ph % 1) < c.duty ? 1 : -1;
            L[j] += s * gl; R[j] += s * gr; ph = (ph + step) % 1;
          }
          c.phase = ph;
        }
      }
      if (this.chans.some(c => c.dead)) this.chans = this.chans.filter(c => !c.dead);
    }
    advance(c, len) {
      if (c.kind !== 0) return;
      const [, , freq] = this.params(c), w = c.wave;
      c.pos += freq / this.rate * len;
      if (c.pos >= w.size) { if (w.loop && w.size > w.loopStart) c.pos = w.loopStart + (c.pos - w.size) % (w.size - w.loopStart); else c.dead = true; }
    }

    // jump to frame f: replay the sequencer silently from the start
    seek(f) {
      const mute = this.tracks.map(t => [t.mute, t.solo]);
      this.reset();
      this.tracks.forEach((t, i) => { [t.mute, t.solo] = mute[i]; });
      while (this.frames < f && !this.done) this.frame();
      this.chans = [];
      this.frac = 0;
    }
    // sound one voice (a 12-byte voicegroup entry at addr) at key for ticks
    static preview(data, addr, key = 60, ticks = 40, rate = 48000) {
      const p = new Player({ song: data.song || data.chunks[0][0], chunks: data.chunks }, rate);
      p.ntracks = 0; p.tracks = [];
      const t = p.newTrack(0, 0); t.vol = 127;
      p.play(t, tone(p.mem, addr), key, 127, ticks);
      p.tracks = [t]; t.on = false;
      return p;
    }

    // ---- analysis: run the sequencer alone, recording notes ----
    // {frames: to the end or the first loop, notes [{t, d, key, vel, track}] in frames, loopFrame}
    survey(maxFrames = FRAME_HZ * 60 * 12) {
      this.reset();
      const notes = [];
      let open = [], loopFrame = null;
      this.onNote = (ti, key, vel) => { const n = { t: this.frames, d: 0, key, vel, track: ti }; notes.push(n); open.push(n); return n; };
      while (this.frames < maxFrames && !this.done) {
        const before = this.looped;
        this.frame();
        open = open.filter(n => { if (n.ch && !n.ch.stop && this.chans.includes(n.ch)) return true; n.d = Math.max(1, this.frames - n.t); return false; });
        if (this.looped > before) { loopFrame = this.frames; break; }
      }
      for (const n of open) n.d = Math.max(1, this.frames - n.t);
      const out = { frames: this.frames, notes, loopFrame };
      this.reset();
      return out;
    }
  }

  // ---- WebAudio: one shared context and processor ----
  let ctx = null, node = null, current = null;
  function start(player) {
    ctx ||= new (window.AudioContext || window.webkitAudioContext)();
    if (ctx.state === 'suspended') ctx.resume();
    if (!node) {
      node = ctx.createScriptProcessor(2048, 0, 2);
      node.onaudioprocess = e => {
        const L = e.outputBuffer.getChannelData(0), R = e.outputBuffer.getChannelData(1);
        if (!current) { L.fill(0); R.fill(0); return; }
        current.render(L, R);
        if (current.done && !current.chans.length) { const p = current; current = null; p.onEnd?.(); }
      };
      node.connect(ctx.destination);
    }
    player.rate = ctx.sampleRate;
    current = player;
  }
  function stop() { const p = current; current = null; p?.onEnd?.(); }
  const rate = () => (ctx ||= new (window.AudioContext || window.webkitAudioContext)()).sampleRate;
  const playing = () => current;

  return { Player, Mem, tone, wave, start, stop, playing, rate, FRAME_HZ, MIXER_HZ };
})();
