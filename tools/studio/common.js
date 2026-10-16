// Shared by the studio pages: the header, the peek panel (open a graphic in a
// side panel without leaving the page), notes on anything (data/notes.csv) and
// the GBA graphics decoders (tiles, sprite packages, large sprites, maps).
'use strict';
const $ = id => document.getElementById(id);
const esc = s => String(s ?? '').replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));
const EMBED = new URLSearchParams(location.search).has('embed');
const post = (url, body) => fetch(url, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) }).then(r => r.json());
function localStore(k, v) { try { if (v === undefined) return localStorage.getItem('studio.' + k); localStorage.setItem('studio.' + k, v); } catch (e) { return null; } }

// ---------- header ----------
const NAV = [
  ['Text', '/'], ['Sprites', '/gfx#sprites'], ['Large sprites', '/gfx#big'], ['Maps', '/gfx#maps'], ['Palettes', '/gfx#palettes'],
  ['Sprite palettes', '/gfx#pool'], ['Font', '/gfx#font'], ['Tiles', '/gfx#tiles'], ['Data', '/gfx#data'], ['Actors', '/actors'], ['Scripts', '/scripts'], ['World', '/world'], ['Live', '/live'], ['Captures', '/captures'],
];
// Fills <header id="top">; `tools` is HTML for the right-hand side.
function studioHeader(tools = '') {
  const h = $('top');
  if (!h) return;
  if (EMBED) { h.remove(); document.body.classList.add('embed'); return; }
  const here = location.pathname === '/index.html' ? '/' : location.pathname;
  const tab = (location.hash.slice(1) || '').split('/')[0];
  h.innerHTML = `<h1>Boktai 3 Studio</h1><nav>${NAV.map(([n, u]) => {
    const [p, t] = u.split('#');
    const on = p === here && (!t || t === tab || (here === '/gfx' && !tab && t === 'sprites'));
    return `<a href="${u}"${t ? ` data-tab="${t}"` : ''} class="${on ? 'on' : ''}">${n}</a>`;
  }).join('')}</nav><span class="sp"></span>${tools}`;
}
function markNav() {
  const tab = (location.hash.slice(1) || 'sprites').split('/')[0];
  document.querySelectorAll('#top nav a[data-tab]').forEach(a => a.classList.toggle('on', location.pathname === '/gfx' && a.dataset.tab === tab));
}

// ---------- peek panel ----------
// peek('/gfx#maps/C5A7') opens the page in a panel on the right (embed mode).
function peek(url) {
  let p = $('peek');
  if (!p) {
    p = document.createElement('aside'); p.id = 'peek';
    p.innerHTML = `<div class="peekbar"><a id="peekopen" target="_blank" title="Open in a new tab">open ↗</a><span class="sp"></span><button id="peekclose" title="Close (Esc)">✕</button></div><iframe id="peekframe"></iframe>`;
    document.body.appendChild(p);
    $('peekclose').onclick = () => p.classList.remove('on');
    document.addEventListener('keydown', e => { if (e.key === 'Escape') p.classList.remove('on'); });
  }
  const [path, hash] = url.split('#');
  $('peekframe').src = `${path}${path.includes('?') ? '&' : '?'}embed=1${hash ? '#' + hash : ''}`;
  $('peekopen').href = url;
  p.classList.add('on');
}
// Links with data-peek open in the panel; ctrl/cmd-click still opens a tab.
document.addEventListener('click', e => {
  const a = e.target.closest && e.target.closest('a[data-peek]');
  if (!a || e.metaKey || e.ctrlKey || e.shiftKey) return;
  e.preventDefault(); e.stopPropagation();
  if (EMBED && window.parent !== window) { location.href = a.getAttribute('href') + ''; return; }
  peek(a.getAttribute('href'));
}, true);
const peekLink = (href, text) => `<a href="${href}" data-peek>${text}</a>`;
// A function address (8 hex digits, Thumb bit ignored): its disassembly and,
// if matched, its C (/code). A script procedure, a text string, an actor.
const codeHref = addr => '/code#' + ((parseInt(addr, 16) & ~1) >>> 0).toString(16).toUpperCase().padStart(8, '0');
const codeLink = (addr, text) => peekLink(codeHref(addr), esc(text || addr));
const procLink = (id, text) => peekLink('/scripts#' + id, esc(text || 'proc ' + id));
const stringLink = (id, text) => peekLink('/#' + id, esc(text || '#' + id));
const actorLink = (key, text) => peekLink('/actors#' + key, esc(text || key));
const gfxHref = (kind, hash, frame) => ({ spr: '/gfx#sprites/', big: '/gfx#big/', bigspr: '/gfx#big/', map: '/gfx#maps/', pal: '/gfx#palettes/' }[kind] || '/gfx#tiles/') + hash + (frame != null ? '/' + frame : '');

