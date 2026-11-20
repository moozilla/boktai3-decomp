'use strict';

const API = '/api/experiments';
let example, catalog, historyRows = [], draft, primary, comparison;
let selected = { trial: 0, kind: 'field', index: 0 };
let sample = 0, timer, loadToken = 0, compareToken = 0, running = false;
let readbackKey = '', queryToken = 0, pixelToken = 0;
const requestProbe = new URLSearchParams(location.search).get('probe');

const pad = n => String(n).padStart(4, '0');
const showValue = value => value == null ? '—' : typeof value === 'number' ? value.toLocaleString('en-US') : value;
const signed = value => value > 0 ? '+' + showValue(value) : showValue(value);
const resultPath = id => API + '/runs/' + id;
const artifact = (path, id = primary.id) => resultPath(id) + '/artifact/' + path;
const pause = ms => new Promise(resolve => setTimeout(resolve, ms));
const unsignedHex = value => '0x' + value;
const savedGame = id => id !== 'boot' && !/^(bookmark|recording)-/.test(id);

async function json(url, body) {
  const options = body === undefined ? {} : {
    method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body),
  };
  const response = await fetch(url, options);
  const data = await response.json();
  if (!response.ok) throw new Error(data.error || 'Experiment request failed');
  return data;
}

function setStatus(text, error = false) {
  $('status-line').textContent = text;
  $('status-line').classList.toggle('error', error);
}

function header() {
  const spec = primary?.recipe || example;
  if (!spec) return;
  const label = catalog?.checkpoints.find(checkpoint => checkpoint.id === spec.checkpoint)?.label || spec.checkpoint;
  $('title').textContent = spec.title;
  $('question-context').textContent = primary
    ? `${primary.checkpoint.label} · same starting frame ${showValue(primary.trials[0].snapshots[0].frame)} · ${primary.trials.length} trials`
    : `${label} · ${spec.trials.length} trials from one starting state · ${spec.duration} frames each`;
  $('run').textContent = primary ? 'Run again' : 'Run experiment';
  $('run').disabled = running;
  $('edit').disabled = !spec;
  $('sweep-open').disabled = !catalog || Boolean(spec.fork);
  const origin = primary?.checkpoint.fork;
  const sweep = primary?.sweep;
  $('parent-run').hidden = !origin && !sweep;
  $('parent-run').textContent = sweep ? 'Back to checkpoint results' : 'Open the parent frame';
  if (sweep) $('parent-run').href = `#sweep/${sweep.id}`;
  if (origin) $('parent-run').href = `#run/${origin.run}/${origin.frame}/${origin.trial}`;
  pageTitle(spec.title, 'Experiments');
}

function drawPreview(spec) {
  $('sweep-results').hidden = true;
  $('preview').innerHTML = spec.trials.map(trial =>
    `<article><h2>${esc(trial.label)}</h2><pre>${esc(trial.input || 'Wait for the trial duration')}</pre>
      ${(trial.interventions || []).map(action => `<p class="claim-math">After frame ${action.frame},
        set <code>${esc(action.field)}</code> to ${showValue(action.value)}.</p>`).join('')}</article>`
  ).join('');
}

function deepLink() {
  if (!primary) return;
  const relative = primary.trials[0].snapshots[sample].relative;
  history.replaceState(null, '', `#run/${primary.id}/${relative}/${primary.trials[selected.trial].id}/${selected.kind}/${selected.index}`);
}

function splitHash() {
  const [view, id, frame, trial, kind, index] = location.hash.slice(1).split('/');
  return view === 'run' && /^[a-f0-9]{32}$/.test(id) ? { id, frame: Number(frame || -1), trial, kind, index: Number(index || 0) } : null;
}

async function refreshHistory() {
  historyRows = await json(API + '/runs');
  const current = primary?.id || '';
  $('history-select').innerHTML = '<option value="">Choose a run</option>' + historyRows.map(row =>
    `<option value="${row.id}">${esc(row.title)} · ${row.id.slice(0, 7)}${row.state === 'complete' ? '' : ' · ' + esc(row.state)}</option>`
  ).join('');
  $('history-select').value = current;
  $('examples').value = '';
  const earlier = historyRows.filter(row => row.state === 'complete' && row.id !== current);
  $('compare-panel').hidden = !current || !earlier.length;
  $('compare').innerHTML = '<option value="">Choose a completed run</option>' + earlier.map(row =>
    `<option value="${row.id}">${esc(row.title)} · ${row.id.slice(0, 7)}</option>`
  ).join('');
  $('compare').value = comparison?.id || '';
}

