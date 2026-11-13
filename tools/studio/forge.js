'use strict';
studioHeader();
let allFunctions = [], current = null, latest = null, busy = false, routeVersion = 0;
const editor = $('editor');
const bootScript = 'wait 600\nshot result';
const continueScript = 'wait 300\ntap START\nwait 60\ntap A\nwait 150\ntap START\nwait 120\ntap A\nwait 120\ntap START\nwait 300\ntap START\nwait 120\ntap START\nwait 120\ntap A\nwait 150\ntap A\nwait 300\nshot field';

async function api(url, body) {
  const r = await fetch(url, body === undefined ? {} : {method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify(body)});
  const d = await r.json();
  if (!r.ok) throw Error(d.error || `Request failed (${r.status})`);
  return d;
}
function status(message, kind = '') { $('status').textContent = message; $('status').className = kind; }
function buttons() {
  ['compile', 'play', 'reset'].forEach(id => $(id).disabled = !current || busy);
  $('find').disabled = busy;
  editor.disabled = !current || busy;
  document.querySelectorAll('.fn').forEach(b => b.disabled = busy);
}
function lineNumbers() {
  $('lines').textContent = Array.from({length: editor.value.split('\n').length}, (_, i) => i + 1).join('\n');
  $('lines').scrollTop = editor.scrollTop;
}
function draftKey() { return 'forge.draft.' + current.addr; }
function changed() {
  lineNumbers();
  if (!current) return;
  const dirty = editor.value !== current.source;
  $('dirty').textContent = dirty ? 'Edited · draft saved in this browser' : 'Original source';
  localStore(draftKey(), JSON.stringify({sha256: current.sha256, source: editor.value}));
  // Downloads always belong to a compiled snapshot, never to uncompiled edits.
  const stale = latest && editor.value !== latest.source;
  $('download').hidden = !latest?.result.patch_url || stale;
  $('download-source').hidden = !latest?.result.source_url || stale;
}
function renderFunctions() {
  const query = $('find').value.trim().toLowerCase();
  const matches = allFunctions.filter(f => !query || `${f.name} ${f.addr} ${f.unit} ${f.category}`.toLowerCase().includes(query));
  const selected = current && !query ? allFunctions.filter(f => f.addr === current.addr) : [];
  const fs = [...selected, ...matches.filter(f => !selected.includes(f))].slice(0, 60);
  $('functions').innerHTML = fs.map(f => `<button class="fn${current?.addr === f.addr ? ' on' : ''}" data-addr="${f.addr}" role="listitem"><b>${esc(f.name)}</b><small>${esc(f.addr)} · ${esc(f.unit)}</small></button>`).join('') +
    `<p class="muted" style="padding:8px;font-size:10px">${matches.length.toLocaleString()} functions${matches.length > 60 ? ' · narrow your search' : ''}</p>`;
  buttons();
}
async function openFunction(query) {
  if (busy) return;
  const version = ++routeVersion;
  try {
    status('Opening source…', 'busy');
    const f = await api('/api/forge/source/' + encodeURIComponent(query));
    if (version !== routeVersion) return;
    current = f; latest = null;
    $('name').textContent = f.name;
    $('crumb').textContent = `${f.category} / ${f.unit}`;
    $('file').textContent = `${f.file} · ${f.addr} · ${f.size} bytes`;
    $('explore').hidden = false; $('explore').href = '/code#' + f.addr;
    document.title = f.name + ' · Boktai 3 Forge';
    editor.value = f.source;
    try {
      const draft = JSON.parse(localStore(draftKey()) || 'null');
      if (draft?.sha256 === f.sha256 && typeof draft.source === 'string') editor.value = draft.source;
    } catch (_) { /* an unavailable browser draft does not prevent editing */ }
    $('download').hidden = $('download-source').hidden = true;
    $('comparison').className = 'empty';
    $('comparison').innerHTML = '<div class="empty-mark">{ }</div><h3>Ready for your experiment.</h3><p>Compile to compare with the original ROM.<br>Build & play to take the result into the game.</p>';
    $('why').textContent = 'Compile an experiment to inspect its compiler evidence.';
    $('shots').className = 'empty'; $('shots').innerHTML = '<p>Build & play returns screenshots of this experiment.</p>';
    $('explain').open = false;
    changed(); buttons(); renderFunctions();
    jumpToLine(f.line, false);
    status(editor.value === f.source ? 'Original source loaded. Compile it to verify MATCH, then make it yours.' : 'Restored your browser draft. Compile to see what changed.');
  } catch (e) { status(e.message, 'error'); }
}
function jumpToLine(line, select = true) {
  const n = Number(line);
  if (!Number.isInteger(n) || n < 1) return;
  const lines = editor.value.split('\n');
  const start = lines.slice(0, n - 1).reduce((sum, s) => sum + s.length + 1, 0);
  if (select) { editor.focus(); editor.setSelectionRange(start, start + (lines[n - 1]?.length || 0)); }
  editor.scrollTop = Math.max(0, (n - 5) * parseFloat(getComputedStyle(editor).lineHeight));
  $('lines').scrollTop = editor.scrollTop;
}
function renderComparison(result) {
  const c = result.comparison;
  if (!c) return;
  const annotations = new Map((result.scope_rows || []).filter(r => r.draft_off != null).map(r => [parseInt(current.addr, 16) + r.draft_off, r]));
  let rows = result.rows || [];
  if ($('diff-only').checked) rows = rows.filter(r => r.tag !== '=');
  $('comparison').className = '';
  $('comparison').innerHTML = `<div class="compare-summary">${c.status === 'match' ? 'MATCH · byte-identical' : `${c.different_byte_count} differing bytes`} · ${c.candidate_bytes} compiled / ${c.target_bytes} original bytes${result.can_play ? ' · fits original layout' : ''}</div>` +
    (rows.length ? `<table class="insns"><thead><tr><th>ROM</th><th>Your compile</th><th>C / pass</th></tr></thead><tbody>${rows.map(r => {
      const ann = annotations.get(parseInt(r.draft_addr, 16));
      const line = ann?.line && /^\d+$/.test(ann.line) ? ann.line : null;
      return `<tr class="${r.tag !== '=' ? 'changed' : ''}"><td><span class="address">${esc(r.rom_addr)}</span>${esc(r.rom)}</td><td><span class="address">${esc(r.draft_addr)}</span>${esc(r.draft)}</td><td>${line ? `<button class="source-line" data-line="${line}" title="Jump to C line ${line}">L${line}</button>` : esc(ann?.line || '')}<span class="pass"> ${esc(ann?.born || '')}</span></td></tr>`;
    }).join('')}</tbody></table>` : '<p class="empty">No instruction differences.</p>');
}
function renderResult(result, snapshot) {
  latest = {result, source: snapshot};
  renderComparison(result);
  const c = result.comparison;
  if (result.state === 'error') status(result.error || result.stage, 'error');
  else if (c) status(`${c.status === 'match' ? 'MATCH — this C builds the original bytes.' : `Compiled — ${c.different_byte_count} bytes changed in this unit.`}${result.play_blocked ? '\n' + result.play_blocked : result.patch_url ? '\nIPS patch and source snapshot are ready to export.' : ''}`, c.status === 'match' ? 'ok' : '');
  $('why').textContent = [result.explanation, result.scope_log].filter(Boolean).join('\n\n') || 'Compiler diagnostics were unavailable. The stock compiler comparison above is authoritative.';
  if (result.patch_url) { $('download').href = result.patch_url; $('download').download = current.name + '.ips'; }
  if (result.source_url) { $('download-source').href = result.source_url; $('download-source').download = current.file.split('/').pop(); }
  if (result.screenshots?.length) {
    $('shots').className = '';
    $('shots').innerHTML = result.screenshots.map(s => `<figure><a href="${s.url}" target="_blank" rel="noopener"><img src="${s.url}" alt="Game screenshot: ${esc(s.name)}" width="360" height="240"></a><figcaption>${esc(s.name)} · ${esc($('checkpoint').selectedOptions[0]?.textContent || '')}</figcaption></figure>`).join('');
  }
  changed();
}
async function compile(action) {
  if (!current || busy) return;
  const snapshot = editor.value;
  let textEdits;
  try { textEdits = JSON.parse($('text-edits').value || '[]'); }
  catch (_) { status('Text edits must be a JSON array, such as [{"id":988,"text":"POTION"}].', 'error'); return; }
  busy = true; buttons();
  $('download').hidden = $('download-source').hidden = true;
  try {
    status('Sending source snapshot…', 'busy');
    const job = await api('/api/forge/compile', {function: current.addr, source: snapshot, sha256: current.sha256, action,
      checkpoint: $('checkpoint').value, script: $('replay').value, text_edits: textEdits});
    let result;
    do {
      await new Promise(resolve => setTimeout(resolve, 700));
      result = await api('/api/forge/job/' + job.id);
      status(result.stage + '…', 'busy');
      if (result.comparison) renderComparison(result);
    } while (!['done', 'error'].includes(result.state));
    renderResult(result, snapshot);
  } catch (e) { status(e.message, 'error'); }
  finally { busy = false; buttons(); }
}
$('find').oninput = renderFunctions;
$('find').onkeydown = e => { if (e.key === 'Enter') $('functions').querySelector('.fn')?.click(); };
$('functions').onclick = e => { const b = e.target.closest('[data-addr]'); if (b && !busy) location.hash = b.dataset.addr; };
$('compile').onclick = () => compile('compile');
$('play').onclick = () => compile('play');
$('reset').onclick = () => { if (current) { editor.value = current.source; changed(); status('Original C restored. Compile to verify its bytes.'); } };
editor.oninput = changed;
editor.onscroll = () => $('lines').scrollTop = editor.scrollTop;
editor.onkeydown = e => {
  if (e.key === 'Tab') { e.preventDefault(); editor.setRangeText('    ', editor.selectionStart, editor.selectionEnd, 'end'); changed(); }
  if (e.key === 'Enter' && (e.metaKey || e.ctrlKey)) { e.preventDefault(); compile('compile'); }
};
$('comparison').onclick = e => { const b = e.target.closest('[data-line]'); if (b) jumpToLine(b.dataset.line); };
$('diff-only').onchange = () => { if (latest) renderComparison(latest.result); };
$('boot-preset').onclick = () => $('replay').value = bootScript;
$('continue-preset').onclick = () => $('replay').value = continueScript;
$('checkpoint').onchange = () => { $('replay').value = $('checkpoint').value === 'boot' ? bootScript : continueScript; };
window.addEventListener('hashchange', () => { if (!busy) openFunction(decodeURIComponent(location.hash.slice(1))); });
(async () => {
  try {
    const [fs, cps] = await Promise.all([api('/api/forge/functions'), api('/api/forge/checkpoints')]);
    allFunctions = fs;
    $('checkpoint').innerHTML = cps.map(c => `<option value="${esc(c.id)}">${esc(c.story != null ? `Story ${c.story} · ` : '')}${esc(c.label)}</option>`).join('');
    renderFunctions();
    if (location.hash) await openFunction(decodeURIComponent(location.hash.slice(1)));
  } catch (e) { status(e.message, 'error'); }
})();