// ---------- notes (data/notes.csv: kind, key, label, notes) ----------
let NOTES_DB = {}, ITEM_FRAMES = {};
// Item, sword and accessory names on their menu icons (data/items.csv, from the
// game's text bank): 'HASH/FRAME' -> [{category, id, name}]
async function loadItems() {
  const rows = await fetch('/api/items').then(r => r.json()).catch(() => []);
  ITEM_FRAMES = {};
  for (const r of rows) if (r.icon) (ITEM_FRAMES[r.icon] ||= []).push({ ...r, name: r.en || r.jp });
  return ITEM_FRAMES;
}
const itemLabel = (hash, frame) => { const l = ITEM_FRAMES[`${hash}/${frame}`]; return l ? (l.length > 3 ? `${l[0].name} (+${l.length - 1})` : l.map(x => x.name).join(' / ')) : null; };
const noteKey = (kind, key) => `${kind}:${String(key).toUpperCase()}`;
async function loadNotes() {
  const [rows] = await Promise.all([fetch('/api/notes').then(r => r.json()).catch(() => []), loadItems()]);
  NOTES_DB = Object.fromEntries(rows.map(r => [noteKey(r.kind, r.key), r]));
  return NOTES_DB;
}
const noteOf = (kind, key) => NOTES_DB[noteKey(kind, key)];
async function saveNote(kind, key, label, notes) {
  const rows = await post('/api/notes', { kind, key: String(key).toUpperCase(), label, notes });
  NOTES_DB = Object.fromEntries(rows.map(r => [noteKey(r.kind, r.key), r]));
}
// A small label + notes editor for any asset; onsave(note) after saving.
function noteEditor(kind, key, onsave) {
  const n = noteOf(kind, key) || {};
  const d = document.createElement('div'); d.className = 'noteed';
  d.innerHTML = `<input class="nl" placeholder="Label (e.g. title screen)" value="${esc(n.label)}"><textarea class="nn" rows="1" placeholder="Notes">${esc(n.notes)}</textarea><span class="ns"></span>`;
  const save = async () => {
    const label = d.querySelector('.nl').value.trim(), notes = d.querySelector('.nn').value.trim();
    if (label === (n.label || '') && notes === (n.notes || '')) return;
    await saveNote(kind, key, label, notes); n.label = label; n.notes = notes;
    d.querySelector('.ns').textContent = 'saved'; setTimeout(() => d.querySelector('.ns').textContent = '', 1500);
    if (onsave) onsave(noteOf(kind, key));
  };
  d.querySelector('.nl').onchange = save; d.querySelector('.nn').onchange = save;
  d.querySelector('.nl').onkeydown = e => { if (e.key === 'Enter') e.target.blur(); };
  return d;
}

// ---------- graphics ----------
const SIZES = { 0: [[8, 8], [16, 16], [32, 32], [64, 64]], 1: [[16, 8], [32, 8], [32, 16], [64, 32]], 2: [[8, 16], [8, 32], [16, 32], [32, 64]], 3: [[8, 8], [8, 8], [8, 8], [8, 8]] };
const le = (b, o) => b[o] | b[o + 1] << 8;
const s16 = (b, o) => (le(b, o) << 16) >> 16;
const u32 = (b, o) => (b[o] | b[o + 1] << 8 | b[o + 2] << 16 | b[o + 3] << 24) >>> 0;
const s8 = (b, o) => (b[o] << 24) >> 24;
function rgb(c) { return [(c & 31) * 255 / 31 | 0, (c >> 5 & 31) * 255 / 31 | 0, (c >> 10 & 31) * 255 / 31 | 0]; }
const cssRgb = c => { const [r, g, b] = rgb(c); return `rgb(${r},${g},${b})`; };
// A sprite palette learned from a capture (build/centaur/sprite_palettes.json):
// the OBJ palette RAM and the bank the resource's bank 0 used; older entries
// are one bank of 16 colors.
function learnedPalette(c) {
  if (!c) return null;
  if (Array.isArray(c)) { const one = c.map(rgb); return Array.from({ length: 256 }, (_, i) => one[i & 15]); }
  return Array.from({ length: 256 }, (_, i) => rgb(c.pal[((((i >> 4) + c.base) & 15) << 4) | (i & 15)]));
}
// 256 RGB triples from 512 bytes of GBA palette data at `off`.
const palFromBytes = (b, off = 0) => Array.from({ length: 256 }, (_, i) => rgb(le(b, off + i * 2)));