async function submit(spec) {
  const token = ++loadToken;
  running = true;
  stopPlayback();
  header();
  setStatus('Submitting the recipe…');
  try {
    const job = await json(API + '/runs', spec);
    if (token !== loadToken) return;
    $('editor').close();
    history.replaceState(null, '', '#run/' + job.id);
    await waitForRun(job.id, null, token);
  } catch (error) {
    if (token !== loadToken) return;
    setStatus(error.message, true);
    $('editor-error').textContent = error.message;
  } finally {
    if (token === loadToken) {
      running = false;
      header();
    }
  }
}

async function waitForRun(id, target, token) {
  for (;;) {
    const state = await json(resultPath(id));
    if (token !== loadToken) return;
    if (state.state === 'failed' || state.state === 'interrupted') {
      const saved = await json(artifact('recipe.json', id));
      if (token !== loadToken) return;
      primary = null;
      comparison = null;
      example = saved;
      $('results').hidden = true;
      $('preview').hidden = false;
      $('artifacts').innerHTML = '';
      drawPreview(saved);
      await refreshHistory();
      setStatus(state.message, true);
      return;
    }
    if (state.state === 'complete') break;
    setStatus(state.message + '…');
    await pause(450);
  }
  const result = await json(resultPath(id) + '/result');
  if (token !== loadToken) return;
  const old = primary;
  showResult(result, target);
  if (old && old.id !== result.id && old.recipe.title === result.recipe.title) comparison = old;
  await refreshHistory();
  drawComparison();
}

async function openRun(route) {
  const token = ++loadToken;
  running = true;
  header();
  stopPlayback();
  setStatus('Opening saved evidence…');
  try {
    await waitForRun(route.id, route, token);
  } catch (error) {
    if (token === loadToken) setStatus(error.message, true);
  } finally {
    if (token === loadToken) {
      running = false;
      header();
    }
  }
}

function unmetRequirement(check) {
  return check.prerequisites?.find(condition => !condition.meets);
}

function describeRequirement(condition) {
  const name = condition.field || condition.function + ' entries';
  return `${name} · ${condition.trial}, frame ${condition.to} · wanted ${condition.sign} ${showValue(condition.value)} · observed ${showValue(condition.measurement)}`;
}

function showResult(result, route) {
  $('sweep-results').hidden = true;
  primary = result;
  draft = result.recipe;
  comparison = null;
  const trials = result.trials;
  const last = trials[0].snapshots.length - 1;
  const time = trials[0].snapshots.findIndex(snap => snap.relative === route?.frame);
  sample = time < 0 ? last : time;
  const trial = Math.max(0, trials.findIndex(item => item.id === route?.trial));
  const kind = route?.kind === 'probe' && trials[trial].probes.length ? 'probe' : 'field';
  const max = kind === 'probe' ? trials[trial].probes.length - 1 : result.fields.length - 1;
  const initial = result.fields.findIndex((_, i) => trials[0].snapshots[last].fields[i].last_write);
  const requestedIndex = Number.isInteger(route?.index) && route.index >= 0 ? route.index : 0;
  selected = { trial, kind, index: route ? Math.min(max, requestedIndex) : Math.max(0, initial) };
  $('preview').hidden = true;
  $('results').hidden = false;
  $('scrub').max = last;
  $('screens').style.setProperty('--trials', trials.length);
  $('screens').innerHTML = trials.map((item, i) => `<article class="screen" id="screen-${i}">
    <button type="button" data-trial="${i}"><span>${esc(item.label)}</span><span id="input-${i}"></span></button>
    <div class="frame-view"><img id="image-${i}" data-pixel-trial="${i}" width="240" height="160"
      alt="${esc(item.label)} game frame"><span class="pixel-mark" id="pixel-mark-${i}" hidden></span></div>
  </article>`).join('');
  $('claims').innerHTML = (result.checks || []).map((item, i) => {
    const mark = { confirmed: '✓', contradicted: '×', unresolved: '?' }[item.state];
    const requirement = unmetRequirement(item);
    const explanation = requirement ? describeRequirement(requirement) : '';
    return `<button type="button" data-claim="${i}" aria-label="${esc(item.state + '. ' + item.statement + (explanation ? '. Requires ' + explanation : ''))}">
      <span class="claim-mark ${item.state}">${mark}</span>
      <span>${esc(item.statement)}<span class="claim-math">${explanation
        ? 'Requires ' + esc(explanation) : showValue(item.measurement) + ' ' + esc(item.sign) + ' ' + showValue(item.value)}</span></span>
    </button>`;
  }).join('');
  $('artifacts').innerHTML = [
    ['recipe.json', 'Recipe JSON'], ['transcript.txt', 'Transcript'], ['contact.png', 'Contact sheet'],
  ].map(([name, label]) => `<a href="${artifact(name)}" target="_blank" rel="noopener">${label}</a>`).join('');
  setStatus('');
  header();
  renderFrame();
}

