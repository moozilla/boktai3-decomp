// Actor panel: Inspect (I), then click a sprite in the game. A panel docks on
// the right with the actor's own struct (typed by its task's update function,
// editable live), its sprite, its world position over time, and a 3D diorama
// of the scene: every projected sprite stands at its world position as a
// billboard of its real pixels, over the screen projected onto the floor.
// The worker side is port/web/actor.mjs.
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

  let env = null, panel = null, sel = null, last = null, refs = {}, history = [], view = null;
  let showPad = false, filter = '', structureKey = '', valueEls = new Map(), prevValues = new Map();

  // "TerrainEdgeMarker_UpdateVisibility" -> "Terrain Edge Marker"
  function pretty(update) {
    if (!update || update === '?') return 'Sprite';
    const head = update.split('_')[0];
    return head.replace(/([a-z0-9])([A-Z])/g, '$1 $2').replace(/([A-Z]+)([A-Z][a-z])/g, '$1 $2');
  }

  function init(e) { env = e; }

  function open(hit) {
    if (!hit.task) { env.flash('No task owns this sprite'); return; }
    sel = Number(hit.task);
    last = null; history = []; structureKey = ''; prevValues.clear();
    build();
    const settings = $('settings');
    if (settings && settings.open) settings.close();
    panel.hidden = false;
    document.body.classList.add('actor-open');
    refs.title.textContent = pretty(hit.owner);
    refs.sub.textContent = 'loading…';
    env.request({ op: 'actor', task: sel });
    start3d();
  }
  function close() {
    if (!panel || panel.hidden) return;
    panel.hidden = true;
    document.body.classList.remove('actor-open');
    if (sel) env.request({ op: 'actor', task: 0 });
    sel = null;
    stop3d();
  }
  function isOpen() { return !!(panel && !panel.hidden); }

  // ---- layout ----------------------------------------------------------------
  function build() {
    if (panel) return;
    refs.thumb = el('canvas', { className: 'ap-thumb', width: 64, height: 64 });
    refs.title = el('strong', { className: 'ap-title' });
    refs.sub = el('div', { className: 'ap-sub' });
    refs.badge = el('span', { className: 'ap-badge' });
    const closeBtn = el('button', { className: 'ap-close', title: 'Close (Esc)', textContent: '×', onclick: close });
    refs.view = el('div', { className: 'ap-view' });
    refs.gl = el('canvas', { className: 'ap-gl' });
    refs.viewNote = el('div', { className: 'ap-view-note' });
    const angle = el('button', { textContent: 'Game angle', title: 'Look from where the game\'s camera looks', onclick: () => view && view.reset() });
    const spin = el('button', { textContent: 'Turntable', title: 'Swing slowly around the actor', onclick: () => view && view.turntable() });
    refs.view.append(refs.gl, refs.viewNote, el('div', { className: 'ap-view-tools' }, angle, spin),
      el('div', { className: 'ap-view-hint', textContent: 'drag to orbit · scroll to zoom' }));
    refs.stats = el('div', { className: 'ap-stats' });
    refs.coord = {};
    for (const axis of ['x', 'y', 'z']) {
      const value = el('span', { className: 'ap-num' });
      const spark = el('canvas', { className: 'ap-spark', width: 120, height: 28 });
      refs.coord[axis] = { value, spark };
      refs.stats.append(el('div', { className: 'ap-coord' }, el('span', { className: 'ap-axis', textContent: axis }), value, spark));
    }
    refs.meta = el('div', { className: 'ap-meta' });
    refs.actions = el('div', { className: 'ap-actions' },
      el('button', { textContent: 'Move player here', onclick: movePlayerHere }),
      refs.bring = el('button', { textContent: 'Bring to player', onclick: bringToPlayer }),
      el('button', { textContent: 'Copy JS', title: 'A typed view for the console or port/agent/play.py js', onclick: copyJs }));
    refs.filter = el('input', { className: 'ap-filter', placeholder: 'Filter fields', type: 'search' });
    refs.filter.oninput = () => { filter = refs.filter.value.trim().toLowerCase(); applyFilter(); };
    const pad = el('label', { className: 'ap-pad' }, el('input', { type: 'checkbox', onchange: (e) => { showPad = e.target.checked; structureKey = ''; render(); } }), ' padding');
    refs.fields = el('div', { className: 'ap-fields' });
    panel = el('aside', { id: 'actor-panel', hidden: true },
      el('header', { className: 'ap-head' }, refs.thumb, el('div', { className: 'ap-titles' }, el('div', {}, refs.title, refs.badge), refs.sub), closeBtn),
      refs.view, refs.stats, refs.meta, refs.actions,
      el('div', { className: 'ap-fields-head' }, refs.filter, pad), refs.fields);
    panel.addEventListener('keydown', (e) => { if (e.key === 'Escape' && !e.target.closest('input')) close(); });
    document.body.append(panel);
  }

  // ---- updates from the worker ------------------------------------------------
  function onMessage(m) {
    if (!sel || !panel) return;
    if (m.error) { refs.sub.textContent = m.error.split('\n')[0]; return; }
    last = m;
    const info = m.info;
    const own = info.owner;
    refs.title.textContent = pretty(own && own.update);
    refs.sub.textContent = own
      ? `${own.type || 'untyped'} · task ${hex(own.task)}${info.size ? ' · ' + info.size.toLocaleString() + ' bytes' : ''} · ${own.update}`
      : `task ${hex(sel)} · ended`;
    refs.badge.textContent = info.alive ? (info.hidden ? 'hidden' : 'live') : 'gone';
    refs.badge.className = 'ap-badge ' + (info.alive ? (info.hidden ? 'dim' : 'live') : 'gone');
    if (info.world) {
      history.push(info.world.slice());
      if (history.length > 150) history.shift();
    }
    ['x', 'y', 'z'].forEach((axis, i) => {
      refs.coord[axis].value.textContent = info.world ? info.world[i] : '—';
      spark(refs.coord[axis].spark, history.map((w) => w[i]));
    });
    refs.meta.textContent = (info.box ? `screen ${info.box[0]}, ${info.box[1]} · ${info.box[2]}×${info.box[3]} px · ${info.pieces} OAM ${info.pieces === 1 ? 'entry' : 'entries'}` : 'no sprites this frame') + ` · frame ${info.frame}`;
    refs.bring.hidden = !positionPath();
    const layer = m.layer ? new Uint8ClampedArray(m.layer) : null;
    if (layer && info.box) thumb(layer, info.box);
    render();
    if (view) view.update(m.scene, info, layer);
  }

  function spark(c, values) {
    const x = c.getContext('2d');
    const w = c.width, h = c.height;
    x.clearRect(0, 0, w, h);
    if (values.length < 2) return;
    let lo = Math.min(...values), hi = Math.max(...values);
    if (hi - lo < 8) { const mid = (hi + lo) / 2; lo = mid - 4; hi = mid + 4; }
    x.beginPath();
    values.forEach((v, i) => {
      const px = (i / (150 - 1)) * w, py = h - 3 - ((v - lo) / (hi - lo)) * (h - 6);
      if (i) x.lineTo(px, py); else x.moveTo(px, py);
    });
    x.strokeStyle = '#f2a33a'; x.lineWidth = 1.5; x.stroke();
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
    const t = refs.thumb.getContext('2d');
    t.clearRect(0, 0, 64, 64);
    t.imageSmoothingEnabled = false;
    const k = Math.max(1, Math.floor(60 / Math.max(img.width, img.height)));
    const w = img.width * k, h = img.height * k;
    t.drawImage(crop, (64 - w) >> 1, (64 - h) >> 1, w, h);
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
    const key = (typed ? 'T' : 'R') + nodesKey(nodes) + showPad;
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
  }
  function tree(container, nodes, path, depth, editable) {
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
      const row = el('div', { className: 'ap-row' }, label, type, value);
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
