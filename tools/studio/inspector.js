// Screen inspector shared by the Captures and Live pages: a capture's
// screenshot with sprite and text outlines, sprites (grouped, or one per OAM
// entry) drawn from its VRAM with their resource, frame and palette, the text
// on screen, BG layers and palette RAM. Needs common.js.
'use strict';

const INSPECTOR_CSS = `
.insp .top { display: flex; gap: 18px; flex-wrap: wrap; align-items: flex-start; }
.insp .top > .scr { flex: 1 1 480px; min-width: 0; max-width: 720px; }
.insp .screen { position: relative; width: 100%; aspect-ratio: 3/2; background: var(--stage); border-radius: 6px; overflow: hidden; flex: none; }
.insp .screen img { width: 100%; height: 100%; image-rendering: pixelated; display: block; }
.insp .screen .ob { position: absolute; border: 1px solid rgba(255, 211, 77, .6); cursor: pointer; }
.insp .screen.nobox .ob { border-color: transparent; }
.insp .screen .ob:hover, .insp .screen .ob.hl { border: 2px solid #ffd34d; background: rgba(255, 211, 77, .18); z-index: 2; }
.insp .screen .ob.sel { border: 2px solid var(--accent); background: rgba(224, 116, 90, .22); z-index: 3; }
.insp .screen .tx { position: absolute; border: 1px dashed rgba(120, 220, 255, .9); cursor: pointer; z-index: 1; }
.insp .screen.notext .tx { display: none; }
.insp .screen .tx:hover, .insp .screen .tx.hl { background: rgba(120, 220, 255, .2); }
.insp .bar { display: flex; gap: 12px; flex-wrap: wrap; align-items: center; font-size: 13px; color: var(--muted); margin: 6px 0; }
.insp .seg { display: inline-flex; border: 1px solid var(--line); border-radius: 7px; overflow: hidden; }
.insp .seg button { border: 0; border-radius: 0; padding: 3px 10px; font-size: 12px; }
.insp .seg button.on { background: var(--sel); color: var(--ink); font-weight: 600; }
.insp .detail { flex: 1; min-width: 260px; }
.insp .big { background: var(--stage); border-radius: 6px; padding: 10px; display: inline-block; }
.insp .big canvas { image-rendering: pixelated; }
.insp .tabs { display: flex; gap: 4px; border-bottom: 1px solid var(--line); margin: 14px 0 10px; }
.insp .tabs button { border: 0; border-bottom: 2px solid transparent; border-radius: 0; background: none; padding: 6px 12px; color: var(--muted); }
.insp .tabs button.on { border-bottom-color: var(--accent); color: var(--ink); }
.insp table { border-collapse: collapse; font-size: 12px; width: 100%; }
.insp td, .insp th { text-align: left; padding: 3px 6px; border-bottom: 1px solid var(--line); vertical-align: middle; }
.insp th { color: var(--muted); font-weight: 500; }
.insp tr.sp { cursor: pointer; }
.insp tr.sp:hover, .insp tr.sp.hl { background: var(--sel); }
.insp tr.sp.sel { background: var(--sel); box-shadow: inset 3px 0 var(--accent); }
.insp td.img { background: var(--stage); width: 72px; text-align: center; }
.insp td.img canvas { image-rendering: pixelated; display: inline-block; }
.insp .sw { display: inline-flex; }
.insp .sw i { width: 7px; height: 12px; display: block; }
.insp .pal { display: grid; grid-template-columns: repeat(16, 14px); gap: 1px; }
.insp .pal i { width: 14px; height: 14px; display: block; }
.insp .layers { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 12px; }
.insp .layer { border: 1px solid var(--line); border-radius: 8px; padding: 8px; background: var(--panel); }
.insp .layer .cv { background: var(--stage); overflow: auto; max-height: 520px; border-radius: 4px; }
.insp .layer canvas { image-rendering: pixelated; }
.insp .txt { padding: 6px 0; border-bottom: 1px solid var(--line); cursor: pointer; }
.insp .txt:hover, .insp .txt.hl { background: var(--sel); }
.insp .txt .en { font-size: 15px; white-space: pre-wrap; }
.insp .txt .jp { font-size: 13px; color: var(--muted); white-space: pre-wrap; }
.insp .lab { color: var(--accent); }
.insp .empty, .insp .meta { color: var(--muted); font-size: 13px; }
#itip { position: fixed; pointer-events: none; background: var(--panel); border: 1px solid var(--line); border-radius: 6px; padding: 5px 8px; font-size: 12px; display: none; box-shadow: 0 4px 16px rgba(0,0,0,.2); z-index: 60; max-width: 340px; }
`;
document.head.insertAdjacentHTML('beforeend', `<style>${INSPECTOR_CSS}</style>`);