function lastInput(input, target) {
  let elapsed = 0;
  let last = 'No input';
  for (const raw of input.split('\n')) {
    const [action, a, b] = raw.split('#', 1)[0].trim().split(/\s+/);
    if (action === 'lux') continue;
    const n = Number(action === 'wait' ? a : action === 'hold' ? b : action === 'tap' ? 10 : 0);
    if (!n) continue;
    last = action === 'wait' ? 'No input' : a.toUpperCase();
    if (target <= elapsed + n) return action === 'tap' && target > elapsed + 2 ? 'No input' : last;
    elapsed += n;
  }
  return elapsed < target ? 'No input' : last;
}

function renderFrame() {
  const snap = primary.trials[0].snapshots[sample];
  const trialFrames = primary.trials[0].snapshots;
  $('scrub').value = sample;
  $('frame').value = `Frame ${pad(snap.relative)} / ${pad(primary.recipe.duration)}`;
  $('scrub').setAttribute('aria-valuetext', `Frame ${snap.relative} of ${primary.recipe.duration}`);
  $('previous').disabled = sample === 0;
  $('next').disabled = sample === trialFrames.length - 1;
  const selectedTrial = primary.trials[selected.trial];
  const selectedPoint = selectedTrial.snapshots[sample];
  $('fork-frame').disabled = !selectedPoint.state_sha256;
  $('snapshot-query').hidden = !selectedPoint.state_sha256;
  $('sprite-evidence').hidden = !selectedPoint.graphics;
  const key = `${primary.id}/${selectedTrial.id}/${selectedPoint.relative}`;
  if (key !== readbackKey) {
    readbackKey = key;
    $('snapshot-value').value = '';
    ++queryToken;
    ++pixelToken;
    $('pixel-results').innerHTML = '';
    primary.trials.forEach((_, i) => { $('pixel-mark-' + i).hidden = true; });
  }
  primary.trials.forEach((t, i) => {
    const image = $('image-' + i);
    const url = artifact(`${t.id}/${t.snapshots[sample].label}.png`);
    if (image.getAttribute('src') !== url) image.src = url;
    image.classList.toggle('inspectable', Boolean(t.snapshots[sample].graphics));
    $('screen-' + i).classList.toggle('active', selected.trial === i);
    $('input-' + i).textContent = snap.relative === 0 ? 'Shared start' : lastInput(t.input, snap.relative);
  });
  drawMatrix();
  drawEvidence();
  deepLink();
}

function spriteSubmission(draw) {
  if (!draw.function) return '<p>Native submission was outside the traced renderer routines.</p>';
  const actor = draw.actor;
  const state = !actor ? '' : `<p><code>${esc(actor.member)}</code> in <code>${esc(actor.type)}</code>
    at ${unsignedHex(actor.address)}</p>` + (actor.fields.length ? `<div class="pixel-fields">${actor.fields.map(row =>
    `<button type="button" data-pixel-field="${row.field}"><code>${esc(row.path)}</code> ${showValue(row.value)}</button>`
  ).join('')}</div>` : '');
  return `<div class="pixel-source">
    <div><a href="${esc(draw.function.url)}" data-peek>${esc(draw.function.name)}</a>
      <code>${unsignedHex(draw.runtime_pc)}</code>
      ${draw.caller ? `<a href="${esc(draw.caller.url)}" data-peek>Caller</a>` : ''}</div>
    ${state}
    ${draw.asset ? `<p><a href="${esc(draw.asset.url)}" data-peek>ROM art ${esc(draw.asset.archive)}/${esc(draw.asset.hash)}</a>
      · tile source <code>${unsignedHex(draw.asset.tile_source)}</code></p>` : ''}
  </div>`;
}

