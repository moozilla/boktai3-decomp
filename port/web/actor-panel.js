// Actor panel: Inspect (I), then click a sprite in the game. A panel docks on
// the right with the actor's sprite, a 3D diorama of the scene (every projected
// sprite stands at its world position as a billboard of its real pixels, over
// the screen projected onto the floor) and its own struct, typed by its task's
// update function and editable live. Overview sorts the struct into sections
// with widgets (actor-overview.js); All fields is the whole tree. In the game,
// a reticle follows the actor. The worker side is port/web/actor.mjs.
(function () {
  'use strict';
  const $ = (id) => document.getElementById(id);
  function el(tag, props = {}, ...kids) {
    const e = Object.assign(document.createElement(tag), props);
    for (const k of kids) if (k != null) e.append(k);
    return e;
  }
  const hex = (n) => '0x' + (n >>> 0).toString(16).padStart(8, '0');
  const SCALE = 1 / 16; // world units -> three.js units
  const store = {
    get(k, d) { try { const v = localStorage.getItem(k); return v == null ? d : JSON.parse(v); } catch (e) { return d; } },
    set(k, v) { try { localStorage.setItem(k, JSON.stringify(v)); } catch (e) { /* private window */ } },
  };

  let env = null, panel = null, sel = null, last = null, refs = {}, view = null, overview = null;
  let showPad = false, filter = '', structureKey = '', valueEls = new Map(), prevValues = new Map();
  let tab = store.get('boktai3.actorTab', 'overview'), tinted = 0;

  // "TerrainEdgeMarker_UpdateVisibility" -> "Terrain Edge Marker"
  function pretty(update) {
    if (!update || update === '?') return 'Sprite';
    const head = update.split('_')[0];
    return head.replace(/([a-z0-9])([A-Z])/g, '$1 $2').replace(/([A-Z]+)([A-Z][a-z])/g, '$1 $2');
  }

  // pinned fields, per struct type
  const typeOf = () => (last && last.info && last.info.owner && last.info.owner.type) || '';
  function pins() { return (store.get('boktai3.actorPins', {})[typeOf()]) || []; }
  function togglePin(key) {
    const all = store.get('boktai3.actorPins', {}), t = typeOf();
    const list = all[t] || [];
    const i = list.indexOf(key);
    if (i >= 0) list.splice(i, 1); else list.push(key);
    if (list.length) all[t] = list; else delete all[t];
    store.set('boktai3.actorPins', all);
    env.flash(i >= 0 ? 'Unpinned ' + key : 'Pinned ' + key + ' to the top of Overview');
    structureKey = '';
    render();
  }

  function init(e) { env = e; }

  function open(hit) {
    if (!hit.task) { env.flash('No task owns this sprite'); return; }
    sel = Number(hit.task);
    last = null; structureKey = ''; prevValues.clear(); tinted = 0;
    build();
    overview.reset();
    const settings = $('settings');
    if (settings && settings.open) settings.close();
    panel.hidden = false;
    document.body.classList.add('actor-open');
    refs.title.textContent = pretty(hit.owner);
    refs.chips.replaceChildren(el('span', { textContent: 'loading…' }));
    refs.ov.replaceChildren();
    env.request({ op: 'actor', task: sel });
    start3d();
  }
  function close() {
    if (!panel || panel.hidden) return;
    panel.hidden = true;
    document.body.classList.remove('actor-open');
    if (sel) env.request({ op: 'actor', task: 0 });
    sel = null;
    if (refs.ret) refs.ret.hidden = true;
    stop3d();
  }
  function isOpen() { return !!(panel && !panel.hidden); }

  // ---- layout ----------------------------------------------------------------
  function build() {
    if (panel) return;
    overview = window.ActorOverview({
      set: (path, value) => env.request({ op: 'actor-set', path, value }),
      flash: (t) => env.flash(t),
      isPinned: (key) => pins().includes(key),
      togglePin,
      reveal: (name) => { showTab('fields'); refs.filter.value = name; filter = name.toLowerCase(); applyFilter(); },
      follow: async (addr) => {
        // a pointer into another task's object: open that actor
        const r = await env.request({ op: 'js', code: `JSON.stringify(taskOf(${addr}))` });
        let t = null;
        try { t = JSON.parse(r.value); if (typeof t === 'string') t = JSON.parse(t); } catch (e) { t = null; }
        if (!t || Number(t.task) === sel) return false;
        open({ task: t.task, owner: t.update });
        env.flash('Followed the pointer to ' + pretty(t.update));
        return true;
      },
    });
    refs.thumb = el('canvas', { className: 'ap-thumb', width: 84, height: 84 });
    refs.title = el('strong', { className: 'ap-title' });
    refs.chips = el('div', { className: 'ap-chips' });
    refs.badge = el('span', { className: 'ap-badge' });
    const closeBtn = el('button', { className: 'ap-close', title: 'Close (Esc)', textContent: '×', onclick: close });
    refs.view = el('div', { className: 'ap-view' });
    refs.gl = el('canvas', { className: 'ap-gl' });
    refs.viewNote = el('div', { className: 'ap-view-note' });
    const angle = el('button', { textContent: 'Game angle', title: 'Look from where the game\'s camera looks', onclick: () => view && view.reset() });
    const spin = el('button', { textContent: 'Turntable', title: 'Swing slowly around the actor', onclick: () => view && view.turntable() });
    refs.view.append(refs.gl, refs.viewNote, el('div', { className: 'ap-view-tools' }, angle, spin),
      el('div', { className: 'ap-view-hint', textContent: 'drag to orbit · scroll to zoom · click an actor' }));
    refs.meta = el('div', { className: 'ap-meta' });
    refs.actions = el('div', { className: 'ap-actions' },
      el('button', { textContent: '⇢ Move player here', onclick: movePlayerHere }),
      refs.bring = el('button', { textContent: '⇠ Bring to player', onclick: bringToPlayer }),
      el('button', { textContent: '{ } Copy JS', title: 'A typed view for the console or port/agent/play.py js', onclick: copyJs }));
    refs.tabOverview = el('button', { textContent: 'Overview', onclick: () => showTab('overview') });
    refs.tabFields = el('button', { textContent: 'All fields', onclick: () => showTab('fields') });
    refs.tabs = el('div', { className: 'ap-tabs', role: 'tablist' }, refs.tabOverview, refs.tabFields);
    refs.ov = overview.root;
    refs.filter = el('input', { className: 'ap-filter', placeholder: 'Filter fields', type: 'search' });
    refs.filter.oninput = () => { filter = refs.filter.value.trim().toLowerCase(); applyFilter(); };
    const pad = el('label', { className: 'ap-pad' }, el('input', { type: 'checkbox', onchange: (e) => { showPad = e.target.checked; structureKey = ''; render(); } }), ' padding');
    refs.fields = el('div', { className: 'ap-fields' });
    refs.fieldsPane = el('div', { className: 'ap-fields-pane' }, el('div', { className: 'ap-fields-head' }, refs.filter, pad), refs.fields);
    panel = el('aside', { id: 'actor-panel', hidden: true },
      el('header', { className: 'ap-head' }, el('div', { className: 'ap-thumbwrap' }, refs.thumb),
        el('div', { className: 'ap-titles' }, el('div', { className: 'ap-titleline' }, refs.title, refs.badge), refs.chips), closeBtn),
      refs.view, refs.meta, refs.actions, refs.tabs, refs.ov, refs.fieldsPane);
    panel.addEventListener('keydown', (e) => { if (e.key === 'Escape' && !e.target.closest('input, select')) close(); });
    document.body.append(panel);
    showTab(tab);
  }
  function showTab(t) {
    tab = t;
    store.set('boktai3.actorTab', t);
    refs.tabOverview.classList.toggle('on', t === 'overview');
    refs.tabFields.classList.toggle('on', t === 'fields');
    refs.ov.hidden = t !== 'overview';
    refs.fieldsPane.hidden = t !== 'fields';
  }

  // ---- updates from the worker ------------------------------------------------
  function onMessage(m) {
    if (!sel || !panel) return;
    if (m.error) { refs.chips.replaceChildren(el('span', { textContent: m.error.split('\n')[0] })); return; }
    last = m;
    const info = m.info;
    const own = info.owner;
    refs.title.textContent = pretty(own && own.update);
    const chips = own
      ? [own.type || 'untyped', `task ${hex(own.task)}`, info.size ? info.size.toLocaleString() + ' bytes' : null, own.update]
      : [`task ${hex(sel)}`, 'ended'];
    const text = chips.filter(Boolean).join('|');
    if (refs.chips.dataset.text !== text) {
      refs.chips.dataset.text = text;
      refs.chips.replaceChildren(...chips.filter(Boolean).map((c, i) => el('span', { textContent: c, className: i === 0 && own && own.type ? 'type' : '' })));
    }
    refs.badge.textContent = info.alive ? (info.hidden ? 'hidden' : 'live') : 'gone';
    refs.badge.className = 'ap-badge ' + (info.alive ? (info.hidden ? 'dim' : 'live') : 'gone');
    refs.meta.textContent = (info.box ? `on screen at ${info.box[0]}, ${info.box[1]} · ${info.box[2]}×${info.box[3]} px · ${info.pieces} OAM ${info.pieces === 1 ? 'entry' : 'entries'}` : 'not drawn this frame') + ` · frame ${info.frame}`;
    refs.bring.hidden = !positionPath();
    const layer = m.layer ? new Uint8ClampedArray(m.layer) : null;
    if (layer && info.box) thumb(layer, info.box);
    render();
    reticle(info);
    if (view) view.update(m.scene, info, layer);
  }

  const crop = el('canvas', { width: 64, height: 64 });
  function cropLayer(layer, box) {
    const [bx, by, bw, bh] = box.map((v) => Math.round(v));
    const w = Math.max(1, Math.min(bw, 128)), h = Math.max(1, Math.min(bh, 128));
    const img = new ImageData(w, h);
    for (let y = 0; y < h; y++) {
      const sy = by + y;
      if (sy < 0 || sy >= 160) continue;
      for (let x = 0; x < w; x++) {
        const sx = bx + x;
        if (sx < 0 || sx >= 240) continue;
        const s = (sy * 240 + sx) * 4, d = (y * w + x) * 4;
        img.data[d] = layer[s]; img.data[d + 1] = layer[s + 1]; img.data[d + 2] = layer[s + 2]; img.data[d + 3] = layer[s + 3];
      }
    }
    return img;
  }
  function thumb(layer, box) {
    const img = cropLayer(layer, box);
    crop.width = img.width; crop.height = img.height;
    crop.getContext('2d').putImageData(img, 0, 0);
    const t = refs.thumb.getContext('2d'), S = refs.thumb.width;
    t.clearRect(0, 0, S, S);
    t.imageSmoothingEnabled = false;
    const k = Math.max(1, Math.floor((S - 6) / Math.max(img.width, img.height)));
    const w = img.width * k, h = img.height * k;
    t.drawImage(crop, (S - w) >> 1, (S - h) >> 1, w, h);
    if (tinted++ % 30 === 0) tint(img);
  }
  // the panel's accent: the sprite's most vivid color
  function tint(img) {
    let sx = 0, sy = 0, wsum = 0, light = 0;
    for (let i = 0; i < img.data.length; i += 4) {
      if (!img.data[i + 3]) continue;
      const r = img.data[i] / 255, g = img.data[i + 1] / 255, b = img.data[i + 2] / 255;
      const max = Math.max(r, g, b), min = Math.min(r, g, b), c = max - min;
      if (c < 0.12) continue;
      let h = max === r ? ((g - b) / c) % 6 : max === g ? (b - r) / c + 2 : (r - g) / c + 4;
      h *= Math.PI / 3;
      const w = c * c;
      sx += Math.cos(h) * w; sy += Math.sin(h) * w; wsum += w; light += max * w;
    }
    if (!wsum) { panel.style.removeProperty('--actor'); return; }
    const hue = ((Math.atan2(sy, sx) * 180 / Math.PI) + 360) % 360;
    panel.style.setProperty('--actor', `hsl(${hue.toFixed(0)} 85% 64%)`);
    if (refs.ret) refs.ret.style.setProperty('--actor', `hsl(${hue.toFixed(0)} 85% 64%)`);
  }

  // corner brackets around the actor in the game, with its name and a life bar
  function reticle(info) {
    if (!refs.ret) {
      const screen = env.canvas && env.canvas.parentElement;
      if (!screen) return;
      refs.retName = el('span');
      refs.retFill = el('i');
      refs.retBar = el('b', { className: 'ap-ret-bar' }, refs.retFill);
      refs.ret = el('div', { className: 'ap-reticle', hidden: true },
        el('i', { className: 'c tl' }), el('i', { className: 'c tr' }), el('i', { className: 'c bl' }), el('i', { className: 'c br' }),
        el('div', { className: 'ap-ret-label' }, refs.retName, refs.retBar));
      screen.append(refs.ret);
      const accent = panel.style.getPropertyValue('--actor');
      if (accent) refs.ret.style.setProperty('--actor', accent);
    }
    const b = info.box;
    if (!b || !isOpen()) { refs.ret.hidden = true; return; }
    const pad = 3;
    Object.assign(refs.ret.style, {
      left: ((b[0] - pad) / 240 * 100) + '%', top: ((b[1] - pad) / 160 * 100) + '%',
      width: ((b[2] + 2 * pad) / 240 * 100) + '%', height: ((b[3] + 2 * pad) / 160 * 100) + '%',
    });
    refs.ret.classList.toggle('below', b[1] < 22);
    refs.ret.hidden = false;
    refs.retName.textContent = pretty(info.owner && info.owner.update);
    const r = overview.vital();
    refs.retBar.hidden = r == null;
    if (r != null) { refs.retFill.style.width = (r * 100).toFixed(1) + '%'; refs.retFill.style.setProperty('--h', Math.round(r * 125)); }
  }

  // ---- fields -----------------------------------------------------------------
  function nodesKey(nodes) {
    let s = '';
    const walk = (list) => { for (const n of list) { s += n.n + ':' + (n.c ? '{' : ''); if (n.c) walk(n.c); } };
    walk(nodes || []);
    return s.length + ':' + s.slice(0, 2000);
  }
  function valueText(n) {
    if (n.ptr) return n.v ? hex(n.v) + (n.sym ? ' ' + n.sym : '') : 'NULL';
    if (n.enumName) return `${n.enumName} (${n.v})`;
    if (typeof n.v === 'number' && !Number.isInteger(n.v)) return n.v.toFixed(3);
    return n.v === undefined ? '' : String(n.v);
  }
  function render() {
    const info = last && last.info;
    if (!info) return;
    const nodes = info.fields || [];
    const typed = !!info.fields;
    const key = (typed ? 'T' : 'R') + nodesKey(nodes) + showPad + typeOf();
    if (key !== structureKey) {
      structureKey = key;
      valueEls.clear();
      const root = el('div', { className: 'ap-tree' });
      if (!typed) {
        root.append(el('p', { className: 'ap-note', textContent: info.owner
          ? `No struct type for ${info.owner.update}'s object yet: give its first parameter a struct type in src/ and it appears here. Its first bytes:`
          : 'This task has ended.' }));
        if (info.raw) root.append(el('pre', { className: 'ap-raw', textContent: info.raw.match(/.{1,48}/g).join('\n') }));
      }
      tree(root, nodes, [], 0, typed);
      refs.fields.replaceChildren(root);
      applyFilter();
      overview.build(typed ? nodes : [], info, info.enums || {}, pins());
      if (!typed) {
        refs.ov.prepend(el('p', { className: 'ap-note ov-untyped', textContent: info.owner
          ? `${info.owner.update} has no struct type yet, so there is nothing to sort: give its first parameter a struct type in src/.`
          : 'This task has ended.' }));
      }
    }
    // values: update in place, flash the ones that changed
    const walk = (list, path) => {
      for (const n of list || []) {
        const p = path.concat(n.n);
        if (n.c) { walk(n.c, p); continue; }
        const k = p.join('.');
        const e = valueEls.get(k);
        if (!e || e.classList.contains('editing')) continue;
        const text = valueText(n);
        if (e.textContent !== text) {
          if (prevValues.has(k)) { e.classList.remove('chg'); void e.offsetWidth; e.classList.add('chg'); }
          e.textContent = text;
        }
        prevValues.set(k, text);
      }
    };
    walk(nodes, []);
    overview.update(nodes, info);
  }
  function tree(container, nodes, path, depth, editable) {
    const pinned = pins();
    for (const n of nodes || []) {
      if (n.pad && !showPad) continue;
      const p = path.concat(n.n);
      const label = el('span', { className: 'ap-fname', textContent: typeof n.n === 'number' ? `[${n.n}]` : n.n });
      const type = el('span', { className: 'ap-ftype', textContent: n.t, title: `+0x${n.o.toString(16)}` });
      if (n.c) {
        const det = el('details', { className: 'ap-group' });
        if (depth === 0 && /^(position|velocity|pos|state|health|life|hp)$/i.test(n.n)) det.open = true;
        det.append(el('summary', {}, label, type));
        const body = el('div', { className: 'ap-kids' });
        det.append(body);
        tree(body, n.c, p, depth + 1, editable);
        container.append(det);
        continue;
      }
      if (n.gap) { container.append(el('div', { className: 'ap-row ap-gap', textContent: n.n })); continue; }
      const value = el('span', { className: 'ap-fval' + (n.ptr ? ' ptr' : ''), textContent: valueText(n) });
      if (typeof n.v === 'number' && Number.isInteger(n.v) && !n.ptr) value.title = hex(n.v);
      if (editable && !n.ptr && typeof n.v === 'number') {
        value.classList.add('editable');
        value.onclick = () => edit(value, p, n);
      }
      valueEls.set(p.join('.'), value);
      const key = p.join('.'), on = pinned.includes(key);
      const star = editable ? el('button', { className: 'ap-star' + (on ? ' on' : ''), textContent: on ? '★' : '☆', title: on ? 'Unpin' : 'Pin to Overview', onclick: () => togglePin(key) }) : null;
      const row = el('div', { className: 'ap-row' }, label, type, value, star);
      row.dataset.search = p.join('.').toLowerCase();
      container.append(row);
    }
  }
  function edit(span, path, n) {
    if (span.classList.contains('editing')) return;
    span.classList.add('editing');
    const input = el('input', { className: 'ap-edit', value: span.textContent.replace(/ \(.*\)$/, '') });
    span.replaceChildren(input);
    input.focus(); input.select();
    const done = async (commit) => {
      if (!span.classList.contains('editing')) return;
      span.classList.remove('editing');
      span.textContent = valueText(n);
      if (!commit) return;
      let text = input.value.trim(), v;
      if (n.enumName !== undefined && !/^-?(0x)?[0-9a-f]+$/i.test(text)) v = text;
      else v = /^-?0x/i.test(text) ? parseInt(text, 16) : Number(text);
      if (typeof v === 'number' && !Number.isFinite(v)) return;
      const r = await env.request({ op: 'actor-set', path, value: v });
      env.flash(r.error ? 'Could not set: ' + r.error : `${path.join('.')} = ${text}`);
    };
    input.onkeydown = (e) => { if (e.key === 'Enter') done(true); else if (e.key === 'Escape') { e.stopPropagation(); done(false); } };
    input.onblur = () => done(false);
  }
  function applyFilter() {
    const rows = refs.fields.querySelectorAll('.ap-row');
    for (const r of rows) r.hidden = !!filter && !(r.dataset.search || '').includes(filter);
    for (const d of refs.fields.querySelectorAll('details')) {
      const any = !filter || d.querySelector('.ap-row:not([hidden])');
      d.hidden = !any;
      if (filter && any) d.open = true;
    }
  }

  // ---- actions ------------------------------------------------------------------
  function positionPath() {
    const f = last && last.info && last.info.fields;
    if (!f) return null;
    const pos = f.find((n) => n.c && /^(position|pos)$/i.test(n.n));
    if (!pos) return null;
    const names = pos.c.map((n) => n.n);
    return ['x', 'y', 'z'].every((a) => names.includes(a)) ? pos.n : null;
  }
  async function movePlayerHere() {
    const w = last && last.info && last.info.world;
    if (!w) return env.flash('This sprite has no world position');
    const r = await env.request({ op: 'js', code: `const p = $.gPlayers[0]; if (p) { p.position.x = ${w[0]}; p.position.z = ${w[2] + 48}; } !!p` });
    env.flash(r.error ? r.error : 'Moved the player next to it');
  }
  async function bringToPlayer() {
    const at = positionPath();
    const r = await env.request({ op: 'js', code: 'const p = $.gPlayers[0]; p ? [p.position.x, p.position.y, p.position.z] : null' });
    let pos = null;
    try { pos = JSON.parse(r.value); } catch (e) { pos = null; }
    if (!at || !pos) return env.flash('No position to set');
    await env.request({ op: 'actor-set', path: [at, 'x'], value: pos[0] + 48 });
    await env.request({ op: 'actor-set', path: [at, 'z'], value: pos[2] });
    env.flash('Brought it to the player');
  }
  function copyJs() {
    const own = last && last.info && last.info.owner;
    const text = own && own.type ? `g.at(${hex(own.task)}, '${own.type}')` : `g.at(${hex(sel)}, 'struct Task')`;
    navigator.clipboard.writeText(text).then(() => env.flash('Copied ' + text), () => env.flash(text));
  }

  // ---- 3D diorama (three.js, loaded on first use) -------------------------------
  async function start3d() {
    if (view) { view.resume(); return; }
    refs.viewNote.textContent = 'loading three.js…';
    let THREE;
    try { THREE = await import('three'); } catch (e) {
      refs.viewNote.textContent = '3D view unavailable offline (three.js loads from jsDelivr)';
      return;
    }
    refs.viewNote.textContent = '';
    view = diorama(THREE, refs.gl, refs.view);
    if (last) view.update(last.scene, last.info, last.layer ? new Uint8ClampedArray(last.layer) : null);
  }
  function stop3d() { if (view) view.pause(); }

  function diorama(THREE, canvas, box) {
    const renderer = new THREE.WebGLRenderer({ canvas, antialias: true, alpha: true });
    renderer.setPixelRatio(Math.min(2, window.devicePixelRatio || 1));
    const scene = new THREE.Scene();
    const camera = new THREE.OrthographicCamera(-1, 1, 1, -1, -2000, 2000);
    const GAME = { yaw: Math.atan2(1, 1), pitch: Math.asin(2 / Math.sqrt(6)) }; // looking along -(1,2,1)
    const orbit = { yaw: GAME.yaw, pitch: GAME.pitch, zoom: 2.4, spin: false, t0: performance.now() };
    const target = new THREE.Vector3(), want = new THREE.Vector3();
    let running = true, raf = 0, haveTarget = false, selectedMesh = null;
    // a name tag that follows the selected actor
    const tag = document.createElement('div');
    tag.className = 'ap-tag';
    box.append(tag);

    const grid = new THREE.GridHelper(400, 40, 0x3a4050, 0x262a33);
    scene.add(grid);

    // floor: the game's picture projected onto the ground at the floor's height
    const floorCanvas = document.createElement('canvas');
    floorCanvas.width = 240; floorCanvas.height = 160;
    const floorTex = new THREE.CanvasTexture(floorCanvas);
    floorTex.magFilter = THREE.NearestFilter; floorTex.minFilter = THREE.LinearFilter;
    floorTex.colorSpace = THREE.SRGBColorSpace;
    const floorGeo = new THREE.BufferGeometry();
    floorGeo.setAttribute('position', new THREE.Float32BufferAttribute(new Float32Array(12), 3));
    floorGeo.setAttribute('uv', new THREE.Float32BufferAttribute([0, 1, 1, 1, 1, 0, 0, 0], 2));
    floorGeo.setIndex([0, 3, 1, 1, 3, 2]);
    const floor = new THREE.Mesh(floorGeo, new THREE.MeshBasicMaterial({ map: floorTex, transparent: true, opacity: 0.92, side: THREE.DoubleSide, depthWrite: false }));
    floor.renderOrder = -1;
    scene.add(floor);

    const ring = new THREE.Mesh(new THREE.RingGeometry(0.8, 1.15, 48), new THREE.MeshBasicMaterial({ color: 0xf2a33a, transparent: true, opacity: 0.9, side: THREE.DoubleSide, depthWrite: false }));
    ring.rotation.x = -Math.PI / 2;
    scene.add(ring);
    const stemGeo = new THREE.BufferGeometry().setFromPoints([new THREE.Vector3(), new THREE.Vector3(0, 1, 0)]);
    const stem = new THREE.Line(stemGeo, new THREE.LineDashedMaterial({ color: 0xf2a33a, dashSize: 0.6, gapSize: 0.4 }));
    scene.add(stem);

    const sprites = new Map(); // addr -> {mesh, shadow, canvas, tex}
    const shadowGeo = new THREE.CircleGeometry(1, 32);
    const proj = (v) => { v *= 3; return v >= 0 ? Math.floor(v / 32) : -Math.floor(-v / 32); };

    function corner(world, dx, dy) {
      // a screen offset from the sprite's anchor, on the upright plane facing the game camera
      return [(world[0] + dx * 16 / 3) * SCALE, (world[1] - dy * 32 / 3) * SCALE, (world[2] - dx * 16 / 3) * SCALE];
    }
    function spriteMesh(s, layer, selected) {
      let e = sprites.get(s.addr);
      if (!e) {
        const c = document.createElement('canvas');
        const tex = new THREE.CanvasTexture(c);
        tex.magFilter = THREE.NearestFilter; tex.minFilter = THREE.NearestFilter; tex.colorSpace = THREE.SRGBColorSpace;
        const geo = new THREE.BufferGeometry();
        geo.setAttribute('position', new THREE.Float32BufferAttribute(new Float32Array(12), 3));
        geo.setAttribute('uv', new THREE.Float32BufferAttribute([0, 1, 1, 1, 1, 0, 0, 0], 2));
        geo.setIndex([0, 3, 1, 1, 3, 2]);
        const mesh = new THREE.Mesh(geo, new THREE.MeshBasicMaterial({ map: tex, transparent: true, alphaTest: 0.5, side: THREE.DoubleSide }));
        mesh.userData = { task: s.addr, owner: s.owner };
        const shadow = new THREE.Mesh(shadowGeo, new THREE.MeshBasicMaterial({ color: 0x000000, transparent: true, opacity: 0.32, depthWrite: false }));
        shadow.rotation.x = -Math.PI / 2;
        scene.add(mesh, shadow);
        e = { mesh, shadow, canvas: c, tex };
        sprites.set(s.addr, e);
      }
      const [bx, by, bw, bh] = s.box, [ax, ay] = s.anchor;
      const pts = [corner(s.world, bx - ax, by - ay), corner(s.world, bx + bw - ax, by - ay),
        corner(s.world, bx + bw - ax, by + bh - ay), corner(s.world, bx - ax, by + bh - ay)];
      const pos = e.mesh.geometry.attributes.position;
      pts.forEach((p, i) => pos.setXYZ(i, p[0], p[1], p[2]));
      pos.needsUpdate = true;
      e.mesh.geometry.computeBoundingSphere();
      if (layer) {
        const img = cropLayer(layer, s.box);
        if (e.canvas.width !== img.width || e.canvas.height !== img.height) {
          e.canvas.width = img.width; e.canvas.height = img.height;
          e.tex.dispose();
        }
        e.canvas.getContext('2d').putImageData(img, 0, 0);
        e.tex.needsUpdate = true;
      }
      e.mesh.material.color.setScalar(selected ? 1 : 0.78);
      return e;
    }

    function update(sc, info, layer) {
      if (!sc) return;
      const groundY = (sc.player ? sc.player[1] : info.world ? info.world[1] : 0);
      const seen = new Set();
      selectedMesh = null;
      tag.textContent = pretty(info.owner && info.owner.update);
      for (const s of sc.sprites) {
        seen.add(s.addr);
        const isSel = Number(s.addr) === Number(info.addr);
        const e = spriteMesh(s, layer, isSel);
        if (isSel) selectedMesh = e.mesh;
        const r = Math.max(0.6, s.box[2] * 16 / 3 * SCALE * 0.42);
        e.shadow.scale.set(r, r * 0.55, 1);
        e.shadow.position.set(s.world[0] * SCALE, groundY * SCALE + 0.03, s.world[2] * SCALE);
      }
      for (const [addr, e] of sprites) {
        if (seen.has(addr)) continue;
        scene.remove(e.mesh, e.shadow);
        e.mesh.geometry.dispose(); e.mesh.material.dispose(); e.tex.dispose(); e.shadow.material.dispose();
        sprites.delete(addr);
      }
      // floor picture: the canvas, with sprite pixels faded (they stand up in 3D)
      if (sc.camera && env.canvas) {
        const fx = floorCanvas.getContext('2d');
        fx.clearRect(0, 0, 240, 160);
        fx.drawImage(env.canvas, 0, 0);
        if (layer) {
          const img = fx.getImageData(0, 0, 240, 160);
          for (let i = 3; i < layer.length; i += 4) if (layer[i]) img.data[i] = 40;
          fx.putImageData(img, 0, 0);
        }
        floorTex.needsUpdate = true;
        const [cx, cy] = sc.camera;
        const un = (sx, sy) => {
          const a = sx - 120 + cx, b = sy - 80 + cy + proj(groundY);
          return [(a + b) * 16 / 3 * SCALE, groundY * SCALE, (b - a) * 16 / 3 * SCALE];
        };
        const pos = floorGeo.attributes.position;
        [un(0, 0), un(240, 0), un(240, 160), un(0, 160)].forEach((p, i) => pos.setXYZ(i, p[0], p[1], p[2]));
        pos.needsUpdate = true;
        floorGeo.computeBoundingSphere();
      }
      grid.position.y = groundY * SCALE - 0.02;
      if (info.world) {
        want.set(info.world[0] * SCALE, info.world[1] * SCALE, info.world[2] * SCALE);
        if (!haveTarget) { target.copy(want); haveTarget = true; }
        ring.visible = stem.visible = true;
        ring.position.set(want.x, groundY * SCALE + 0.05, want.z);
        const r = Math.max(1, (info.box ? info.box[2] : 16) * 16 / 3 * SCALE * 0.55);
        ring.scale.set(r, r, 1);
        stem.position.set(want.x, groundY * SCALE, want.z);
        stem.scale.y = Math.max(0.01, (info.world[1] - groundY) * SCALE);
        stem.computeLineDistances();
      } else {
        ring.visible = stem.visible = false;
      }
    }

    function resize() {
      const w = box.clientWidth, h = box.clientHeight;
      if (!w || !h) return;
      renderer.setSize(w, h, false);
      const half = 70 / orbit.zoom;
      camera.left = -half * w / h; camera.right = half * w / h; camera.top = half; camera.bottom = -half;
      camera.updateProjectionMatrix();
    }
    function frame(now) {
      if (!running) return;
      raf = requestAnimationFrame(frame);
      target.lerp(want, 0.18);
      let yaw = orbit.yaw;
      if (orbit.spin) yaw = GAME.yaw + Math.sin((now - orbit.t0) / 2600) * 0.9;
      const r = 400;
      camera.position.set(target.x + r * Math.cos(orbit.pitch) * Math.sin(yaw), target.y + r * Math.sin(orbit.pitch),
        target.z + r * Math.cos(orbit.pitch) * Math.cos(yaw));
      camera.lookAt(target);
      ring.material.opacity = 0.55 + 0.35 * Math.sin(now / 280);
      resize();
      renderer.render(scene, camera);
      if (selectedMesh) {
        selectedMesh.geometry.computeBoundingBox();
        const bb = selectedMesh.geometry.boundingBox;
        const top = new THREE.Vector3((bb.min.x + bb.max.x) / 2, bb.max.y, (bb.min.z + bb.max.z) / 2).project(camera);
        tag.style.left = ((top.x + 1) / 2 * box.clientWidth) + 'px';
        tag.style.top = ((1 - top.y) / 2 * box.clientHeight) + 'px';
        tag.hidden = false;
      } else tag.hidden = true;
    }
    // drag to orbit, wheel to zoom
    let drag = null;
    canvas.addEventListener('pointerdown', (e) => { drag = [e.clientX, e.clientY, orbit.yaw, orbit.pitch]; canvas.setPointerCapture(e.pointerId); });
    canvas.addEventListener('pointermove', (e) => {
      if (!drag) return;
      if (Math.hypot(e.clientX - drag[0], e.clientY - drag[1]) > 3) orbit.spin = false;
      orbit.yaw = drag[2] - (e.clientX - drag[0]) * 0.008;
      orbit.pitch = Math.max(0.05, Math.min(1.5, drag[3] + (e.clientY - drag[1]) * 0.006));
    });
    // a click (not a drag) on a billboard follows that actor instead
    const ray = new THREE.Raycaster(), ndc = new THREE.Vector2();
    canvas.addEventListener('pointerup', (e) => {
      const d = drag;
      drag = null;
      if (!d || Math.hypot(e.clientX - d[0], e.clientY - d[1]) > 3) return;
      const r = canvas.getBoundingClientRect();
      ndc.set((e.clientX - r.left) / r.width * 2 - 1, -((e.clientY - r.top) / r.height) * 2 + 1);
      ray.setFromCamera(ndc, camera);
      const hit = ray.intersectObjects([...sprites.values()].map((x) => x.mesh))[0];
      if (hit && Number(hit.object.userData.task) !== sel) open({ task: hit.object.userData.task, owner: hit.object.userData.owner });
    });
    canvas.addEventListener('wheel', (e) => { e.preventDefault(); orbit.zoom = Math.max(0.3, Math.min(6, orbit.zoom * Math.exp(-e.deltaY * 0.0015))); }, { passive: false });
    raf = requestAnimationFrame(frame);
    return {
      update,
      reset() { orbit.yaw = GAME.yaw; orbit.pitch = GAME.pitch; orbit.zoom = 2.4; orbit.spin = false; },
      turntable() { orbit.spin = !orbit.spin; orbit.t0 = performance.now(); },
      pause() { running = false; cancelAnimationFrame(raf); },
      resume() { if (!running) { running = true; haveTarget = false; raf = requestAnimationFrame(frame); } },
    };
  }

  document.addEventListener('keydown', (e) => {
    if (e.key === 'Escape' && isOpen() && !(e.target.closest && e.target.closest('input, textarea, select'))) close();
  });

  window.ActorPanel = { init, open, close, isOpen, onMessage };
})();