let _display = null;
const displayStrings = () => (_display ??= fetch('/api/display').then(r => r.json()).catch(() => ({ jp: {}, en: {} })));

// Sprite groups: OAM entries drawing the same resource whose boxes touch or
// nearly touch (a
// frame's parts are separate OAM entries; matched one by one they can name
// different frames, so the group's frame is settled afterwards).
function groupSprites(ents) {
  const key = e => e.an?.match ? e.an.match.join(':') : null;
  const parent = ents.map((_, i) => i);
  const find = i => parent[i] === i ? i : (parent[i] = find(parent[i]));
  const G = 8;  // pieces of one frame can sit a tile apart in a row (a button icon and its label)
  const touch = (a, b) => a.x <= b.x + b.w + G && b.x <= a.x + a.w + G && a.y <= b.y + b.h && b.y <= a.y + a.h;
  for (let i = 0; i < ents.length; i++) for (let j = i + 1; j < ents.length; j++) {
    const k = key(ents[i]);
    if (k && k === key(ents[j]) && touch(ents[i], ents[j])) parent[find(j)] = find(i);
  }
  const groups = new Map();
  ents.forEach((e, i) => { const r = find(i); if (!groups.has(r)) groups.set(r, []); groups.get(r).push(e); });
  return [...groups.values()].map((es, n) => {
    const x0 = Math.min(...es.map(e => e.x)), y0 = Math.min(...es.map(e => e.y));
    const x1 = Math.max(...es.map(e => e.x + e.w)), y1 = Math.max(...es.map(e => e.y + e.h));
    return { id: n, ents: es, x: x0, y: y0, w: x1 - x0, h: y1 - y0, an: es[0].an && { ...es[0].an }, bank: es[0].bank, bpp8: es[0].bpp8 };
  });
}
// A package frame for a group: the one whose parts use most of the group's tiles.
async function settleFrames(groups) {
  for (const g of groups) {
    const a = g.an;
    if (!a?.match || a.match[0] !== 'spr' || new Set(g.ents.map(e => e.an?.frame)).size < 2) continue;
    const got = new Set(g.ents.flatMap(e => e.an?.nums || []));
    const pkg = parsePackage(await res('tCB05', a.match[1]));
    let best = a.frame, score = -1;
    pkg.frames.forEach((f, i) => {
      const used = new Set(pkg.parts(f).flatMap(p => { const t0 = (p.tileOff(0, 0) - u32(p.src, 0x1C)) / 32, out = []; for (let ty = 0; ty < p.h / 8; ty++) for (let tx = 0; tx < p.w / 8; tx++) out.push(t0 + ty * 16 + tx); return out; }));
      let s = 0; for (const t of got) if (used.has(t)) s++;
      if (s > score) { score = s; best = i; }
    });
    a.frame = best;
  }
}

class Inspector {
  // opts: { onText(id) } called when a string is clicked (default: peek the editor)
  constructor(el, opts = {}) {
    this.el = el; this.opts = opts; this.mode = localStore('insp.mode') || 'sprite'; this.tab = 'sprites'; this.sel = new Set();
    if (!$('itip')) document.body.insertAdjacentHTML('beforeend', '<div id="itip"></div>');
  }