async function inspectPixel(trialIndex, x, y) {
  const trial = primary.trials[trialIndex], point = trial.snapshots[sample];
  if (!point.graphics) return;
  stopPlayback();
  selected.trial = trialIndex;
  renderFrame();
  const token = ++pixelToken;
  $('pixel-x').value = x;
  $('pixel-y').value = y;
  const mark = $('pixel-mark-' + trialIndex);
  mark.style.left = ((x + .5) / 240 * 100) + '%';
  mark.style.top = ((y + .5) / 160 * 100) + '%';
  mark.hidden = false;
  const pane = $('pixel-results');
  pane.textContent = 'Reading the captured sprite buffers…';
  try {
    const evidence = await json(`${resultPath(primary.id)}/pixel/${trial.id}/${point.relative}/${x}/${y}`);
    if (token !== pixelToken) return;
    const header = `<p><b>${esc(trial.label)} · frame ${point.relative} · (${x}, ${y})</b>
      · RGB ${evidence.rgb.join(', ')}
      ${evidence.upload ? `· <a href="${esc(evidence.upload.function?.url || '#')}" data-peek>OAM upload</a>` : ''}</p>`;
    const source = evidence.objects.map(object => `<article>
      <div class="pixel-candidate">
        <h3>Matching sprite · hardware slot ${object.slot}</h3>
        ${point.relative ? `<button type="button" data-verify-slot="${object.slot}" data-x="${x}" data-y="${y}">Replay without this sprite</button>` : ''}
      </div>
      <p class="muted">VRAM tile ${object.tile} · OBJ palette index ${object.palette_index}</p>
      ${object.submissions.length ? object.submissions.map(spriteSubmission).join('') :
        '<p>This frame has hardware sprite data and no traced submission for this slot.</p>'}
      <div class="pixel-replay" role="status" id="pixel-replay-${object.slot}"></div>
    </article>`).join('');
    pane.innerHTML = header + (source || '<p>No unblended sprite fragment matched this pixel.</p>');
  } catch (error) {
    if (token === pixelToken) pane.textContent = error.message;
  }
}

async function testSprite(button) {
  const token = pixelToken;
  const {trial, snap} = atSelected();
  const {verifySlot: slot, x, y} = button.dataset;
  const pane = $('pixel-replay-' + slot);
  button.disabled = true;
  pane.textContent = 'Replaying twice from the same starting state…';
  try {
    const url = `${resultPath(primary.id)}/pixel-check/${trial.id}/${snap.relative}/${x}/${y}/${slot}`;
    const proof = await json(url, {});
    if (token !== pixelToken) return;
    const measured = proof.changed
      ? `The pixel changed from RGB ${proof.before.join(', ')} to ${proof.after.join(', ')}.`
      : 'The pixel kept the same color with this slot hidden.';
    pane.innerHTML = `<p><b>${measured}</b> Both replays reached the exact same emulator state.</p>
      <a href="${esc(proof.contact_sheet)}" target="_blank" rel="noopener"><img
        src="${esc(proof.contact_sheet)}" width="760" height="210" alt="Original frame, control crop, and crop with the selected sprite slot hidden"></a>`;
  } catch (error) {
    if (token === pixelToken) pane.textContent = error.message;
  } finally {
    button.disabled = false;
  }
}

function countEntries(probe, snapshots, index) {
  if (!index) return 0;
  let count = 0;
  for (let frame = snapshots[index - 1].relative; frame < snapshots[index].relative; ++frame) {
    count += probe.frames[frame] || 0;
  }
  return count;
}

function drawMatrix() {
  const trials = primary.trials;
  const probe = selected.kind === 'probe';
  $('fields-tab').setAttribute('aria-pressed', !probe);
  $('calls-tab').setAttribute('aria-pressed', probe);
  $('calls-tab').disabled = !trials[0].probes.length;
  const fields = probe ? trials[0].probes.map(p => p.name) : primary.fields.map(f => f.path);
  const top = `<thead><tr><th scope="col">${probe ? 'C function' : 'Typed field'}</th>${trials.map(t =>
    `<th scope="col">${esc(t.label)}</th>`).join('')}</tr></thead>`;
  const body = fields.map((name, index) => `<tr><th scope="row"><code>${esc(name)}</code></th>${trials.map((t, i) => {
    const s = t.snapshots[sample];
    const active = selected.trial === i && selected.index === index;
    let value, extra;
    if (probe) {
      value = countEntries(t.probes[index], t.snapshots, sample);
      extra = `${t.probes[index].count} total`;
    } else {
      const { value: current, address } = s.fields[index];
      const first = t.snapshots[0].fields[index];
      const delta = current != null && first.value != null && first.address === address ? current - first.value : null;
      value = current;
      extra = delta ? signed(delta) : '';
    }
    return `<td><button class="cell${extra && !probe ? ' changed' : ''}${active ? ' selected' : ''}" type="button" data-trial="${i}" data-index="${index}" aria-pressed="${active}">
      <span>${showValue(value)}</span><span class="delta">${esc(extra)}</span></button></td>`;
  }).join('')}</tr>`).join('');
  $('matrix').innerHTML = `<table>${top}<tbody>${body}</tbody></table>`;
}