const resCache = new Map();
function res(label, hash) {
  const k = label + '/' + hash;
  if (!resCache.has(k)) resCache.set(k, fetch(`/api/res/${label}/${hash}`).then(r => { if (!r.ok) throw new Error('no ' + k); return r.arrayBuffer(); }).then(b => new Uint8Array(b)));
  return resCache.get(k);
}

// Draw one 4bpp 8x8 tile into ImageData at (x, y).
function tile(img, src, off, x, y, colors, bank, hf, vf) {
  const W = img.width, d = img.data;
  for (let r = 0; r < 8; r++) for (let c = 0; c < 8; c++) {
    const v = (src[off + r * 4 + (c >> 1)] >> (4 * (c & 1))) & 15;
    if (!v) continue;
    const px = x + (hf ? 7 - c : c), py = y + (vf ? 7 - r : r);
    if (px < 0 || py < 0 || px >= W || py >= img.height) continue;
    const [R, G, B] = colors[bank * 16 + v], o = (py * W + px) * 4;
    d[o] = R; d[o + 1] = G; d[o + 2] = B; d[o + 3] = 255;
  }
}
// 8bpp tile (64 bytes).
function tile8(img, src, off, x, y, colors, hf, vf) {
  const W = img.width, d = img.data;
  for (let r = 0; r < 8; r++) for (let c = 0; c < 8; c++) {
    const v = src[off + r * 8 + c];
    if (!v) continue;
    const px = x + (hf ? 7 - c : c), py = y + (vf ? 7 - r : r);
    if (px < 0 || py < 0 || px >= W || py >= img.height) continue;
    const [R, G, B] = colors[v], o = (py * W + px) * 4;
    d[o] = R; d[o + 1] = G; d[o + 2] = B; d[o + 3] = 255;
  }
}
function toCanvas(img, z = 1) {
  const cv = document.createElement('canvas');
  cv.width = img.width; cv.height = img.height;
  cv.getContext('2d').putImageData(img, 0, 0);
  cv.style.width = img.width * z + 'px'; cv.style.height = img.height * z + 'px';
  return cv;
}
// Scale a canvas to fit a max x max box (for thumbnails).
function fitCanvas(cv, max) {
  const s = Math.min(4, max / Math.max(cv.width, cv.height));
  cv.style.width = cv.width * s + 'px'; cv.style.height = cv.height * s + 'px';
  return cv;
}
// Compose parts [{x,y,w,h,tileOff(tx,ty),src,bank,hf,vf}] into one image.
function compose(parts, colors, box) {
  if (!parts.length) return null;
  let x0 = Math.min(...parts.map(p => p.x)), y0 = Math.min(...parts.map(p => p.y));
  let x1 = Math.max(...parts.map(p => p.x + p.w)), y1 = Math.max(...parts.map(p => p.y + p.h));
  if (box) { x0 = Math.min(x0, box[0]); y0 = Math.min(y0, box[1]); x1 = Math.max(x1, box[2]); y1 = Math.max(y1, box[3]); }
  const img = new ImageData(Math.max(1, x1 - x0), Math.max(1, y1 - y0));
  for (const p of [...parts].reverse()) {
    const tw = p.w / 8, th = p.h / 8;
    for (let ty = 0; ty < th; ty++) for (let tx = 0; tx < tw; tx++) {
      const dx = p.hf ? (tw - 1 - tx) * 8 : tx * 8, dy = p.vf ? (th - 1 - ty) * 8 : ty * 8;
      tile(img, p.src, p.tileOff(tx, ty), p.x - x0 + dx, p.y - y0 + dy, colors, p.bank, p.hf, p.vf);
    }
  }
  return img;
}

