// Actor overview: the actor's struct sorted into sections, each field drawn as
// the widget that suits it. Current/maximum pairs become bars, headings dials,
// flag words lit bit grids, booleans switches, enums menus, timers ticking
// tiles with a trace, pointers chips. Numbers scrub: drag sideways to change,
// click to type. A star pins a field to the top (kept per struct type).
// Sorting is by field name and type, so it works for any actor struct; padding
// and unknowns stay in "All fields". Used by actor-panel.js.
(function () {
  'use strict';
  function el(tag, props = {}, ...kids) {
    const e = Object.assign(document.createElement(tag), props);
    for (const k of kids) if (k != null) e.append(k);
    return e;
  }
  const SVG = 'http://www.w3.org/2000/svg';
  function svg(tag, attrs = {}, ...kids) {
    const e = document.createElementNS(SVG, tag);
    for (const [k, v] of Object.entries(attrs)) e.setAttribute(k, v);
    for (const k of kids) e.append(k);
    return e;
  }
  const hex = (n) => '0x' + (n >>> 0).toString(16).padStart(8, '0');

  // "movementSpeedScale" -> "Movement speed scale", "currentStatA" -> "Current stat A"
  function human(name) {
    if (typeof name === 'number') return `[${name}]`;
    const words = String(name).replace(/_/g, ' ')
      .replace(/([a-z0-9])([A-Z])/g, '$1 $2').replace(/([A-Z]+)([A-Z][a-z])/g, '$1 $2').replace(/([a-zA-Z])(\d+)/g, '$1 $2')
      .split(/\s+/).filter(Boolean);
    return words.map((w, i) => (/[A-Z]/.test(w) && w === w.toUpperCase()) ? w : i ? w.toLowerCase() : w[0].toUpperCase() + w.slice(1)).join(' ');
  }
  // PLAYER_STATE_IDLE, PLAYER_STATE_RUN -> "Idle", "Run"
  function enumLabels(values) {
    const names = Object.keys(values);
    let pre = names[0] || '';
    for (const n of names) while (pre && !n.startsWith(pre)) pre = pre.slice(0, -1);
    pre = pre.slice(0, pre.lastIndexOf('_') + 1);
    return names.map((n) => {
      const s = (n.slice(pre.length) || n).replace(/_/g, ' ').toLowerCase();
      return [n, s[0].toUpperCase() + s.slice(1), values[n]];
    }).sort((a, b) => a[2] - b[2]);
  }

  const RANGES = {
    u8: [0, 255], s8: [-128, 127], u16: [0, 65535], s16: [-32768, 32767], u32: [0, 4294967295], s32: [-2147483648, 2147483647],
    bool8: [0, 255], bool16: [0, 65535], bool32: [0, 4294967295], char: [-128, 127], 'unsigned char': [0, 255], 'signed char': [-128, 127],
    short: [-32768, 32767], 'unsigned short': [0, 65535], int: [-2147483648, 2147483647], 'unsigned int': [0, 4294967295],
    long: [-2147483648, 2147483647], 'unsigned long': [0, 4294967295],
    vu8: [0, 255], vs8: [-128, 127], vu16: [0, 65535], vs16: [-32768, 32767], vu32: [0, 4294967295], vs32: [-2147483648, 2147483647],
  };
  function rangeOf(n) {
    if (n.bits) return n.v < 0 ? [-(2 ** (n.bits - 1)), 2 ** (n.bits - 1) - 1] : [0, 2 ** n.bits - 1];
    return RANGES[n.t] || [-2147483648, 2147483647];
  }
  const widthOf = (n) => n.bits || Math.round(Math.log2(rangeOf(n)[1] - rangeOf(n)[0] + 1)) || 32;
  const isFloat = (n) => /float|double/.test(n.t) || (typeof n.v === 'number' && !Number.isInteger(n.v));
  const fmt = (v, float) => float ? v.toFixed(3) : String(v);
  function pulse(e) { e.classList.remove('chg'); void e.offsetWidth; e.classList.add('chg'); }

  // ---- what kind of field is this --------------------------------------------
  const BOOL = /^(is|has|can|should|was|did)[A-Z]|(Blocked|Enabled|Disabled|Active|Pending|Visible|Hidden|Locked|Ready|Done|Finished|Valid|Requested|Changed|Complete|Completed|Registered|Mirrored|Moving|Dead|Alive|Paused|Vulnerable|Lock|Flag|Enter|On|Off)$|^(active|done|dead|locked|moving|vulnerable|enabled|visible|hidden|enter|ready|alive|paused|finished|mirrored|grounded|invincible)$/;
  const HEADING = /(heading|facing|angle|direction|rotation|yaw)\d*$/i;
  const TIMER = /(timer|counter|countdown|duration|cooldown|delay|frames?|ticks?|wait|time|elapsed)\d*$/i;
  const MOTION = /(veloc|speed|accel|position|^pos|Pos|offset|Offset|delta|Delta|sway|move|Move|jump|Jump|height|Height|distance|Distance|gravity|Gravity|knock|push|(?:^|[a-z])[XYZ]$)/;
  const VITAL = /(life|Life|health|Health|^hp|Hp$|energy|Energy|^stat(?!e|us)|Stat(?!e|us)|attack|Attack|defen[cs]e|Defen[cs]e|power|Power|charge|Charge|solar|Solar|^sun|Sun|damage|Damage|armor|Armor|level|Level|^exp|Exp$|strength|Strength)/;
  const STATE = /(mode|Mode|phase|Phase|state|State|kind|Kind|action|Action|behavior|Behavior|status|Status|step|Step|stage|Stage|slot|Slot|index|Index|type|Type|^id$|Id$|request|Request|result|Result|cue|Cue|event|Event|command|Command|target|Target|group|Group)/;
  const LOOK = /(sprite|Sprite|anim|Anim|visual|Visual|palette|Palette|colou?r|Colou?r|alpha|Alpha|blend|Blend|glow|Glow|shake|Shake|flash|Flash|scale|Scale|display|Display|draw|Draw|oam|Oam|tile|Tile|mirror|Mirror|flip|Flip|layer|Layer|priority|Priority|blink|Blink|^bg|Bg|effect|Effect|particle|Particle|shadow|Shadow)/;

  function classify(n) {
    const name = String(n.n);
    if (n.ptr) return ['links', 'chip'];
    if ('enumName' in n) return ['state', 'enum'];
    const [lo, hi] = rangeOf(n);
    if (/(^flags?|Flags?|flags)$/.test(name) || /^flags/i.test(name)) return ['state', 'bits'];
    if (n.bits === 1 || (lo === 0 && hi <= 65535 && BOOL.test(name))) return ['state', 'switch'];
    if (HEADING.test(name) && hi - lo <= 65535) return ['motion', 'dial'];
    if (TIMER.test(name)) return ['timers', 'timer'];
    if (MOTION.test(name)) return ['motion', 'num'];
    if (VITAL.test(name)) return ['vitals', 'num'];
    if (STATE.test(name)) return ['state', 'num'];
    if (LOOK.test(name)) return ['look', 'num'];
    return ['other', 'num'];
  }

  const SECTIONS = [
    ['pinned', 'Pinned', '★'], ['vitals', 'Vitals', '♥'], ['motion', 'Motion', '➹'], ['state', 'State', '◆'],
    ['timers', 'Timers', '◷'], ['look', 'Look', '✦'], ['links', 'Links', '↗'], ['other', 'Other numbers', '#'], ['parts', 'Parts', '▦'],
  ];

  window.ActorOverview = function createOverview(api) {
    // api: {set(path, value) -> Promise, flash(text), isPinned(key), togglePin(key), reveal(name)}
    const root = el('div', { className: 'ov' });
    let updaters = [], tracked = new Set(), hist = new Map(), vital = null, worldOnly = null;
    const expanded = new Set();

    function send(path) {
      let inflight = false, pending;
      const go = () => {
        const x = pending;
        pending = undefined; inflight = true;
        Promise.resolve(api.set(path, x)).then((r) => {
          inflight = false;
          if (r && r.error) api.flash('Could not set ' + path.join('.') + ': ' + r.error);
          if (pending !== undefined) go();
        });
      };
      return (v) => { pending = v; if (!inflight) go(); };
    }
    const keyOf = (path) => path.join('.');
    const bind = (fn) => updaters.push(fn);

    // ---- widgets: each returns {el, update(node)} ------------------------------
    // a number you drag sideways to change, or click to type
    function scrubber(path, n, cls = '') {
      const [lo, hi] = rangeOf(n), span = hi - lo, float = isFloat(n);
      const e = el('span', { className: 'ov-num ' + cls, tabIndex: 0, title: 'drag sideways to change (Shift: finer) · click to type' });
      const put = send(path);
      let v = n.v, quietUntil = 0, drag = null;
      const show = () => { e.textContent = fmt(v, float); };
      const commit = (nv) => {
        nv = float ? Math.round(nv * 1000) / 1000 : Math.round(nv);
        nv = Math.max(lo, Math.min(hi, nv));
        if (nv === v) return;
        v = nv; show(); put(v);
        quietUntil = performance.now() + 500;
      };
      show();
      const per = float ? 0.02 : span <= 16 ? 1 / 14 : span <= 256 ? 1 / 3 : 1;
      e.addEventListener('pointerdown', (ev) => {
        if (ev.button || e.querySelector('input')) return;
        ev.preventDefault();
        drag = { x: ev.clientX, v0: v, moved: false };
        e.setPointerCapture(ev.pointerId);
      });
      e.addEventListener('pointermove', (ev) => {
        if (!drag) return;
        const dx = ev.clientX - drag.x;
        if (!drag.moved && Math.abs(dx) < 3) return;
        if (!drag.moved) { drag.moved = true; e.classList.add('scrub'); document.body.classList.add('ov-scrubbing'); }
        let d = dx * per * (ev.shiftKey ? 0.2 : 1);
        if (!float && span > 256) d *= 1 + Math.abs(dx) / 90;
        commit(drag.v0 + d);
      });
      const end = (typed) => {
        if (!drag) return;
        const moved = drag.moved;
        drag = null;
        e.classList.remove('scrub'); document.body.classList.remove('ov-scrubbing');
        if (!moved && typed) typeIn();
      };
      e.addEventListener('pointerup', () => end(true));
      e.addEventListener('pointercancel', () => end(false));
      e.addEventListener('keydown', (ev) => {
        if (e.querySelector('input')) return;
        const step = ev.shiftKey ? 10 : 1;
        if (ev.key === 'ArrowUp' || ev.key === 'ArrowRight') { ev.preventDefault(); commit(v + step * (float ? 0.1 : 1)); }
        else if (ev.key === 'ArrowDown' || ev.key === 'ArrowLeft') { ev.preventDefault(); commit(v - step * (float ? 0.1 : 1)); }
        else if (ev.key === 'Enter') { ev.preventDefault(); typeIn(); }
      });
      function typeIn() {
        quietUntil = Infinity;
        const input = el('input', { className: 'ov-input', value: String(v), spellcheck: false });
        e.replaceChildren(input);
        input.focus(); input.select();
        let finished = false;
        const done = (ok) => {
          if (finished) return;
          finished = true;
          quietUntil = 0;
          const text = input.value.trim();
          show();
          if (!ok) return;
          const nv = /^-?0x/i.test(text) ? parseInt(text, 16) : Number(text);
          if (Number.isFinite(nv)) commit(nv);
        };
        input.onkeydown = (ev) => { ev.stopPropagation(); if (ev.key === 'Enter') done(true); else if (ev.key === 'Escape') done(false); };
        input.onblur = () => done(true);
      }
      return {
        el: e,
        get value() { return v; },
        set: commit,
        update(nn) {
          if (performance.now() < quietUntil || drag || nn.v === v) return false;
          v = nn.v; show(); pulse(e);
          return true;
        },
      };
    }

    // current / maximum as a bar; drag on the bar to set the current value
    function vitalBar(cn, mn) {
      const cur = scrubber([cn.n], cn, 'ov-cur'), max = scrubber([mn.n], mn, 'ov-max');
      const fill = el('div', { className: 'ov-fill' }), ghost = el('div', { className: 'ov-ghost' });
      const pct = el('span', { className: 'ov-pct' });
      const track = el('div', { className: 'ov-bar', title: 'drag to set' }, ghost, fill, el('div', { className: 'ov-shine' }));
      let last = -1;
      const ratio = () => (max.value > 0 ? Math.max(0, Math.min(1, cur.value / max.value)) : 0);
      const paint = () => {
        const r = ratio();
        if (r === last) return;
        ghost.style.transition = r < last ? 'width .7s cubic-bezier(.3,0,.2,1) .35s' : 'none';
        fill.style.width = ghost.style.width = (r * 100).toFixed(2) + '%';
        fill.style.setProperty('--h', Math.round(r * 125));
        pct.textContent = Math.round(r * 100) + '%';
        last = r;
      };
      let dragging = false;
      const at = (ev) => {
        const b = track.getBoundingClientRect();
        cur.set(Math.round(Math.max(0, Math.min(1, (ev.clientX - b.left) / b.width)) * max.value));
        paint();
      };
      track.addEventListener('pointerdown', (ev) => { dragging = true; track.setPointerCapture(ev.pointerId); at(ev); });
      track.addEventListener('pointermove', (ev) => { if (dragging) at(ev); });
      track.addEventListener('pointerup', () => { dragging = false; });
      paint();
      if (!vital) vital = ratio;
      return {
        el: el('div', { className: 'ov-vital' }, el('div', { className: 'ov-vital-nums' }, cur.el, el('span', { className: 'ov-slash', textContent: '/' }), max.el, pct), track),
        keys: [cn.n, mn.n],
        update(vals) { const a = vals.get(String(cn.n)), b = vals.get(String(mn.n)); if (a) cur.update(a); if (b) max.update(b); paint(); },
      };
    }

    // a heading as a compass dial; drag around it to turn
    // how many steps make a full turn: facing8 -> 8; a small value starts as an
    // 8-way compass (Player.facing runs 0-7 clockwise from up) and widens to the
    // type's range once a bigger value shows up
    function turnOf(n, v) {
      const name = String(n.n), full = 2 ** Math.min(16, widthOf(n));
      const fixed = /4$/.test(name) ? 4 : /8$/.test(name) ? 8 : /16$/.test(name) ? 16 : 0;
      if (fixed) return fixed;
      return Math.abs(v) < 8 && full >= 256 ? 8 : full;
    }
    function dial(path, n) {
      const [lo, hi] = rangeOf(n);
      let turn = turnOf(n, n.v);
      const num = scrubber(path, n, 'ov-dial-num');
      const ticks = svg('g', { class: 'ov-ticks' });
      const drawTicks = () => {
        ticks.replaceChildren();
        const marks = turn <= 16 ? turn : 16;
        for (let i = 0; i < marks; i++) {
          const a = i / marks * Math.PI * 2, r0 = i % (marks / 4 || 1) ? 23 : 20;
          ticks.append(svg('line', { x1: 32 + Math.sin(a) * r0, y1: 32 - Math.cos(a) * r0, x2: 32 + Math.sin(a) * 27, y2: 32 - Math.cos(a) * 27 }));
        }
      };
      drawTicks();
      const needle = svg('g', { class: 'ov-needle' }, svg('line', { x1: 32, y1: 32, x2: 32, y2: 9 }), svg('circle', { cx: 32, cy: 9, r: 3.6 }));
      const face = svg('svg', { viewBox: '0 0 64 64', class: 'ov-dial' }, svg('circle', { cx: 32, cy: 32, r: 28, class: 'ov-ring' }), ticks, needle, svg('circle', { cx: 32, cy: 32, r: 3, class: 'ov-hub' }));
      const deg = el('span', { className: 'ov-deg' });
      const draw = () => {
        if (Math.abs(num.value) >= turn && turn < 2 ** Math.min(16, widthOf(n)) && !/(4|8|16)$/.test(String(n.n))) { turn = 2 ** Math.min(16, widthOf(n)); drawTicks(); }
        const a = (((num.value % turn) + turn) % turn) / turn * 360;
        needle.setAttribute('transform', `rotate(${a.toFixed(1)} 32 32)`);
        deg.textContent = turn === 8 ? '↑↗→↘↓↙←↖'[Math.round(a / 45) % 8] + ' of 8' : turn === 4 ? '↑→↓←'[Math.round(a / 90) % 4] + ' of 4' : turn >= 256 ? Math.round(a) + '°' : `${num.value} of ${turn}`;
      };
      let dragging = false;
      const at = (ev) => {
        const b = face.getBoundingClientRect();
        const a = Math.atan2(ev.clientX - (b.left + b.width / 2), -(ev.clientY - (b.top + b.height / 2)));
        let v = Math.round(((a / (Math.PI * 2)) + 1) % 1 * turn) % turn;
        if (v > hi) v -= turn;
        if (v < lo) v += turn;
        num.set(v); draw();
      };
      face.addEventListener('pointerdown', (ev) => { dragging = true; face.setPointerCapture(ev.pointerId); at(ev); });
      face.addEventListener('pointermove', (ev) => { if (dragging) at(ev); });
      face.addEventListener('pointerup', () => { dragging = false; });
      draw();
      return { el: el('div', { className: 'ov-dialbox' }, face, el('div', { className: 'ov-dial-read' }, num.el, deg)), update(nn) { if (!dragging) num.update(nn); draw(); } };
    }

    // a flag word as lit cells; click one to flip that bit
    function bitGrid(path, n) {
      const width = Math.min(32, widthOf(n)), [lo] = rangeOf(n), put = send(path);
      const cells = [];
      const grid = el('div', { className: 'ov-bits', style: `--cols:${Math.min(width, 16)}` });
      let v = n.v, quietUntil = 0;
      for (let i = width - 1; i >= 0; i--) {
        const b = el('button', { className: 'ov-bit' + ((i >> 2) & 1 ? ' alt' : ''), title: `bit ${i} (0x${(2 ** i).toString(16)})` });
        b.onclick = () => {
          let nv = v ^ (1 << i);
          nv = lo < 0 ? nv | 0 : nv >>> 0;
          if (width < 32) nv &= 2 ** width - 1;
          v = nv; paint(); put(v); quietUntil = performance.now() + 500;
        };
        cells[i] = b;
        grid.append(b);
      }
      const hx = el('span', { className: 'ov-hex' });
      const count = el('span', { className: 'ov-bitcount' });
      const paint = (changed = 0) => {
        let on = 0;
        for (let i = 0; i < width; i++) {
          const lit = ((v >>> i) & 1) === 1;
          on += lit;
          cells[i].classList.toggle('on', lit);
          if ((changed >>> i) & 1) pulse(cells[i]);
        }
        hx.textContent = '0x' + (v >>> 0).toString(16).padStart(Math.ceil(width / 4), '0');
        count.textContent = on ? `${on} set` : 'clear';
      };
      paint();
      return {
        el: el('div', { className: 'ov-bitbox' }, grid, el('div', { className: 'ov-bitmeta' }, hx, count)),
        head: [hx, count],
        update(nn) {
          if (performance.now() < quietUntil || nn.v === v) return;
          const changed = (v ^ nn.v) >>> 0;
          v = nn.v; paint(changed);
        },
      };
    }

    function toggle(path, n) {
      const put = send(path);
      const b = el('button', { className: 'ov-switch', role: 'switch' }, el('i'));
      let v = n.v, quietUntil = 0;
      const paint = () => {
        b.classList.toggle('on', !!v);
        b.setAttribute('aria-checked', String(!!v));
        b.title = v === 0 || v === 1 ? (v ? 'on' : 'off') : `on (${v})`;
      };
      b.onclick = () => { v = v ? 0 : 1; paint(); put(v); quietUntil = performance.now() + 500; };
      paint();
      return { el: b, update(nn) { if (performance.now() < quietUntil || nn.v === v) return; v = nn.v; paint(); pulse(b); } };
    }

    function enumSelect(path, n, values) {
      if (!values) return scrubber(path, n);
      const put = send(path);
      const s = el('select', { className: 'ov-select' });
      for (const [name, label, value] of enumLabels(values)) s.add(new Option(label, value)), s.options[s.options.length - 1].title = name;
      const show = (v) => {
        if (![...s.options].some((o) => Number(o.value) === v)) s.add(new Option(`${v} (unnamed)`, v));
        s.value = String(v);
      };
      show(n.v);
      let quietUntil = 0;
      s.onchange = () => { put(Number(s.value)); quietUntil = performance.now() + 500; };
      return { el: s, update(nn) { if (document.activeElement === s || performance.now() < quietUntil || Number(s.value) === nn.v) return; show(nn.v); pulse(s); } };
    }

    function spark(c, values, color) {
      const x = c.getContext('2d'), w = c.width, h = c.height;
      x.clearRect(0, 0, w, h);
      if (!values || values.length < 2) return;
      let lo = Math.min(...values), hi = Math.max(...values);
      if (hi - lo < 4) { const mid = (hi + lo) / 2; lo = mid - 2; hi = mid + 2; }
      const pts = values.map((v, i) => [(i / 119) * w + (w - (values.length - 1) / 119 * w), h - 2 - ((v - lo) / (hi - lo)) * (h - 4)]);
      const grad = x.createLinearGradient(0, 0, 0, h);
      grad.addColorStop(0, color + '55'); grad.addColorStop(1, color + '00');
      x.beginPath();
      pts.forEach(([px, py], i) => (i ? x.lineTo(px, py) : x.moveTo(px, py)));
      x.lineTo(pts[pts.length - 1][0], h); x.lineTo(pts[0][0], h); x.closePath();
      x.fillStyle = grad; x.fill();
      x.beginPath();
      pts.forEach(([px, py], i) => (i ? x.lineTo(px, py) : x.moveTo(px, py)));
      x.strokeStyle = color; x.lineWidth = 1.5; x.stroke();
      const [ex, ey] = pts[pts.length - 1];
      x.beginPath(); x.arc(ex - 1.5, ey, 2, 0, Math.PI * 2); x.fillStyle = color; x.fill();
    }

    // a timer: its number, a trace of its last 12 seconds, a dot while it ticks
    function timer(path, n, tile) {
      const num = scrubber(path, n);
      const c = el('canvas', { className: 'ov-trace', width: 120, height: 22 });
      const key = keyOf(path);
      tracked.add(key);
      let still = 0;
      return {
        el: el('div', { className: 'ov-timer' }, num.el, c),
        update(nn) {
          const moved = num.update(nn);
          still = moved ? 0 : still + 1;
          tile.classList.toggle('ticking', still < 3);
          tile.classList.toggle('idle', nn.v === 0 && still >= 3);
          spark(c, hist.get(key), '#f2c94c');
        },
      };
    }

    function chip(path, n) {
      const fn = n.t === 'fn *' || /\(\*\)/.test(n.t);
      const target = n.t.replace(/\s*\*$/, '');
      const c = el('button', { className: 'ov-chip' });
      let v = null;
      const show = (nn) => {
        v = nn.v;
        c.className = 'ov-chip' + (fn ? ' fn' : '') + (v ? '' : ' null');
        c.textContent = !v ? 'NULL' : fn ? 'ƒ ' + (nn.sym || hex(v)) : (nn.sym ? nn.sym : hex(v));
        c.title = `${n.t} → ${hex(v)}` + (v && !fn ? '\nclick: open the actor it points into, or copy a typed view' : v ? '\nclick: copy' : '');
      };
      show(n);
      c.onclick = async () => {
        if (!v) return;
        if (!fn && api.follow && await api.follow(v)) return;
        const text = fn ? (n.sym || hex(v)) : target !== 'void' && !/^fn/.test(target) ? `g.at(${hex(v)}, '${target}')` : hex(v);
        navigator.clipboard.writeText(text).then(() => api.flash('Copied ' + text), () => api.flash(text));
      };
      const box = el('span', { className: 'ov-chipbox' }, !fn && target !== 'void' ? el('span', { className: 'ov-target', textContent: target.replace(/^struct /, '') }) : null, c);
      return { el: box, update(nn) { if (nn.v !== v) { show(nn); pulse(c); } } };
    }

    // x, y, z side by side (a SceneVec, or posX/posZ siblings); an arrow for velocities
    const AXIS = { x: '#ff6b6b', y: '#6fe39a', z: '#4fa3ff' };
    function vector(vec, big) {
      const axes = vec.axes.map((a) => {
        const s = scrubber(a.path, a.node, big ? 'big' : '');
        const c = big ? el('canvas', { className: 'ov-trace', width: 120, height: 26 }) : null;
        if (big) tracked.add(keyOf(a.path));
        return { a, s, c };
      });
      const box = el('div', { className: 'ov-vec' + (big ? ' big' : '') },
        ...axes.map((x) => el('div', { className: 'ov-axis' }, el('span', { className: 'ov-axis-l', textContent: x.a.label, style: `color:${AXIS[x.a.label]}` }), x.s.el, x.c)));
      let arrow = null;
      if (vec.arrow) {
        arrow = el('canvas', { className: 'ov-arrow', width: 64, height: 64 });
        box.append(arrow);
      }
      const drawArrow = () => {
        if (!arrow) return;
        const get = (l) => { const x = axes.find((q) => q.a.label === l); return x ? x.s.value : 0; };
        const vx = get('x'), vz = get('z');
        const x = arrow.getContext('2d');
        x.clearRect(0, 0, 64, 64);
        // the game's isometric floor: +x runs down-right, +z down-left
        x.strokeStyle = 'rgba(255,255,255,.12)'; x.lineWidth = 1;
        x.beginPath(); x.moveTo(32, 8); x.lineTo(58, 21); x.lineTo(32, 34 + 0); x.lineTo(6, 21); x.closePath();
        x.save(); x.translate(0, 11); x.stroke(); x.restore();
        const sx = vx - vz, sy = (vx + vz) / 2, len = Math.hypot(sx, sy);
        if (!len) { x.fillStyle = 'rgba(255,255,255,.35)'; x.beginPath(); x.arc(32, 32, 2.5, 0, 7); x.fill(); return; }
        const k = Math.min(24, 6 + Math.log2(1 + len) * 3.2) / len;
        const ex = 32 + sx * k, ey = 32 + sy * k, a = Math.atan2(sy, sx);
        x.strokeStyle = '#4fd1ff'; x.fillStyle = '#4fd1ff'; x.lineWidth = 2.5; x.lineCap = 'round';
        x.shadowColor = '#4fd1ff'; x.shadowBlur = 8;
        x.beginPath(); x.moveTo(32, 32); x.lineTo(ex, ey); x.stroke();
        x.beginPath(); x.moveTo(ex + Math.cos(a) * 4, ey + Math.sin(a) * 4);
        x.lineTo(ex + Math.cos(a + 2.4) * 7, ey + Math.sin(a + 2.4) * 7); x.lineTo(ex + Math.cos(a - 2.4) * 7, ey + Math.sin(a - 2.4) * 7); x.fill();
      };
      drawArrow();
      return {
        el: box,
        update(vals) {
          for (const x of axes) {
            const k = keyOf(x.a.path), nn = vals.get(k);
            if (nn) x.s.update(nn);
            if (x.c) spark(x.c, hist.get(k), AXIS[x.a.label]);
          }
          drawArrow();
        },
      };
    }

    // ---- layout ---------------------------------------------------------------
    function tile(path, label, body, { wide = false, kind = '', node = null } = {}) {
      const key = keyOf(path);
      const pin = el('button', { className: 'ov-pin' + (api.isPinned(key) ? ' on' : ''), textContent: api.isPinned(key) ? '★' : '☆', title: api.isPinned(key) ? 'Unpin' : 'Pin to the top' });
      pin.onclick = (e) => { e.stopPropagation(); api.togglePin(key); };
      const lab = el('div', { className: 'ov-label' }, el('span', { className: 'ov-name', textContent: label }), kind === 'timer' ? el('i', { className: 'ov-dot' }) : null, pin);
      if (node) lab.title = `${path.join('.')} · ${node.t} · +0x${node.o.toString(16)}`;
      const t = el('div', { className: 'ov-tile' + (wide ? ' wide' : '') + (kind ? ' k-' + kind : '') }, lab, body);
      return t;
    }
    function widget(path, n, kind, enums) {
      const key = keyOf(path);
      const holder = el('div');
      const t = tile(path, human(n.n), holder, { kind, node: n, wide: kind === 'bits' && widthOf(n) > 8 });
      const w = kind === 'chip' ? chip(path, n)
        : kind === 'enum' ? enumSelect(path, n, enums && enums[n.t])
          : kind === 'bits' ? bitGrid(path, n)
            : kind === 'switch' ? toggle(path, n)
              : kind === 'dial' ? dial(path, n)
                : kind === 'timer' ? timer(path, n, t)
                  : scrubber(path, n);
      holder.replaceWith(w.el);
      let prev = n.v;
      bind((vals) => {
        const nn = vals.get(key);
        if (!nn) return;
        if (nn.v !== prev) { prev = nn.v; t.changedAt = performance.now(); }
        w.update(nn);
      });
      t.score = score(n.n, n, kind);
      return t;
    }
    // how telling a field probably is: short plain names, live values and rich widgets first
    function score(name, n, kind) {
      const text = String(name), words = human(name).split(' ').length;
      let s = words <= 1 ? 3 : words === 2 ? 2 : words === 3 ? 1 : 0;
      if (/\d/.test(text)) s -= 4;
      if (/^(previous|saved|secondary|alternate|stored|last|reset|history|pending|auxiliary|tracked|recovery)/i.test(text)) s -= 2;
      if (n && typeof n.v === 'number' && n.v !== 0) s += 2;
      if (kind === 'dial') s += 2;
      if (kind === 'enum' || kind === 'timer') s += 1;
      return s;
    }
    // each section shows its few most telling fields; the rest wait behind "N more",
    // which says how many of them are changing right now
    const SHOWN = { vitals: 4, motion: 5, state: 8, timers: 6, look: 4, links: 4 };
    function section(id, items, extra) {
      if (!items.length) return null;
      const [, title, icon] = SECTIONS.find((s) => s[0] === id);
      const grid = el('div', { className: 'ov-grid' });
      const cap = SHOWN[id];
      let more = null;
      if (cap && items.length > cap + 1) {
        const order = items.map((t, i) => ({ t, i })).sort((a, b) => (b.t.score || 0) - (a.t.score || 0) || a.i - b.i);
        const featured = new Set(order.slice(0, cap).map((x) => x.t));
        const top = items.filter((t) => featured.has(t)), rest = items.filter((t) => !featured.has(t));
        grid.append(...top, ...rest);
        const label = el('span'), live = el('span', { className: 'ov-live' });
        more = el('button', { className: 'ov-more' }, label, live);
        const show = (open) => {
          for (const t of rest) t.hidden = !open;
          label.textContent = open ? 'Show fewer' : `${rest.length} more`;
          live.hidden = open;
          more.classList.toggle('open', open);
        };
        show(expanded.has(id));
        more.onclick = () => { const open = !expanded.has(id); if (open) expanded.add(id); else expanded.delete(id); show(open); };
        bind(() => {
          if (expanded.has(id)) return;
          const now = performance.now();
          const k = rest.filter((t) => now - (t.changedAt || -1e9) < 1500).length;
          live.textContent = k ? ` · ${k} changing` : '';
        });
      } else grid.append(...items);
      const head = el('h3', {}, el('span', { className: 'ov-ico', textContent: icon }), title, el('span', { className: 'ov-count', textContent: extra || '' }));
      if (id === 'other') {
        const d = el('details', { className: 'ov-sec ov-' + id }, el('summary', {}, head), grid);
        return d;
      }
      return el('section', { className: 'ov-sec ov-' + id }, head, grid, more);
    }

    function build(nodes, info, enums, pins) {
      updaters = []; tracked = new Set(); vital = null; worldOnly = null;
      const top = nodes || [];
      const byKey = new Map();
      (function walk(list, path) {
        for (const n of list || []) {
          const p = path.concat(n.n);
          if (n.c) walk(n.c, p); else if (!n.gap) byKey.set(keyOf(p), { n, p });
        }
      })(top, []);

      const leaves = [], vecs = [], parts = [];
      for (const n of top) {
        if (n.pad || n.gap) continue;
        if (n.c) {
          const kids = n.c.filter((k) => !k.c && !k.gap);
          const names = kids.map((k) => k.n);
          if (['x', 'y', 'z'].every((a) => names.includes(a))) {
            vecs.push({ name: n.n, axes: ['x', 'y', 'z'].map((a) => ({ label: a, path: [n.n, a], node: kids.find((k) => k.n === a) })) });
          } else if (n.c.length === 3 && kids.length === 3 && names.every((k, i) => k === i) && MOTION.test(n.n)) {
            vecs.push({ name: n.n, axes: [0, 1, 2].map((i) => ({ label: 'xyz'[i], path: [n.n, i], node: kids[i] })), arrow: /veloc|speed|delta/i.test(n.n) });
          } else parts.push(n);
          continue;
        }
        if (n.more) { parts.push(n); continue; }
        if (typeof n.v === 'number') leaves.push(n);
      }
      const used = new Set();
      // velocityX / velocityZ, cameraOffsetX/Y/Z: one vector
      const groups = new Map();
      for (const n of leaves) {
        // velocityX, velocityZ; or plain x, y, z (the actor's own position)
        const m = /^(.+?)([XYZ])$/.exec(String(n.n)) || /^()([xyz])$/.exec(String(n.n));
        if (!m || n.ptr || 'enumName' in n) continue;
        const base = m[1] || 'position';
        if (!groups.has(base)) groups.set(base, {});
        groups.get(base)[m[2].toUpperCase()] = n;
      }
      for (const [base, gr] of groups) {
        const axes = ['X', 'Y', 'Z'].filter((a) => gr[a]);
        if (axes.length < 2) continue;
        axes.forEach((a) => used.add(gr[a]));
        vecs.push({ name: base, axes: axes.map((a) => ({ label: a.toLowerCase(), path: [gr[a].n], node: gr[a] })), arrow: !!(gr.X && gr.Z) && /veloc|speed|delta|move|push|knock|accel/i.test(base) });
      }
      // current / maximum pairs: currentStatA + maximumStatA, life + maxLife, secondaryValue + secondaryMaximum
      const isMax = (s) => /^(max|maximum)(?=[A-Z_]|$)|(Max|Maximum|Limit)$/.test(s);
      const norm = (s) => s.toLowerCase().replace(/^(current|cur|maximum|max)/, '').replace(/(maximum|max|limit|value|current|cur)$/, '');
      const pairs = [];
      for (const m of leaves) {
        if (used.has(m) || m.ptr || !isMax(String(m.n))) continue;
        const k = norm(String(m.n));
        const c = leaves.find((x) => x !== m && !used.has(x) && !x.ptr && !isMax(String(x.n)) && norm(String(x.n)) === k);
        if (!c) continue;
        used.add(c); used.add(m);
        pairs.push([c, m]);
      }

      const S = { pinned: [], vitals: [], motion: [], state: [], timers: [], look: [], links: [], other: [], parts: [] };
      // pinned fields first, in the order they were pinned
      for (const k of pins) {
        const hit = byKey.get(k);
        if (!hit) continue;
        const [, kind] = classify(hit.n);
        const t = widget(hit.p, hit.n, kind, enums);
        t.querySelector('.ov-name').textContent = hit.p.map(human).join(' · ');
        S.pinned.push(t);
      }
      for (const [c, m] of pairs) {
        const w = vitalBar(c, m);
        const title = human(norm(String(c.n)) ? String(c.n).replace(/^(current|cur)/i, '').replace(/(Value|Current)$/, '') || String(c.n) : String(c.n));
        const t = tile([c.n], title, w.el, { wide: true, kind: 'vital', node: c });
        bind((vals) => w.update(vals));
        t.score = 50;
        S.vitals.push(t);
      }
      // the actor's own position leads Motion, with a trace per axis
      const main = vecs.find((v) => /^(position|pos)$/i.test(v.name)) || null;
      const ordered = main ? [main, ...vecs.filter((v) => v !== main)] : vecs;
      if (!main && info && info.world) {
        // no position field in the struct (or no struct): show where it is drawn
        const box = el('div', { className: 'ov-vec big' });
        const axes = ['x', 'y', 'z'].map((a, i) => {
          const v = el('span', { className: 'ov-num big ro', textContent: info.world[i] });
          const c = el('canvas', { className: 'ov-trace', width: 120, height: 26 });
          box.append(el('div', { className: 'ov-axis' }, el('span', { className: 'ov-axis-l', textContent: a, style: `color:${AXIS[a]}` }), v, c));
          return { v, c, a, i };
        });
        worldOnly = { axes, hist: [] };
        const t = tile(['$world'], 'World position (drawn at)', box, { wide: true });
        t.score = 100;
        S.motion.push(t);
      }
      for (const v of ordered) {
        const w = vector(v, v === main);
        const t = tile([v.name], human(v.name), w.el, { wide: v === main || v.arrow || v.axes.length > 2, kind: 'vec' });
        let prev = '';
        bind((vals) => {
          w.update(vals);
          const now = v.axes.map((a) => { const nn = vals.get(keyOf(a.path)); return nn ? nn.v : ''; }).join(',');
          if (prev && now !== prev) t.changedAt = performance.now();
          prev = now;
        });
        t.score = v === main ? 100 : score(v.name, { v: v.axes.some((a) => a.node && a.node.v) ? 1 : 0 }, 'vec') + (v.arrow ? 2 : 0);
        S.motion.push(t);
      }
      const rest = leaves.filter((n) => !used.has(n)).map((n) => ({ n, c: classify(n) }));
      const rank = { dial: 0, switch: 1, enum: 2, num: 3, bits: 4, timer: 5, chip: 6 };
      rest.sort((a, b) => (rank[a.c[1]] ?? 9) - (rank[b.c[1]] ?? 9));
      for (const { n, c: [sec, kind] } of rest) S[sec].push(widget([n.n], n, kind, enums));
      for (const n of parts.filter((p) => !p.pad)) {
        const b = el('button', { className: 'ov-part', title: `${n.t} · +0x${n.o.toString(16)}` }, el('b', { textContent: human(n.n) }), el('span', { textContent: n.t }));
        b.onclick = () => api.reveal(String(n.n));
        S.parts.push(b);
      }

      const out = [];
      for (const [id] of SECTIONS) {
        const items = S[id];
        const extra = id === 'other' ? `${items.length}` : id === 'parts' ? 'open in All fields' : '';
        const s = section(id, items, extra);
        if (s) out.push(s);
      }
      if (!out.length) out.push(el('p', { className: 'ap-note', textContent: 'Nothing to sort here yet: see All fields.' }));
      root.replaceChildren(...out);
    }

    function update(nodes, info) {
      const vals = new Map();
      (function walk(list, path) {
        for (const n of list || []) {
          const p = path ? path + '.' + n.n : String(n.n);
          if (n.c) walk(n.c, p); else vals.set(p, n);
        }
      })(nodes, '');
      for (const k of tracked) {
        const n = vals.get(k);
        if (!n) continue;
        let h = hist.get(k);
        if (!h) hist.set(k, h = []);
        h.push(n.v);
        if (h.length > 120) h.shift();
      }
      for (const u of updaters) u(vals);
      if (worldOnly && info && info.world) {
        worldOnly.hist.push(info.world.slice());
        if (worldOnly.hist.length > 120) worldOnly.hist.shift();
        for (const a of worldOnly.axes) {
          a.v.textContent = info.world[a.i];
          spark(a.c, worldOnly.hist.map((w) => w[a.i]), AXIS[a.a]);
        }
      }
    }

    return {
      root, build, update,
      reset() { hist.clear(); },
      vital() { return vital ? vital() : null; },
    };
  };
  window.ActorOverview.human = human;
})();