function atSelected() {
  const trial = primary.trials[selected.trial];
  return { trial, snap: trial.snapshots[sample], index: selected.index };
}

function drawEvidence() {
  const { trial, snap, index } = atSelected();
  const details = $('evidence');
  if (selected.kind === 'probe') {
    const p = trial.probes[index];
    const count = countEntries(p, trial.snapshots, sample);
    details.innerHTML = `<h2>${esc(p.name)}</h2><p class="observed">${sample ? `${count} ${count === 1 ? 'entry' : 'entries'} since frame ${trial.snapshots[sample - 1].relative}` : 'Shared starting frame'}</p>
      <p>${showValue(p.count)} observed entries during ${esc(trial.label)}.</p>
      <div class="link-row">${p.function ? `<a href="${esc(p.function.url)}" data-peek>Open C and callers</a>` : ''}
      <a href="${artifact(trial.id + '/probes.tsv')}" target="_blank" rel="noopener">Register trace</a></div>`;
    return;
  }
  const field = primary.fields[index], row = snap.fields[index], record = row.last_write;
  const before = row.previous != null && row.previous !== row.value ? `${showValue(row.previous)} → ` : '';
  const header = `<h2><code>${esc(field.path)}</code></h2>
    <p class="meta">${esc(trial.label)} · frame ${snap.relative} ${row.address ? '· RAM ' + unsignedHex(row.address) : ''}</p>
    <p class="big-value">${before}${showValue(row.value)}</p>`;
  if (!record) {
    const intervention = row.last_intervention;
    if (intervention) {
      const moment = intervention.frame - trial.snapshots[0].frame;
      details.innerHTML = header + `<p class="observed">Recipe intervention after frame ${moment}</p>
        <p><code>${showValue(intervention.before)} → ${showValue(intervention.value)}</code></p>
        <a href="${artifact(trial.id + '/writes.json')}" target="_blank" rel="noopener">Observed events</a>`;
      return;
    }
    const notes = {
      initial: 'Read from the shared starting state.',
      unchanged: 'The sampled value stayed the same. No earlier value-changing CPU store is available here.',
      unbound: 'The pointer did not resolve to a RAM field at this frame.',
      rebound: 'The field points to a different object. Its sampled value is shown above.',
      unobserved: 'The sampled value changed, with no matching observed CPU store.',
    };
    details.innerHTML = header + `<p>${esc(notes[row.state] || 'No writer evidence at this frame.')}</p>`;
    return;
  }
  const f = record.function, c = record.compiler;
  const path = (record.context || []).filter(entry => entry.function).map(entry =>
    `<a href="${esc(entry.function.url)}" title="Observed return to ${esc(entry.resume)}" data-peek>${esc(entry.function.name)}</a>`
  ).join(' → ');
  const frame = record.frame - trial.snapshots[0].frame;
  const excerpt = c?.excerpt ? `<pre aria-label="Exact matching C excerpt">${c.excerpt.map((line, i) => {
    const n = c.first + i;
    const text = esc(`${n}  ${line}`);
    return n === c.line ? '<b>' + text + '</b>' : text + '\n';
  }).join('')}</pre>` : '';
  details.innerHTML = header + `<p class="observed">Last value-changing CPU store · frame ${frame}</p>
    <p><code>${showValue(record.before)} → ${showValue(record.value)}</code> · <strong>${esc(f?.name || 'Unmapped instruction')}</strong></p>
    <p class="meta">${unsignedHex(record.pc)}${c?.instruction ? ' · ' + esc(c.instruction) : ''}</p>
    ${c?.file ? `<p class="meta">${esc(c.file)}:${c.line}${c.compiler_pass ? ' · compiler pass ' + esc(c.compiler_pass) : ''}</p>` : ''}
    ${path ? `<p class="meta">Active probed entries <span>${path}</span></p>` : ''}
    ${excerpt}<div class="link-row">${f ? `<a href="${esc(f.url)}" data-peek>Open full function</a>` : ''}
    <a href="${artifact(trial.id + '/writes.json')}" target="_blank" rel="noopener">Write transcript</a></div>`;
}

function chooseFrame(index) {
  if (!primary) return;
  sample = Math.max(0, Math.min(primary.trials[0].snapshots.length - 1, Number(index)));
  renderFrame();
}