// CB05 sprite package (tools/assets/sprites.py)
function parsePackage(b) {
  const nframes = le(b, 2), nanims = le(b, 4), offs = [0, 1, 2, 3, 4].map(i => u32(b, 0xC + i * 4));
  const frames = [], anims = [];
  for (let i = 0; i < nframes; i++) {
    const o = offs[0] + i * 20;
    frames.push({ id: le(b, o), n: le(b, o + 2), po: u32(b, o + 16) });
  }
  for (let i = 0; i < nanims; i++) {
    const o = offs[1] + i * 8, n = le(b, o + 2), ko = u32(b, o + 4), keys = [];
    for (let k = 0; k < n; k++) keys.push({ frame: le(b, offs[3] + ko + k * 4), dur: le(b, offs[3] + ko + k * 4 + 2) });
    anims.push({ id: le(b, o), keys });
  }
  const parts = f => {
    const out = [];
    for (let k = 0; k < f.n; k++) {
      const o = offs[2] + f.po + k * 8, attr = le(b, o), a2 = le(b, o + 6);
      const [w, h] = SIZES[(attr >> 8) & 3][(attr >> 10) & 3], t = a2 & 0xFFF;  // 12-bit tile number (DE23 has 4096 tiles)
      out.push({ x: s16(b, o + 2), y: s16(b, o + 4), w, h, bank: a2 >> 12, hf: !!(attr & 4), vf: !!(attr & 8), src: b, tileOff: (tx, ty) => offs[4] + (t + ty * 16 + tx) * 32 });
    }
    return out;
  };
  return { frames, anims, parts, ntiles: le(b, 10) };
}

// 98F5 large sprites (tools/assets/bigsprites.py)
function parseBig(d) {
  const count = u32(d, 4), pixels = u32(d, 0xC), framesAt = u32(d, 0x10), partsAt = u32(d, 0x14), recs = [];
  for (let i = 0; i < count; i++) {
    const o = 0x18 + i * 12;
    recs.push({ hash: u32(d, o).toString(16).toUpperCase().padStart(4, '0'), w: d[o + 4], h: d[o + 5], cx: d[o + 6], cy: d[o + 7], foff: u32(d, o + 8) });
  }
  const frames = i => {
    const a = recs[i].foff, e = i + 1 < count ? recs[i + 1].foff : partsAt - framesAt, out = [];
    for (let o = a; o < e; o += 12) out.push({ n: d[framesAt + o], pal: le(d, framesAt + o + 2), pix: u32(d, framesAt + o + 4), po: u32(d, framesAt + o + 8) });
    return out;
  };
  const parts = f => {
    const out = []; let t = 0;
    for (let k = 0; k < f.n; k++) {
      const o = partsAt + f.po + k * 4, attr = d[o], [w, h] = SIZES[attr & 3][(attr >> 2) & 3], t0 = t;
      out.push({ x: s8(d, o + 2), y: s8(d, o + 3), w, h, bank: 0, hf: false, vf: false, src: d, tileOff: (tx, ty) => pixels + f.pix + (t0 + ty * (w / 8) + tx) * 32 });
      t += (w / 8) * (h / 8);
    }
    return out;
  };
  // a frame's u16 at +2 is its default palette pool entry (9B1B); AnimData_Load
  // (082151E4) applies the first frame's, so it is the sprite's palette until
  // code picks another (AnimData_SelectPaletteBank, data/sprite_palettes.csv)
  recs.forEach((r, i) => { r.pal = frames(i)[0]?.pal ?? -1; });
  return { recs, frames, parts };
}