  async load(name) {
    this.name = name;
    const [an, cap, disp] = await Promise.all([fetch(`/api/live/analysis/${encodeURIComponent(name)}`).then(r => r.json()), loadCapture(name), displayStrings()]);
    this.an = an; this.cap = cap; this.disp = disp;
    this.ents = parseOam(cap.oam);
    const byOam = Object.fromEntries((an.sprites || []).map(s => [s.oam, s]));
    this.ents.forEach(e => e.an = byOam[e.oam]);
    this.groups = groupSprites(this.ents);
    await settleFrames(this.groups);
    this.texts = (an.meta?.onscreen || []).map(t => ({ ...t, en: disp.en[t.id], jp: disp.jp[t.id] }));
    if (!this.texts.length && an.meta?.text != null) this.texts = [{ id: an.meta.text, en: disp.en[an.meta.text], jp: disp.jp[an.meta.text], last: true }];
    this.sel = new Set([...this.sel].filter(id => this.items().some(it => it.id === id)));
    this.render();
  }

  items() { return this.mode === 'oam' ? this.ents.map(e => ({ ...e, id: e.oam, ents: [e] })) : this.groups; }

  render() {
    const t = Date.now(), m = this.an.meta || {}, nMatched = this.ents.filter(e => e.an?.match).length;
    this.el.classList.add('insp');
    this.el.innerHTML = `
      <div class="top">
        <div class="scr">
          <div class="screen${localStore('insp.boxes') === '0' ? ' nobox' : ''}${localStore('insp.text') === '0' ? ' notext' : ''}"><img alt="" src="/api/live/file/captures/${encodeURIComponent(this.name)}/screen.png?t=${t}"></div>
          <div class="bar">
            <span class="seg"><button data-m="sprite">Sprites</button><button data-m="oam">OAM entries</button></span>
            <label title="Outline every sprite on the screen"><input type="checkbox" class="cbx" ${localStore('insp.boxes') === '0' ? '' : 'checked'}> outline sprites</label>
            <label title="Outline the text the game drew"><input type="checkbox" class="ctx" ${localStore('insp.text') === '0' ? '' : 'checked'}> outline text</label>
            <span>${this.ents.length} OAM entries in ${this.groups.length} sprites, ${nMatched} matched · shift-click to select several</span>
          </div>
        </div>
        <div class="detail"></div>
      </div>
      <div class="tabs"><button data-t="sprites">Sprites</button><button data-t="text">Text (${this.texts.filter(x => !x.last).length})</button><button data-t="layers">BG layers</button><button data-t="palettes">Palettes</button></div>
      <div class="tab"></div>`;
    const q = s => this.el.querySelector(s);
    this.el.querySelectorAll('.seg button').forEach(b => { b.classList.toggle('on', b.dataset.m === this.mode); b.onclick = () => { this.mode = b.dataset.m; localStore('insp.mode', this.mode); this.sel.clear(); this.render(); }; });
    q('.cbx').onchange = e => { localStore('insp.boxes', e.target.checked ? '1' : '0'); q('.screen').classList.toggle('nobox', !e.target.checked); };
    q('.ctx').onchange = e => { localStore('insp.text', e.target.checked ? '1' : '0'); q('.screen').classList.toggle('notext', !e.target.checked); };
    this.el.querySelectorAll('.tabs button').forEach(b => { b.classList.toggle('on', b.dataset.t === this.tab); b.onclick = () => { this.tab = b.dataset.t; this.el.querySelectorAll('.tabs button').forEach(x => x.classList.toggle('on', x === b)); this.renderTab(); }; });
    this.drawBoxes();
    this.renderDetail();
    this.renderTab();
  }