function chooseClaim(index) {
  const claim = primary.checks?.[index];
  if (!claim) return;
  const target = unmetRequirement(claim) || claim;
  selected = { trial: primary.trials.findIndex(t => t.id === target.trial),
    kind: target.evidence.kind, index: target.evidence.index };
  chooseFrame(primary.trials[0].snapshots.findIndex(s => s.relative === target.to));
}

function stopPlayback() {
  clearTimeout(timer);
  timer = undefined;
  $('play').textContent = 'Play';
  $('play').setAttribute('aria-label', 'Play trial frames');
}

function playback() {
  if (timer) return stopPlayback();
  if (!primary) return;
  const timeline = primary.trials[0].snapshots;
  const end = timeline.length - 1;
  if (sample === end) chooseFrame(0);
  $('play').textContent = 'Pause';
  $('play').setAttribute('aria-label', 'Pause trial frames');
  function advance() {
    if (sample === end) return stopPlayback();
    const ms = Math.max(36, (timeline[sample + 1].relative - timeline[sample].relative) * (1000 / 59.7275));
    timer = setTimeout(() => { chooseFrame(sample + 1); advance(); }, ms);
  }
  advance();
}

async function compareRun(id) {
  const token = ++compareToken;
  if (!id) {
    comparison = null;
    drawComparison();
    return;
  }
  try {
    const result = await json(resultPath(id) + '/result');
    if (token === compareToken && id !== primary?.id) {
      comparison = result;
      drawComparison();
    }
  } catch (error) {
    if (token === compareToken) setStatus(error.message, true);
  }
}

function drawComparison() {
  if (!comparison || !primary) {
    $('comparison').innerHTML = '';
    return;
  }
  const rows = [];
  for (const now of primary.trials) {
    const old = comparison.trials.find(t => t.id === now.id);
    if (!old) continue;
    for (const [i, field] of primary.fields.entries()) {
      const j = comparison.fields.findIndex(f => f.path === field.path);
      if (j < 0) continue;
      const before = old.snapshots.at(-1).fields[j].value, after = now.snapshots.at(-1).fields[i].value;
      rows.push(`<tr><th scope="row">${esc(now.label)}</th><td><code>${esc(field.path)}</code></td>
        <td>${showValue(before)}</td><td>${showValue(after)}</td></tr>`);
    }
  }
  $('comparison').innerHTML = rows.length ? `<table><thead><tr><th scope="col">Trial</th><th scope="col">Final sampled field</th>
    <th scope="col"><a href="#run/${comparison.id}">${comparison.id.slice(0, 7)}</a> · frame ${comparison.recipe.duration}</th>
    <th scope="col">Current · frame ${primary.recipe.duration}</th></tr></thead><tbody>${rows.join('')}</tbody></table>`
    : '<p class="section-head muted">These runs have no trial and field names in common.</p>';
}

function trialRow(trial) {
  const row = document.createElement('div');
  row.className = 'trial-form';
  row.dataset.id = trial.id;
  row.innerHTML = `<label>Trial label <input class="tlabel" maxlength="60" required value="${esc(trial.label)}"></label>
    <label>Controller input <textarea class="tinput" rows="4" spellcheck="false">${esc(trial.input)}</textarea></label>
    <details ${trial.interventions?.length ? 'open' : ''}><summary>Set a named field</summary>
      <label>Interventions (JSON) <textarea class="tinterventions" rows="4" spellcheck="false"
        placeholder='[{"frame":60,"field":"gEnginePauseFlags","value":4}]'>${esc(JSON.stringify(trial.interventions || [], null, 2))}</textarea></label>
    </details>
    <button type="button" class="remove-trial">Remove</button>`;
  return row;
}

function toggleTrialButtons() {
  const all = [...$('trial-editor').children];
  $('add-trial').disabled = all.length >= (catalog?.limits.trials || 4);
  all.forEach(row => { row.querySelector('.remove-trial').disabled = all.length <= 1; });
}

function setForm(spec) {
  draft = structuredClone(spec);
  $('recipe-title').value = spec.title;
  $('checkpoint').value = spec.checkpoint;
  $('checkpoint').disabled = Boolean(spec.fork);
  $('fork-origin').hidden = !spec.fork;
  $('fork-origin').textContent = spec.fork ?
    `Starting from ${spec.fork.trial}, frame ${spec.fork.frame}, in run ${spec.fork.run.slice(0, 7)}.` : '';
  $('duration').value = spec.duration;
  $('interval').value = spec.sample_every;
  $('resume-save').checked = spec.continue;
  $('sprite-trace').checked = Boolean(spec.sprite_trace);
  $('resume-save').disabled = !savedGame(spec.checkpoint) || Boolean(spec.fork);
  $('prep').value = spec.prepare;
  $('observe').value = spec.observe.join('\n');
  $('probe').value = spec.probe.join('\n');
  $('checks').value = JSON.stringify(spec.checks || [], null, 2);
  $('trial-editor').replaceChildren(...spec.trials.map(trialRow));
  $('editor-error').textContent = '';
  toggleTrialButtons();
}