// ---------- resource types (docs/ASSETS.md) ----------
const RES_TYPES = {
  anm: 'animation scripts (922E)', t92B3: 'palettes: 20-byte header + 256 colors (92B3)',
  t9B1B: 'sprite palette pool (9B1B): u16 count, u16 ?, then 16-color sub-palettes; a sprite package names its first entry',
  t98F5: 'large sprites (98F5)', fnt: 'font (A635)', gcx: 'compiled scripts (A8D9)',
  tAE6C: 'HP data (AE6C): LZ77, magic "HP"; the third argument of proc_884(map, tileset, hp), probably collision or height, not decoded',
  map: 'maps (C091): LZ77, magic "MP"', tCB05: 'sprite packages (CB05)', ptn: 'tileset library (CEEF)', tCEAA: 'unknown 10 KB blob (CEAA)',
};
// Which archive (as /api/res names it) a Centaur resource event belongs to.
const archiveFor = (label, type, index) => index[label] ? label : index['t' + type] ? 't' + type : label === 'hp' ? 'tAE6C' : null;

// ---------- sprite palettes: the 9B1B pool ----------
// Sprite package word 0 is the pool index of its palette bank 0; part bank b
// uses pool entry base + b by default (the code showing a sprite may choose
// another, as menus do). The palette manager copies pool entries into OBJ
// banks: IWRAM 0x030051F0 holds the pool index per bank, 0x03004DC0 the
// shadow of OBJ palette RAM.
let _pool = null;
function spritePool() {
  return _pool ??= fetch('/api/res').then(r => r.json()).then(ix => res('t9B1B', ix.t9B1B[0][0])).then(b => {
    const n = le(b, 0);
    return { n, entry: i => i >= 0 && i < n ? Array.from({ length: 16 }, (_, k) => rgb(le(b, 4 + i * 32 + k * 2))) : null };
  });
}
// 256 colors for a package: bank b = pool entry base + b.
async function packageColors(pkgBytes, base = le(pkgBytes, 0)) {
  const pool = await spritePool(), grey = k => { const v = k * 17; return [v, v, v]; };
  return Array.from({ length: 256 }, (_, i) => (pool.entry(base + (i >> 4)) || null)?.[i & 15] ?? grey(i & 15));
}

// A thumbnail of a sprite's first frame ('spr' = CB05 package, 'big' = 98F5
// large sprite), in captured colors when the stage survey saw it.
let _learnedSpr = null, _bigSpr = null;
const learnedSprites = () => _learnedSpr ??= fetch('/api/learned').then(r => r.json()).then(l => l.sprites || {}).catch(() => ({}));
const bigSprites = () => _bigSpr ??= fetch('/api/res').then(r => r.json()).then(ix => res('t98F5', ix.t98F5[0][0])).then(parseBig);
async function spriteThumb(kind, hash, max = 64) {
  const L = await learnedSprites();
  let parts, colors;
  if (kind === 'big') {
    const big = await bigSprites(), i = big.recs.findIndex(r => r.hash === hash), f = i >= 0 && big.frames(i)[0];
    if (!f) return null;
    colors = await packageColors(null, big.recs[i].pal);
    parts = big.parts(f);
  } else {
    const b = await res('tCB05', hash), pkg = parsePackage(b), k = pkg.anims[0]?.keys[0]?.frame ?? 0;
    if (!pkg.frames[k]) return null;
    colors = await packageColors(b); parts = pkg.parts(pkg.frames[k]);
  }
  const img = compose(parts, colors);
  return img && fitCanvas(toCanvas(img), max);
}

