// Player-facing features of the port page: subtitles with style settings,
// rebindable keyboard and gamepad controls, display filters, a sun sensor
// (slider, your clock, or your camera), screenshots and clip recording.
// Settings live in localStorage. index.html calls UI.init(...) once.
'use strict';

const UI = (() => {
  const STORE = 'boktai3.port.settings';
  const BUTTONS = ['a', 'b', 'select', 'start', 'right', 'left', 'up', 'down', 'r', 'l'];
  const ACTIONS = [
    ['up', 'Up'], ['down', 'Down'], ['left', 'Left'], ['right', 'Right'],
    ['a', 'A'], ['b', 'B'], ['l', 'L'], ['r', 'R'], ['start', 'Start'], ['select', 'Select'],
    ['fast', 'Fast-forward (hold)'], ['pause', 'Pause'], ['shot', 'Screenshot'], ['record', 'Record a clip'],
    ['fullscreen', 'Fullscreen'], ['subs', 'Subtitles on/off'],
  ];
  // Gamepad buttons in the browser's standard layout (0 = bottom face button).
  // Standard-mapping names, short enough for a bind slot (the title gives the long name).
  const PAD_NAMES = ['Ⓐ', 'Ⓑ', 'Ⓧ', 'Ⓨ', 'LB', 'RB', 'LT', 'RT', 'Back', 'Start', 'L3', 'R3', 'D↑', 'D↓', 'D←', 'D→', 'Home'];
  const PAD_TITLES = ['A / Cross', 'B / Circle', 'X / Square', 'Y / Triangle', 'Left bumper', 'Right bumper', 'Left trigger',
    'Right trigger', 'Back / Select', 'Start', 'Left stick press', 'Right stick press', 'D-pad up', 'D-pad down',
    'D-pad left', 'D-pad right', 'Home'];
  const DEFAULTS = {
    subs: { mode: 'auto', style: 'box', size: 100, opacity: 62, color: '#ffffff', speaker: 'color', font: 'sans',
      reflow: true, jp: false, quotes: false },
    keys: {
      up: ['ArrowUp'], down: ['ArrowDown'], left: ['ArrowLeft'], right: ['ArrowRight'],
      a: ['KeyX'], b: ['KeyZ'], l: ['KeyA'], r: ['KeyS'], start: ['Enter'], select: ['Backspace'],
      fast: ['Space'], pause: ['KeyP'], shot: ['F9'], record: ['F10'], fullscreen: ['KeyF'], subs: ['KeyT'],
    },
    pad: {
      up: [{ b: 12 }, { a: 1, d: -1 }], down: [{ b: 13 }, { a: 1, d: 1 }], left: [{ b: 14 }, { a: 0, d: -1 }],
      right: [{ b: 15 }, { a: 0, d: 1 }], a: [{ b: 0 }], b: [{ b: 1 }], l: [{ b: 4 }], r: [{ b: 5 }],
      start: [{ b: 9 }], select: [{ b: 8 }], fast: [{ b: 7 }], pause: [], shot: [], record: [], fullscreen: [], subs: [],
    },
    display: { color: 'raw', lcd: 0, smooth: false },
    sun: { mode: 'slider', camera: 1 },
  };
  let S = load();
  let env = null;
  const $ = (id) => document.getElementById(id);
  const el = (tag, props = {}, ...kids) => {
    const e = Object.assign(document.createElement(tag), props);
    for (const k of kids) if (k != null) e.append(k);
    return e;
  };

  function load() {
    let saved = {};
    try { saved = JSON.parse(localStorage.getItem(STORE) || '{}'); } catch (e) { saved = {}; }
    const out = structuredClone(DEFAULTS);
    for (const k of Object.keys(out)) if (saved[k]) Object.assign(out[k], saved[k]);
    return out;
  }
  function save() { try { localStorage.setItem(STORE, JSON.stringify(S)); } catch (e) { /* private window */ } }

  // ---- subtitles ----------------------------------------------------------
  // A line of dialogue from the fan translation: "Speaker:\n"Text..."
  // becomes a speaker label and flowing text over the picture.
  const subs = { id: null, bounds: [1, 13, 28, 6], strings: { en: {}, jp: {} }, preview: false };
  const SPEAKER_HUES = [205, 35, 145, 330, 265, 95, 10, 180];
  const KNOWN = { Django: '#ff9f6b', Trinity: '#ffd166', Otenko: '#ffe680', Sabata: '#d9a0ff', Zazie: '#8fd3ff' };
  function speakerColor(name) {
    if (KNOWN[name]) return KNOWN[name];
    let h = 0;
    for (const c of name) h = (h * 31 + c.charCodeAt(0)) >>> 0;
    return `hsl(${SPEAKER_HUES[h % SPEAKER_HUES.length]} 85% 72%)`;
  }
  function parse(text) {
    let speaker = null, body = text;
    const m = text.match(/^([^\n"「:]{1,24}):\s*\n([\s\S]*)$/);
    if (m) { speaker = m[1].trim(); body = m[2]; }
    if (!S.subs.quotes) body = body.replace(/^\s*["“]/, '').replace(/["”]\s*$/, '');
    body = S.subs.reflow ? body.replace(/\s*\n\s*/g, ' ').trim() : body.trim();
    return { speaker, body };
  }
  const FONTS = {
    sans: '"Inter", "Segoe UI", system-ui, sans-serif',
    rounded: 'ui-rounded, "SF Pro Rounded", "Nunito", "Varela Round", system-ui, sans-serif',
    serif: '"Iowan Old Style", "Palatino Linotype", Georgia, serif',
    mono: 'ui-monospace, "SF Mono", Menlo, monospace',
  };
  function styleSubs(box) {
    const s = S.subs;
    box.className = 'subs subs-' + s.style;
    box.style.setProperty('--sub-size', (3.9 * s.size / 100) + 'cqw');
    box.style.setProperty('--sub-bg', `rgba(8, 10, 18, ${s.opacity / 100})`);
    box.style.setProperty('--sub-color', s.color);
    box.style.setProperty('--sub-font', FONTS[s.font] || FONTS.sans);
  }
  function renderSubs() {
    const box = $('subtitle'), cap = $('caption');
    const s = S.subs;
    const text = subs.preview ? 'Trinity:\n"Don\'t tell me you...\nin my stead..."' : subs.id == null ? null : subs.strings.en[subs.id];
    const jp = subs.preview ? 'トリニティ\n「まさか　オイラの代わりに・・・' : subs.id == null ? null : subs.strings.jp[subs.id];
    if (!text || s.mode === 'off') {
      box.classList.remove('shown');
      cap.replaceChildren();
      return;
    }
    const { speaker, body } = parse(text);
    const target = s.mode === 'below' ? cap : box;
    (target === box ? cap : box).replaceChildren();
    styleSubs(target);
    box.classList.toggle('shown', target === box);
    const kids = [];
    if (speaker && s.speaker !== 'hide') {
      if (s.speaker === 'color') kids.push(el('div', { className: 'sub-speaker', textContent: speaker, style: `color:${speakerColor(speaker)}` }));
    }
    const lineText = speaker && s.speaker === 'prefix' ? `${speaker}: ${body}` : body;
    kids.push(el('div', { className: 'sub-line' }, el('span', { textContent: lineText })));
    if (s.jp && jp) {
      // "トリニティ\n「まさか…" -> "まさか…": the name is already above
      const lines = jp.split('\n');
      if (lines.length > 1 && lines[0].length <= 12 && /^[「『(（]/.test(lines[1])) lines.shift();
      const j = lines.join(' ').replace(/^[「『]/, '').replace(/[」』]$/, '').replace(/\s+/g, ' ');
      kids.push(el('div', { className: 'sub-jp' }, el('span', { textContent: j })));
    }
    target.replaceChildren(...(s.style === 'glass' ? [el('div', { className: 'glass-panel' }, ...kids)] : kids));
    // where: just above the game's own text box (it stays visible), over it, or at an edge
    if (target === box) {
      const [, y, , h] = subs.preview ? [1, 13, 28, 6] : subs.bounds;
      const top = y * 8 / 160 * 100, bottom = (y + h) * 8 / 160 * 100;
      box.style.top = box.style.bottom = box.style.transform = '';
      let mode = s.mode;
      if (mode === 'auto') mode = top > 50 ? 'above' : 'under';
      if (mode === 'above') box.style.bottom = (100 - top + 1.5) + '%';
      else if (mode === 'under') box.style.top = (bottom + 1.5) + '%';
      else if (mode === 'over') { box.style.top = ((top + bottom) / 2) + '%'; box.style.transform = 'translateY(-50%)'; }
      else if (mode === 'top') box.style.top = '4%';
      else box.style.bottom = '4%';
    }
  }
  function showText(id) { subs.id = id; renderSubs(); }
  function setBounds(b) { if (b && b[2] > 0) subs.bounds = b; }
  async function loadStrings() {
    if (Object.keys(subs.strings.en).length) return;
    try { const r = await fetch('strings.json'); if (r.ok) subs.strings = await r.json(); } catch (e) { /* none */ }
  }

  // ---- input --------------------------------------------------------------
  let kbHeld = new Set(), padMask = 0, padFast = false, padPrev = new Set(), listening = null;
  function codeLabel(code) {
    const arrows = { ArrowUp: '↑', ArrowDown: '↓', ArrowLeft: '←', ArrowRight: '→' };
    if (arrows[code]) return arrows[code];
    if (/^Key[A-Z]$/.test(code)) return code.slice(3);
    if (/^Digit\d$/.test(code)) return code.slice(5);
    const short = { Backspace: '⌫', Enter: '⏎', Space: 'Space', Escape: 'Esc', Tab: '⇥', ShiftLeft: '⇧L', ShiftRight: '⇧R',
      ControlLeft: 'Ctrl L', ControlRight: 'Ctrl R', AltLeft: 'Alt L', AltRight: 'Alt R', MetaLeft: '⌘L', MetaRight: '⌘R',
      Semicolon: ';', Quote: "'", Comma: ',', Period: '.', Slash: '/', Backslash: '\\', BracketLeft: '[', BracketRight: ']',
      Minus: '-', Equal: '=', Backquote: '`', PageUp: 'PgUp', PageDown: 'PgDn' };
    if (short[code]) return short[code];
    return code.replace(/^Numpad/, 'Num');
  }
  function padLabel(p) {
    if (p.b !== undefined) return PAD_NAMES[p.b] || 'Button ' + p.b;
    return `${p.a >> 1 ? 'RS' : 'LS'}${['←', '→', '↑', '↓'][(p.a & 1) * 2 + (p.d > 0 ? 1 : 0)]}`;
  }
  function padTitle(p) {
    if (p.b !== undefined) return PAD_TITLES[p.b] || 'Button ' + p.b;
    return `${p.a >> 1 ? 'Right' : 'Left'} stick ${['left', 'right', 'up', 'down'][(p.a & 1) * 2 + (p.d > 0 ? 1 : 0)]}`;
  }
  function actionFor(code) {
    for (const [act, codes] of Object.entries(S.keys)) if (codes.includes(code)) return act;
    return null;
  }
  function kbMask() {
    let m = 0;
    for (const act of kbHeld) { const i = BUTTONS.indexOf(act); if (i >= 0) m |= 1 << i; }
    return m;
  }
  function writeKeys() {
    Atomics.store(env.control, 0, kbMask() | padMask);
    Atomics.store(env.control, 1, kbHeld.has('fast') || padFast ? 1 : 0);
  }
  function trigger(act) {
    if (act === 'pause') env.togglePause();
    else if (act === 'shot') screenshot();
    else if (act === 'record') toggleRecord();
    else if (act === 'fullscreen') toggleFullscreen();
    else if (act === 'subs') { S.subs.mode = S.subs.mode === 'off' ? 'auto' : 'off'; save(); env.subtitlesChanged(); renderSubs(); refreshDialog(); }
  }
  function typing(e) { return e.target.closest && e.target.closest('input, textarea, select, [contenteditable]'); }
  function onKeyDown(e) {
    if (listening && listening.kind === 'key') {
      e.preventDefault();
      if (e.code !== 'Escape') bindKey(listening.action, listening.slot, e.code);
      listening = null; refreshDialog();
      return;
    }
    if (e.key === 'Escape' && dialog && dialog.open) { dialog.close(); return; }
    if (typing(e) || e.metaKey || e.ctrlKey) return;
    const act = actionFor(e.code);
    if (!act) return;
    e.preventDefault();
    if (e.repeat) return;
    const i = BUTTONS.indexOf(act);
    if (i >= 0 || act === 'fast') {
      kbHeld.add(act);
      if (i >= 0) Atomics.or(env.control, 5, 1 << i); // latched: a tap shorter than a frame still counts
      writeKeys();
    } else trigger(act);
  }
  function onKeyUp(e) {
    const act = actionFor(e.code);
    if (act && kbHeld.delete(act)) writeKeys();
  }
  function bindKey(action, slot, code) {
    for (const codes of Object.values(S.keys)) { const j = codes.indexOf(code); if (j >= 0) codes.splice(j, 1); }
    const list = S.keys[action] || (S.keys[action] = []);
    list[slot] = code;
    S.keys[action] = list.filter(Boolean);
    save(); legend();
  }
  function bindPad(action, slot, p) {
    const same = (q) => q && q.b === p.b && q.a === p.a && q.d === p.d;
    for (const list of Object.values(S.pad)) { const j = list.findIndex(same); if (j >= 0) list.splice(j, 1); }
    const list = S.pad[action] || (S.pad[action] = []);
    list[slot] = p;
    S.pad[action] = list.filter(Boolean);
    save();
  }
  function pollPads() {
    const pads = navigator.getGamepads ? [...navigator.getGamepads()].filter(Boolean) : [];
    const active = new Set();
    for (const pad of pads) {
      pad.buttons.forEach((b, i) => { if (b.pressed || b.value > 0.5) active.add('b' + i); });
      pad.axes.forEach((v, i) => { if (v > 0.55) active.add(`a${i}+`); if (v < -0.55) active.add(`a${i}-`); });
    }
    if (listening && listening.kind === 'pad') {
      const fresh = [...active].find((k) => !padPrev.has(k));
      if (fresh) {
        const p = fresh[0] === 'b' ? { b: Number(fresh.slice(1)) } : { a: Number(fresh.slice(1, -1)), d: fresh.endsWith('+') ? 1 : -1 };
        bindPad(listening.action, listening.slot, p);
        listening = null; refreshDialog();
      }
      padPrev = active;
      return;
    }
    let mask = 0, fast = false;
    const key = (p) => (p.b !== undefined ? 'b' + p.b : `a${p.a}${p.d > 0 ? '+' : '-'}`);
    for (const [act, list] of Object.entries(S.pad)) {
      const on = list.some((p) => active.has(key(p)));
      const was = list.some((p) => padPrev.has(key(p)));
      const i = BUTTONS.indexOf(act);
      if (i >= 0 && on) { mask |= 1 << i; if (!was) Atomics.or(env.control, 5, 1 << i); }
      else if (act === 'fast') fast = fast || on;
      else if (i < 0 && on && !was) trigger(act);
    }
    if (mask !== padMask || fast !== padFast) { padMask = mask; padFast = fast; writeKeys(); }
    padPrev = active;
    $('pad-status') && ($('pad-status').textContent = pads.length ? `Gamepad: ${pads[0].id.replace(/\s*\(.*\)/, '')}` : '');
  }
  function legend() {
    const keys = $('keys-legend');
    if (!keys) return;
    const show = (act) => (S.keys[act] || []).map(codeLabel).join(' / ') || '—';
    const items = [['D-pad', ['up', 'down', 'left', 'right'].map(show).join(' ')], ['A', show('a')], ['B', show('b')],
      ['L', show('l')], ['R', show('r')], ['Start', show('start')], ['Select', show('select')],
      ['fast-forward (hold)', show('fast')], ['pause', show('pause')], ['screenshot', show('shot')], ['record', show('record')]];
    keys.replaceChildren(...items.map(([what, k]) => el('span', {}, el('kbd', { textContent: k }), ' ' + what)));
  }

  // ---- display ------------------------------------------------------------
  function applyDisplay() {
    const d = S.display;
    const c = env.canvas;
    c.style.imageRendering = d.smooth ? 'auto' : 'pixelated';
    c.style.filter = d.color === 'gba' ? 'url(#gba-lcd-color)' : d.color === 'warm' ? 'saturate(.88) sepia(.12) brightness(1.03)' : '';
    const grid = $('lcd-grid');
    grid.style.opacity = d.lcd / 100;
    grid.hidden = !d.lcd;
  }
  function toggleFullscreen() {
    const s = document.querySelector('.screen');
    if (document.fullscreenElement) document.exitFullscreen();
    else s.requestFullscreen && s.requestFullscreen().catch(() => {});
  }

  // ---- sun ----------------------------------------------------------------
  // The cartridge's solar sensor: the slider, the sun's height at your local
  // time, or your camera (point it at the sky; bright light = strong sun).
  let sunTimer = null, cam = null;
  function setSunValue(v) {
    const slider = $('sun');
    slider.value = Math.round(Math.max(0, Math.min(255, v)));
    slider.dispatchEvent(new Event('input'));
  }
  function clockSun() {
    const now = new Date(), h = now.getHours() + now.getMinutes() / 60;
    const elevation = Math.sin(Math.PI * (h - 6) / 12); // 0 at 6:00 and 18:00, 1 at noon
    return elevation > 0 ? 40 + 215 * elevation : 0;
  }
  async function startCamera() {
    try {
      const stream = await navigator.mediaDevices.getUserMedia({ video: { facingMode: 'environment', width: 160, height: 120 } });
      const track = stream.getVideoTracks()[0];
      // Fixed exposure makes brightness absolute; auto exposure would undo the sun.
      try {
        const caps = track.getCapabilities ? track.getCapabilities() : {};
        if (caps.exposureMode && caps.exposureMode.includes('manual') && caps.exposureTime) {
          await track.applyConstraints({ advanced: [{ exposureMode: 'manual', exposureTime: caps.exposureTime.min + (caps.exposureTime.max - caps.exposureTime.min) * 0.05 }] });
        }
      } catch (e) { /* not adjustable: highlights still tell */ }
      const video = el('video', { muted: true, playsInline: true, srcObject: stream });
      await video.play();
      const c = el('canvas', { width: 32, height: 24 }), x = c.getContext('2d', { willReadFrequently: true });
      cam = { stream, video, c, x, level: 0 };
      return true;
    } catch (e) {
      $('sun-note') && ($('sun-note').textContent = 'Camera unavailable: ' + e.message);
      return false;
    }
  }
  function cameraSun() {
    if (!cam) return null;
    cam.x.drawImage(cam.video, 0, 0, 32, 24);
    const px = cam.x.getImageData(0, 0, 32, 24).data;
    let sum = 0, bright = 0;
    for (let i = 0; i < px.length; i += 4) {
      const l = (0.2126 * px[i] + 0.7152 * px[i + 1] + 0.0722 * px[i + 2]) / 255;
      sum += l; if (l > 0.94) bright++;
    }
    const mean = sum / (px.length / 4), clipped = bright / (px.length / 4);
    const raw = Math.max(0, Math.min(1, (mean - 0.35) * 1.6 + clipped * 1.2)) * S.sun.camera;
    cam.level = cam.level * 0.7 + raw * 0.3;
    return cam.level * 255;
  }
  function applySun() {
    clearInterval(sunTimer);
    sunTimer = null;
    $('sun').disabled = S.sun.mode !== 'slider';
    if (S.sun.mode !== 'camera' && cam) { cam.stream.getTracks().forEach((t) => t.stop()); cam = null; }
    if (S.sun.mode === 'clock') { setSunValue(clockSun()); sunTimer = setInterval(() => setSunValue(clockSun()), 30000); }
    if (S.sun.mode === 'camera') {
      (cam ? Promise.resolve(true) : startCamera()).then((ok) => {
        if (!ok) { S.sun.mode = 'slider'; save(); applySun(); refreshDialog(); return; }
        sunTimer = setInterval(() => { const v = cameraSun(); if (v !== null) setSunValue(v); }, 250);
      });
    }
  }

  // ---- capture ------------------------------------------------------------
  function stamp() {
    const d = new Date(), p = (n) => String(n).padStart(2, '0');
    return `${d.getFullYear()}-${p(d.getMonth() + 1)}-${p(d.getDate())}-${p(d.getHours())}${p(d.getMinutes())}${p(d.getSeconds())}`;
  }
  function download(blob, name) {
    const a = el('a', { href: URL.createObjectURL(blob), download: name });
    document.body.append(a); a.click(); a.remove();
    setTimeout(() => URL.revokeObjectURL(a.href), 10000);
  }
  function scaled(scale = 4) {
    const c = el('canvas', { width: 240 * scale, height: 160 * scale });
    const x = c.getContext('2d');
    x.imageSmoothingEnabled = false;
    if (S.display.color === 'gba') x.filter = 'url(#gba-lcd-color)';
    x.drawImage(env.canvas, 0, 0, c.width, c.height);
    return c;
  }
  function screenshot() {
    scaled(4).toBlob((b) => b && download(b, `boktai3-${stamp()}.png`), 'image/png');
    flash('Screenshot saved');
  }
  let rec = null;
  function toggleRecord() {
    if (rec) { rec.recorder.stop(); return; }
    const c = el('canvas', { width: 720, height: 480 });
    const x = c.getContext('2d');
    x.imageSmoothingEnabled = false;
    const stream = c.captureStream(60);
    const audio = env.audio && env.audio();
    if (audio && audio.context && audio.node) {
      const dest = audio.context.createMediaStreamDestination();
      audio.node.connect(dest);
      dest.stream.getAudioTracks().forEach((t) => stream.addTrack(t));
      rec = { dest, audio };
    } else rec = {};
    const type = ['video/webm;codecs=vp9,opus', 'video/webm;codecs=vp8,opus', 'video/webm', 'video/mp4']
      .find((t) => window.MediaRecorder && MediaRecorder.isTypeSupported(t));
    const chunks = [];
    rec.recorder = new MediaRecorder(stream, { mimeType: type, videoBitsPerSecond: 6e6 });
    rec.recorder.ondataavailable = (e) => e.data.size && chunks.push(e.data);
    rec.recorder.onstop = () => {
      if (rec.audio) try { rec.audio.node.disconnect(rec.dest); } catch (e) { /* gone */ }
      cancelAnimationFrame(rec.raf);
      download(new Blob(chunks, { type }), `boktai3-${stamp()}.${type.includes('mp4') ? 'mp4' : 'webm'}`);
      rec = null;
      $('record').textContent = 'Record';
      $('record').classList.remove('recording');
      flash('Clip saved');
    };
    const draw = () => { x.drawImage(env.canvas, 0, 0, 720, 480); rec && (rec.raf = requestAnimationFrame(draw)); };
    draw();
    rec.recorder.start(1000);
    $('record').textContent = '■ Stop';
    $('record').classList.add('recording');
  }
  function flash(text) {
    const f = $('toast');
    f.textContent = text;
    f.classList.add('on');
    clearTimeout(flash.t);
    flash.t = setTimeout(() => f.classList.remove('on'), 1400);
  }

  // ---- settings dialog ----------------------------------------------------
  let dialog = null, dialogTab = 'subtitles';
  function control(label, input, hint) {
    return el('label', { className: 'set-row' }, el('span', { className: 'set-label', textContent: label }), input,
      hint ? el('span', { className: 'set-hint', textContent: hint }) : null);
  }
  function select(obj, key, options, onChange) {
    const s = el('select');
    for (const [v, t] of options) s.add(new Option(t, v));
    s.value = obj[key];
    s.onchange = () => { obj[key] = s.value; save(); onChange && onChange(); };
    return s;
  }
  function range(obj, key, min, max, step, onChange, fmt) {
    const out = el('span', { className: 'set-value' });
    const r = el('input', { type: 'range', min, max, step, value: obj[key] });
    const show = () => { out.textContent = fmt ? fmt(Number(r.value)) : r.value; };
    r.oninput = () => { obj[key] = Number(r.value); show(); save(); onChange && onChange(); };
    show();
    return el('span', { className: 'set-range' }, r, out);
  }
  function check(obj, key, onChange) {
    const c = el('input', { type: 'checkbox', checked: !!obj[key] });
    c.onchange = () => { obj[key] = c.checked; save(); onChange && onChange(); };
    return c;
  }
  function subsPane() {
    const s = S.subs, re = () => { renderSubs(); env.subtitlesChanged(); };
    const swatches = el('span', { className: 'swatches' }, ...['#ffffff', '#ffe066', '#9ef0ff', '#b8f5a0'].map((c) => {
      const b = el('button', { className: 'swatch' + (s.color === c ? ' on' : ''), title: c, style: `background:${c}` });
      b.onclick = () => { s.color = c; save(); renderSubs(); refreshDialog(); };
      return b;
    }));
    return [
      control('Show', select(s, 'mode', [['auto', 'Next to the game\'s text box'], ['over', 'Over the text box'],
        ['bottom', 'Bottom of the screen'], ['top', 'Top of the screen'], ['below', 'Below the game'], ['off', 'Off']], re)),
      control('Style', select(s, 'style', [['box', 'Soft box behind each line'], ['glass', 'Frosted glass panel'],
        ['outline', 'Outlined text, no box'], ['shadow', 'Shadow only']], re)),
      control('Size', range(s, 'size', 60, 180, 5, re, (v) => v + '%')),
      control('Background', range(s, 'opacity', 0, 100, 2, re, (v) => v + '%'), 'box and glass styles'),
      control('Text color', swatches),
      control('Font', select(s, 'font', [['sans', 'Sans'], ['rounded', 'Rounded'], ['serif', 'Serif'], ['mono', 'Monospace']], re)),
      control('Speaker', select(s, 'speaker', [['color', 'Colored name above the line'], ['prefix', 'Name: before the line'], ['hide', 'Hidden']], re)),
      control('Join lines', check(s, 'reflow', re), 'flow the translation as one sentence instead of the box\'s line breaks'),
      control('Japanese too', check(s, 'jp', re), 'the original line, smaller, under the English'),
      control('Keep quotes', check(s, 'quotes', re)),
      el('p', { className: 'set-hint', textContent: 'A sample line stays on the game screen while this tab is open.' }),
    ];
  }
  function controlsPane() {
    const rows = [];
    const slot = (action, i, kind) => {
      const list = kind === 'key' ? S.keys[action] || [] : S.pad[action] || [];
      const v = list[i];
      const on = listening && listening.action === action && listening.slot === i && listening.kind === kind;
      const b = el('button', { className: 'bind' + (on ? ' listening' : '') + (v ? '' : ' empty'),
        textContent: on ? (kind === 'key' ? 'Key…' : 'Button…') : v ? (kind === 'key' ? codeLabel(v) : padLabel(v)) : '+',
        title: v ? (kind === 'key' ? v : padTitle(v)) : 'Click, then press a ' + (kind === 'key' ? 'key' : 'gamepad button') });
      b.onclick = () => { listening = { action, slot: i, kind }; refreshDialog(); };
      if (v) {
        const x = el('button', { className: 'unbind', title: 'Remove', textContent: '×' });
        x.onclick = () => { list.splice(i, 1); save(); legend(); refreshDialog(); };
        return el('span', { className: 'bind-wrap' }, b, x);
      }
      return b;
    };
    for (const [action, label] of ACTIONS) {
      rows.push(el('div', { className: 'bind-row' }, el('span', { className: 'set-label', textContent: label }),
        slot(action, 0, 'key'), slot(action, 1, 'key'), slot(action, 0, 'pad'), slot(action, 1, 'pad')));
    }
    const reset = el('button', { textContent: 'Reset to defaults' });
    reset.onclick = () => { S.keys = structuredClone(DEFAULTS.keys); S.pad = structuredClone(DEFAULTS.pad); save(); legend(); refreshDialog(); };
    return [el('div', { className: 'bind-head' }, el('span'), el('span', { textContent: 'Keyboard' }), el('span'),
      el('span', { textContent: 'Gamepad' }), el('span')), ...rows,
    el('p', { className: 'set-hint', textContent: 'Click a slot, then press the key or gamepad button (Esc cancels). A key bound here leaves its old action.' }),
    el('div', { className: 'set-foot' }, reset, el('span', { id: 'pad-status', className: 'set-hint' }))];
  }
  function displayPane() {
    const d = S.display;
    return [
      control('Colors', select(d, 'color', [['raw', 'As the game wrote them'], ['gba', 'GBA screen (softer, like the handheld)'], ['warm', 'Warm']], applyDisplay)),
      control('LCD grid', range(d, 'lcd', 0, 100, 5, applyDisplay, (v) => (v ? v + '%' : 'off'))),
      control('Smooth scaling', check(d, 'smooth', applyDisplay), 'off keeps sharp pixels'),
      el('p', { className: 'set-hint', textContent: 'Fullscreen: the F key or the Fullscreen button.' }),
    ];
  }
  function sunPane() {
    const s = S.sun;
    const meter = el('span', { id: 'sun-note', className: 'set-hint' });
    return [
      control('Sunlight from', select(s, 'mode', [['slider', 'The slider under the game'], ['clock', 'Your clock (the sun at this hour)'],
        ['camera', 'Your camera (experimental)']], () => { applySun(); refreshDialog(); })),
      s.mode === 'camera' ? control('Camera gain', range(s, 'camera', 0.5, 2.5, 0.1, null, (v) => v.toFixed(1) + '×')) : null,
      el('p', { className: 'set-hint', textContent: s.mode === 'camera'
        ? 'Point the camera at the sky or a window: bright light charges the Gun Del Sol the way the cartridge\'s sensor did. Nothing leaves this page.'
        : s.mode === 'clock' ? 'Strongest at noon, none at night; updates every 30 seconds.' : 'Move the Sunlight slider yourself.' }),
      meter,
    ];
  }
  function refreshDialog() {
    if (!dialog || !dialog.open) return;
    const panes = { subtitles: subsPane, controls: controlsPane, display: displayPane, sun: sunPane };
    for (const b of dialog.querySelectorAll('.set-tabs button')) b.classList.toggle('active', b.dataset.tab === dialogTab);
    dialog.querySelector('.set-body').replaceChildren(...panes[dialogTab]().filter(Boolean));
    subs.preview = dialogTab === 'subtitles';
    renderSubs();
  }
  function openSettings(tab) {
    if (!dialog) {
      dialog = el('dialog', { id: 'settings' },
        el('div', { className: 'set-top' }, el('strong', { textContent: 'Settings' }),
          el('div', { className: 'set-tabs' }, ...[['subtitles', 'Subtitles'], ['controls', 'Controls'], ['display', 'Display'], ['sun', 'Sun']]
            .map(([t, n]) => { const b = el('button', { textContent: n }); b.dataset.tab = t; b.onclick = () => { dialogTab = t; listening = null; refreshDialog(); }; return b; })),
          el('button', { className: 'set-close', textContent: 'Done', onclick: () => dialog.close() })),
        el('div', { className: 'set-body' }));
      dialog.addEventListener('close', () => { listening = null; subs.preview = false; renderSubs(); document.body.classList.remove('settings-open'); });
      dialog.addEventListener('keydown', (e) => { if (e.key === 'Escape' && !listening) dialog.close(); });
      dialog.addEventListener('keydown', (e) => { if (listening) { onKeyDown(e); e.stopPropagation(); } }, true);
      document.body.append(dialog);
    }
    if (tab) dialogTab = tab;
    if (dialog.open && !tab) { dialog.close(); return; }
    dialog.show(); // not modal: the game stays visible and playable beside it
    document.body.classList.add('settings-open');
    refreshDialog();
  }

  function init(e) {
    env = e;
    addEventListener('keydown', onKeyDown);
    addEventListener('keyup', onKeyUp);
    addEventListener('blur', () => { kbHeld.clear(); writeKeys(); });
    // The game runs in a worker, not on animation frames: poll pads on a short
    // timer for lower latency than rAF and input that keeps working when it stalls.
    setInterval(pollPads, 8);
    $('settings-open').onclick = () => openSettings();
    $('shot').onclick = screenshot;
    $('record').onclick = toggleRecord;
    $('fullscreen').onclick = toggleFullscreen;
    legend();
    applyDisplay();
    applySun();
    loadStrings();
  }

  return { init, showText, setBounds, loadStrings, renderSubs, subtitlesOn: () => S.subs.mode !== 'off', openSettings, settings: () => S };
})();