  // ---- helpers ----
  poolOf(bank) { const p = this.cap.objPool?.[bank]; return p != null && p < 0x8000 ? p : null; }
  poolLink(bank) { const p = this.poolOf(bank); return p != null ? peekLink('/gfx#pool/' + p, '#' + p) : (this.cap.objPool?.length ? 'special' : ''); }
  name_(it) { const a = it.an; return a?.match ? `${a.match[0] === 'big' ? 'large sprite' : 'sprite'} ${a.match[1]}${a.frame != null ? ' frame ' + a.frame : ''}` : 'unidentified'; }
  label(it) { return it.an?.match && noteOf(it.an.match[0], it.an.match[1])?.label; }
  image(ents) {  // entries drawn together, back to front (lower OAM index is in front)
    const x0 = Math.min(...ents.map(e => e.x)), y0 = Math.min(...ents.map(e => e.y));
    const w = Math.max(...ents.map(e => e.x + e.w)) - x0, h = Math.max(...ents.map(e => e.y + e.h)) - y0;
    const cv = document.createElement('canvas'); cv.width = w; cv.height = h;
    const ctx = cv.getContext('2d');
    for (const e of [...ents].sort((a, b) => b.oam - a.oam)) ctx.drawImage(toCanvas(oamImage(this.cap, e)), e.x - x0, e.y - y0);
    return cv;
  }
  hl(id, on) { this.el.querySelectorAll(`[data-id="${id}"]`).forEach(x => x.classList.toggle('hl', on)); }
  markSel() { this.el.querySelectorAll('[data-id]').forEach(x => x.classList.toggle('sel', this.sel.has(+x.dataset.id))); }
  pick(id, ev) {
    if (ev?.shiftKey) { this.sel.has(id) ? this.sel.delete(id) : this.sel.add(id); }
    else this.sel = new Set([id]);
    this.markSel(); this.renderDetail();
  }
  tip(ev, html) { const t = $('itip'); t.innerHTML = html; t.style.display = 'block'; t.style.left = ev.clientX + 14 + 'px'; t.style.top = ev.clientY + 14 + 'px'; }
  untip() { $('itip').style.display = 'none'; }
  openText(id) { (this.opts.onText || (i => peek('/#' + i)))(id); }

  drawBoxes() {
    const sc = this.el.querySelector('.screen');
    const box = (cls, x, y, w, h) => { const b = document.createElement('div'); b.className = cls; Object.assign(b.style, { left: x / 2.4 + '%', top: y / 1.6 + '%', width: w / 2.4 + '%', height: h / 1.6 + '%' }); return b; };
    this.texts.filter(t => t.x0 != null).forEach((t, i) => {
      const b = box('tx', t.x0 * 8, t.y0 * 8, (t.x1 - t.x0 + 1) * 8, (t.y1 - t.y0 + 1) * 8);
      b.dataset.tid = i;
      b.onmousemove = ev => this.tip(ev, `<b>#${t.id}</b> ${esc(t.en || t.jp || '')}`);
      b.onmouseleave = () => this.untip();
      b.onclick = () => this.openText(t.id);
      sc.appendChild(b);
    });
    for (const it of [...this.items()].reverse()) {
      const b = box('ob', it.x, it.y, it.w, it.h);
      b.dataset.id = it.id;
      b.onmouseenter = () => this.hl(it.id, true);
      b.onmousemove = ev => { const lab = this.label(it); this.tip(ev, `<b>${this.mode === 'oam' ? 'OAM ' + it.id : 'sprite'}</b> · ${it.w}×${it.h} at (${it.x}, ${it.y})${it.ents.length > 1 ? ` · ${it.ents.length} OAM entries` : ''}<br>${esc(this.name_(it))}${lab ? `<br><span class="lab">${esc(lab)}</span>` : ''}`); };
      b.onmouseleave = () => { this.hl(it.id, false); this.untip(); };
      b.onclick = ev => this.pick(it.id, ev);
      sc.appendChild(b);
    }
    this.markSel();
  }