// ---------- GBA screen state from a capture (pal/vram/oam/io .bin) ----------
// One OAM entry drawn from VRAM, as an ImageData (affine entries unrotated).
function oamImage(cap, e) {
  const img = new ImageData(e.w, e.h), obj1d = !!(le(cap.io, 0) & 0x40);
  const tw = e.w / 8, th = e.h / 8;
  for (let ty = 0; ty < th; ty++) for (let tx = 0; tx < tw; tx++) {
    const dx = e.hf ? (tw - 1 - tx) * 8 : tx * 8, dy = e.vf ? (th - 1 - ty) * 8 : ty * 8;
    if (e.bpp8) {
      const t = e.tile + (obj1d ? (ty * tw + tx) * 2 : ty * 32 + tx * 2);
      tile8(img, cap.vram, 0x10000 + (t & 0x3FF) * 32, dx, dy, cap.objColors, e.hf, e.vf);
    } else {
      const t = e.tile + (obj1d ? ty * tw + tx : ty * 32 + tx);
      tile(img, cap.vram, 0x10000 + (t & 0x3FF) * 32, dx, dy, cap.objColors, e.bank, e.hf, e.vf);
    }
  }
  return img;
}
function parseOam(oam) {
  const out = [];
  for (let i = 0; i < 128; i++) {
    const a0 = le(oam, i * 8), a1 = le(oam, i * 8 + 2), a2 = le(oam, i * 8 + 4);
    const affine = !!(a0 & 0x100);
    if (!affine && (a0 & 0x200)) continue;  // hidden
    const [w, h] = SIZES[a0 >> 14][a1 >> 14];
    let y = a0 & 0xFF, x = a1 & 0x1FF;
    if (y >= 160) y -= 256;
    if (x >= 256) x -= 512;
    const dbl = affine && (a0 & 0x200);
    if (y + (dbl ? 2 * h : h) <= 0 || y >= 160 || x + w <= 0 || x >= 240) continue;
    out.push({ oam: i, x: dbl ? x + w / 2 : x, y: dbl ? y + h / 2 : y, w, h, affine, tile: a2 & 0x3FF, bank: a2 >> 12, prio: (a2 >> 10) & 3,
      bpp8: !!(a0 & 0x2000), hf: !affine && !!(a1 & 0x1000), vf: !affine && !!(a1 & 0x2000), mode: (a0 >> 10) & 3 });
  }
  return out;
}
// A text-mode BG layer (n = 0..3) as an ImageData of its whole map.
function bgLayer(cap, n) {
  const cnt = le(cap.io, 8 + n * 2), cb = ((cnt >> 2) & 3) * 0x4000, sb = ((cnt >> 8) & 31) * 0x800, bpp8 = !!(cnt & 0x80), size = cnt >> 14;
  const W = size & 1 ? 64 : 32, H = size & 2 ? 64 : 32;
  const img = new ImageData(W * 8, H * 8);
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    const blk = (x >> 5) + (y >> 5) * (W >> 5), e = le(cap.vram, sb + blk * 0x800 + ((y & 31) * 32 + (x & 31)) * 2);
    const t = e & 0x3FF, hf = !!(e & 0x400), vf = !!(e & 0x800);
    if (bpp8) { const o = cb + t * 64; if (o + 64 <= 0x10000) tile8(img, cap.vram, o, x * 8, y * 8, cap.bgColors, hf, vf); }
    else { const o = cb + t * 32; if (o + 32 <= 0x10000) tile(img, cap.vram, o, x * 8, y * 8, cap.bgColors, e >> 12, hf, vf); }
  }
  return { img, cnt, prio: cnt & 3, cb, sb, W, H, bpp8 };
}
async function loadCapture(name) {
  const f = n => fetch(`/api/live/file/captures/${encodeURIComponent(name)}/${n}`).then(r => r.ok ? r.arrayBuffer() : new ArrayBuffer(0)).then(b => new Uint8Array(b));
  const [pal, vram, oam, io, iwram] = await Promise.all(['pal.bin', 'vram.bin', 'oam.bin', 'io.bin', 'iwram.bin'].map(f));
  // pool index per OBJ bank (high bit set: not a pool entry)
  const objPool = iwram.length >= 0x5210 ? Array.from({ length: 16 }, (_, b) => le(iwram, 0x51F0 + b * 2)) : [];
  return { name, pal, vram, oam, io, objPool, bgColors: palFromBytes(pal, 0), objColors: palFromBytes(pal, 0x200) };
}

// Up/down arrows move through a list (outside text fields): the row matching
// `onSel` is the current one; the next/previous row is clicked.
function listKeys(listEl, rowSel, onSel = '.on') {
  document.addEventListener('keydown', e => {
    if (e.key !== 'ArrowDown' && e.key !== 'ArrowUp') return;
    if (e.target.closest && e.target.closest('input, textarea, select')) return;
    const rows = [...listEl().querySelectorAll(rowSel)];
    if (!rows.length) return;
    const i = rows.findIndex(r => r.matches(onSel));
    const n = rows[Math.max(0, Math.min(rows.length - 1, i < 0 ? 0 : i + (e.key === 'ArrowDown' ? 1 : -1)))];
    if (n && n !== rows[i]) { e.preventDefault(); n.click(); n.scrollIntoView({ block: 'nearest' }); }
  });
}