function readLines(id) {
  return $(id).value.split('\n').map(line => line.trim()).filter(Boolean);
}

function formRecipe() {
  return { version: 1, title: $('recipe-title').value, checkpoint: draft.fork ? draft.checkpoint : $('checkpoint').value, fork: draft.fork,
    continue: $('resume-save').checked && !$('resume-save').disabled, rtc: draft.rtc,
    duration: Number($('duration').value), sample_every: Number($('interval').value),
    sprite_trace: $('sprite-trace').checked,
    prepare: $('prep').value, observe: readLines('observe'), probe: readLines('probe'),
    checks: JSON.parse($('checks').value || '[]'),
    trials: [...$('trial-editor').children].map(row => ({ id: row.dataset.id,
      label: row.querySelector('.tlabel').value, input: row.querySelector('.tinput').value,
      interventions: JSON.parse(row.querySelector('.tinterventions').value || '[]') })) };
}

function openEditor(spec = primary?.recipe || example) {
  if (!spec) return;
  setForm(spec);
  $('json-editor').value = '';
  $('editor').showModal();
}

function seedProbe() {
  if (!requestProbe || !/^[A-Za-z_]\w*$/.test(requestProbe)) return;
  const spec = structuredClone(example);
  if (!spec.probe.includes(requestProbe)) spec.probe.push(requestProbe);
  openEditor(spec);
}

$('run').onclick = () => submit(primary?.recipe || example);
$('edit').onclick = () => openEditor();
$('play').onclick = playback;
$('scrub').oninput = event => { stopPlayback(); chooseFrame(event.target.value); };
$('previous').onclick = () => { stopPlayback(); chooseFrame(sample - 1); };
$('snapshot-query').onsubmit = async event => {
  event.preventDefault();
  stopPlayback();
  const key = readbackKey, token = ++queryToken;
  const { trial, snap } = atSelected();
  const output = $('snapshot-value');
  output.value = 'Restoring the captured frame…';
  try {
    const url = resultPath(primary.id) + '/snapshot/' + trial.id + '/' + snap.relative;
    const { fields: rows, game_frame: gameFrame } = await json(url, [$('snapshot-field').value.trim()]);
    if (key !== readbackKey || token !== queryToken) return;
    const row = rows[0];
    output.value = row.resolved_address
      ? `${row.path} = ${showValue(row.value)} · RAM 0x${row.resolved_address} · game frame ${gameFrame}`
      : `${row.path} has no bound value at game frame ${gameFrame}`;
  } catch (error) {
    if (key === readbackKey && token === queryToken) output.value = error.message;
  }
};