  renderDetail() {
    const d = this.el.querySelector('.detail'), items = this.items().filter(it => this.sel.has(it.id));
    if (!items.length) { d.innerHTML = '<div class="empty">Click a sprite on the screen or in the list; shift-click to add more.</div>'; return; }
    const ents = items.flatMap(it => it.ents), first = items[0], mt = first.an?.match;
    const cv = this.image(ents);
    cv.style.width = cv.width * Math.max(2, Math.floor(180 / Math.max(cv.width, cv.height))) + 'px';
    const res = [...new Map(items.filter(it => it.an?.match).map(it => [this.name_(it), it])).values()];
    d.innerHTML = `<h3 style="margin:0 0 4px;font-size:15px">${items.length > 1 ? `${items.length} sprites` : this.mode === 'oam' ? 'OAM ' + first.id : 'Sprite'} · ${ents.length} OAM entr${ents.length > 1 ? 'ies' : 'y'}</h3>
      <div class="meta">${items.length === 1 ? `at (${first.x}, ${first.y}) · ${first.w}×${first.h} · ` : ''}OBJ palette ${[...new Set(ents.map(e => e.bpp8 ? '256-color' : 'bank ' + e.bank + (this.poolOf(e.bank) != null ? ' = pool ' + this.poolLink(e.bank) : '')))].join(', ')}</div>
      <div class="big" style="margin:8px 0"></div>
      <div>${res.length ? res.map(it => `From ${peekLink(gfxHref(it.an.match[0], it.an.match[1], it.an.frame), esc(this.name_(it)))}`).join('<br>') : 'No resource found with these tiles (generated at runtime, compressed, or a format not indexed yet).'}</div>
      <div class="dnote"></div><div class="dtag"></div>`;
    d.querySelector('.big').appendChild(cv);
    if (mt) {
      d.querySelector('.dnote').appendChild(noteEditor(mt[0], mt[1], () => this.renderTab()));
      const b = document.createElement('button'); b.textContent = 'Has text…';
      b.onclick = async () => {
        const note = prompt(`What text does ${mt[0]} ${mt[1]} show? (Japanese or a description)`);
        if (note === null) return;
        await post('/api/tags', { kind: mt[0], hash: mt[1], part: first.an.frame ?? '', note });
        b.textContent = 'Tagged: ' + note;
      };
      d.querySelector('.dtag').appendChild(b);
    }
  }