$('fork-frame').onclick = () => {
  stopPlayback();
  const { trial, snap } = atSelected();
  const spec = {
    ...primary.recipe, title: `What changes after frame ${snap.relative}?`, continue: false,
    fork: { run: primary.id, trial: trial.id, frame: snap.relative }, prepare: '',
    checks: [],
    trials: [
      { id: 'control', label: 'No input', input: '' },
      { id: 'test', label: 'Try an input', input: 'tap A' },
    ],
  };
  openEditor(spec);
};
$('next').onclick = () => { stopPlayback(); chooseFrame(sample + 1); };
$('screens').onclick = event => {
  const image = event.target.closest('img[data-pixel-trial].inspectable');
  if (image) {
    const rect = image.getBoundingClientRect();
    const x = Math.max(0, Math.min(239, Math.floor((event.clientX - rect.left) * 240 / rect.width)));
    const y = Math.max(0, Math.min(159, Math.floor((event.clientY - rect.top) * 160 / rect.height)));
    inspectPixel(Number(image.dataset.pixelTrial), x, y);
    return;
  }
  const button = event.target.closest('[data-trial]');
  if (button) { selected.trial = Number(button.dataset.trial); renderFrame(); }
};
$('pixel-query').onsubmit = event => {
  event.preventDefault();
  inspectPixel(selected.trial, Number($('pixel-x').value), Number($('pixel-y').value));
};
$('pixel-results').onclick = event => {
  const replay = event.target.closest('[data-verify-slot]');
  if (replay) { testSprite(replay); return; }
  const field = event.target.closest('[data-pixel-field]');
  if (!field) return;
  selected = { trial: selected.trial, kind: 'field', index: Number(field.dataset.pixelField) };
  renderFrame();
  $('evidence').scrollIntoView({block: 'nearest'});
};
$('matrix').onclick = event => {
  const button = event.target.closest('[data-index]');
  if (!button) return;
  selected.trial = Number(button.dataset.trial);
  selected.index = Number(button.dataset.index);
  renderFrame();
  $('matrix').querySelector('.selected')?.focus();
};
$('claims').onclick = event => {
  const button = event.target.closest('[data-claim]');
  if (button) { stopPlayback(); chooseClaim(Number(button.dataset.claim)); }
};
$('fields-tab').onclick = () => { selected.kind = 'field'; selected.index = 0; renderFrame(); };
$('calls-tab').onclick = () => { selected.kind = 'probe'; selected.index = 0; renderFrame(); };
$('history-select').onchange = event => {
  if (event.target.value) location.hash = '#run/' + event.target.value;
};
$('compare').onchange = event => compareRun(event.target.value);
$('checkpoint').onchange = () => {
  $('resume-save').disabled = !savedGame($('checkpoint').value);
  $('resume-save').checked = !$('resume-save').disabled;
};
$('recipe-form').onsubmit = event => {
  event.preventDefault();
  try { submit(formRecipe()); } catch (error) { $('editor-error').textContent = error.message; }
};
$('close-editor').onclick = $('cancel-editor').onclick = () => $('editor').close();
$('add-trial').onclick = () => {
  const used = new Set([...$('trial-editor').children].map(node => node.dataset.id));
  let i = 1;
  while (used.has('trial-' + i)) ++i;
  const row = trialRow({ id: 'trial-' + i, label: 'Trial ' + i, input: '' });
  $('trial-editor').append(row);
  toggleTrialButtons();
  row.querySelector('input').focus();
};
$('trial-editor').onclick = event => {
  if (event.target.matches('.remove-trial')) {
    event.target.closest('.trial-form').remove();
    toggleTrialButtons();
  }
};
$('export-json').onclick = () => {
  try { $('json-editor').value = JSON.stringify(formRecipe(), null, 2); }
  catch (error) { $('editor-error').textContent = error.message; }
};
$('apply-json').onclick = () => {
  try {
    const spec = JSON.parse($('json-editor').value);
    if (spec.version !== 1 || !Array.isArray(spec.trials) || !Array.isArray(spec.observe) ||
        !Array.isArray(spec.probe) || !spec.trials.length) throw new Error('Use a version 1 recipe with trials, observed fields and probes.');
    setForm(spec);
  } catch (error) { $('editor-error').textContent = error.message; }
};
window.addEventListener('hashchange', () => {
  const route = splitHash();
  if (route) openRun(route);
});

async function openExample(id) {
  const token = ++loadToken;
  stopPlayback();
  try {
    const spec = await json(API + '/example/' + encodeURIComponent(id));
    if (token !== loadToken) return;
    example = spec;
    draft = spec;
    primary = null;
    comparison = null;
    running = false;
    $('preview').hidden = false;
    $('results').hidden = true;
    $('artifacts').innerHTML = '';
    setStatus('');
    header();
    drawPreview(spec);
    history.replaceState(null, '', '#example/' + id);
    await refreshHistory();
  } catch (error) {
    if (token === loadToken) setStatus(error.message, true);
  }
}

$('examples').onchange = event => {
  if (event.target.value) openExample(event.target.value);
};

async function start() {
  studioHeader();
  try {
    let examples;
    [example, catalog, examples] = await Promise.all([
      json(API + '/example'), json(API + '/catalog'), json(API + '/examples'),
    ]);
    $('examples').innerHTML = '<option value="">Example questions and recordings</option>' + examples.map(item =>
      `<option value="${esc(item.id)}">${esc(item.label)}</option>`).join('');
    $('checkpoint').innerHTML = catalog.checkpoints.map(c =>
      `<option value="${esc(c.id)}">${esc(c.label)}${c.scene == null ? '' : ' · scene ' + c.scene}</option>`).join('');
    header();
    drawPreview(example);
    if (requestProbe) seedProbe();
    const route = splitHash();
    if (route) await openRun(route);
    else if (location.hash.startsWith('#example/')) await openExample(location.hash.slice(9));
    else await refreshHistory();
  } catch (error) {
    setStatus(error.message, true);
  }
}
start();