  renderTab() {
    const t = this.el.querySelector('.tab'), cap = this.cap;
    if (this.tab === 'sprites') {
      const items = this.items();
      t.innerHTML = items.length ? `<table><tr><th></th><th>${this.mode === 'oam' ? 'OAM' : 'OAM entries'}</th><th>x, y</th><th>Size</th><th>Resource</th><th>Label</th><th>Palette (OBJ bank)</th><th>Pool</th></tr>${items.map(it => {
        const mt = it.an?.match, lab = this.label(it);
        return `<tr class="sp" data-id="${it.id}"><td class="img"></td><td>${it.ents.map(e => e.oam).join(', ')}</td><td>${it.x}, ${it.y}</td><td>${it.w}×${it.h}</td>
          <td>${mt ? peekLink(gfxHref(mt[0], mt[1], it.an.frame), esc(this.name_(it))) : '<span class="empty">unidentified</span>'}</td><td>${lab ? `<span class="lab">${esc(lab)}</span>` : ''}</td>
          <td><span class="sw">${Array.from({ length: 16 }, (_, k) => `<i style="background:${cssRgb(le(cap.pal, 0x200 + (it.bank * 16 + k) * 2))}"></i>`).join('')}</span> ${it.bpp8 ? '256' : it.bank}</td><td>${it.bpp8 ? '' : this.poolLink(it.bank)}</td></tr>`;
      }).join('')}</table>` : '<div class="empty">No sprites on screen.</div>';
      t.querySelectorAll('tr.sp').forEach(r => {
        const it = items.find(x => x.id === +r.dataset.id);
        r.querySelector('.img').appendChild(fitCanvas(this.image(it.ents), 64));
        r.onmouseenter = () => this.hl(it.id, true); r.onmouseleave = () => this.hl(it.id, false);
        r.onclick = ev => { if (!ev.target.closest('a')) this.pick(it.id, ev); };
      });
      this.markSel();
    } else if (this.tab === 'text') {
      t.innerHTML = this.texts.length ? this.texts.map((x, i) => `<div class="txt" data-tid="${i}"><div class="en">${esc(x.en || '(no translation)')}</div><div class="jp">${esc(x.jp || '')}</div>
        <div class="meta">string #${x.id}${x.x0 != null ? ` · tiles ${x.x0},${x.y0}–${x.x1},${x.y1}` : x.last ? ' · last fetched (this capture predates on-screen tracking)' : ''}</div></div>`).join('') : '<div class="empty">No text on screen.</div>';
      t.querySelectorAll('.txt').forEach(r => {
        const x = this.texts[+r.dataset.tid];
        r.onclick = () => this.openText(x.id);
        r.onmouseenter = () => this.el.querySelector(`.tx[data-tid="${r.dataset.tid}"]`)?.classList.add('hl');
        r.onmouseleave = () => this.el.querySelector(`.tx[data-tid="${r.dataset.tid}"]`)?.classList.remove('hl');
      });
    } else if (this.tab === 'layers') {
      const disp = le(cap.io, 0), mode = disp & 7, an = this.an;
      t.innerHTML = `<div class="meta" style="margin-bottom:8px">Video mode ${mode} · whole BG maps from VRAM, unscrolled (the capture has no scroll registers) · tiles in palette RAM colors${an.stage_map ? ` · stage map ${peekLink(gfxHref('map', an.stage_map), an.stage_map)}` : ''}</div><div class="layers"></div>`;
      for (let n = 0; n < 4; n++) {
        const on = !!(disp & (0x100 << n)), d = document.createElement('div'); d.className = 'layer';
        if (mode !== 0 && !(mode === 1 && n < 2)) { d.innerHTML = `<b>BG${n}</b> <span class="meta">${on ? 'affine/bitmap layer, not drawn here' : 'off'}</span>`; t.querySelector('.layers').appendChild(d); continue; }
        const L = bgLayer(cap, n);
        d.innerHTML = `<b>BG${n}</b> <span class="meta">${on ? 'shown' : 'off'} · priority ${L.prio} · ${L.W}×${L.H} tiles · chars ${L.cb / 0x4000} · map block ${L.sb / 0x800}${L.bpp8 ? ' · 256 colors' : ''}</span><div class="cv"></div>`;
        d.querySelector('.cv').appendChild(toCanvas(L.img, 1));
        if (!on) d.style.opacity = .55;
        t.querySelector('.layers').appendChild(d);
      }
      if (an.bg_best) t.insertAdjacentHTML('beforeend', `<p class="meta">BG palette RAM matches palette resource ${peekLink(gfxHref('pal', an.bg_best), an.bg_best)} in banks ${an.bg.filter(b => b.resources.includes(an.bg_best)).map(b => b.bank).join(', ')}.</p>`);
    } else {
      const banks = off => Array.from({ length: 16 }, (_, b) => `<div class="pal" style="margin-bottom:2px" title="bank ${b}">${Array.from({ length: 16 }, (_, k) => `<i style="background:${cssRgb(le(cap.pal, off + (b * 16 + k) * 2))}"></i>`).join('')}</div>`).join('');
      t.innerHTML = `<div class="top"><div><h3 class="meta">BG palette RAM</h3>${banks(0)}</div><div><h3 class="meta">OBJ palette RAM</h3>${banks(0x200)}</div>
        <div><h3 class="meta">OBJ bank → pool entry</h3>${Array.from({ length: 16 }, (_, b) => `<div class="meta" style="height:16px">bank ${b}: ${this.poolLink(b) || '—'}</div>`).join('')}</div>
        <div><h3 class="meta">BG banks → palette resources</h3>${(this.an.bg || []).map(b => `<div class="meta">bank ${b.bank}: ${b.resources.length ? b.resources.map(h => peekLink(gfxHref('pal', h), h)).join(', ') : 'no resource'}</div>`).join('') || '<div class="empty">none</div>'}</div></div>`;
    }
  }
}
