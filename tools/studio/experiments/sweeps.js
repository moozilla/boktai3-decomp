'use strict';

const sweepAPI = API + '/sweeps';
const sweepMarks = { confirmed: '✓', contradicted: '×', unresolved: '?' };
let sweepRecipe;

function sweepId() {
  return /^#sweep\/([a-f0-9]{32})$/.exec(location.hash)?.[1];
}

async function refreshSweepHistory() {
  const rows = await json(sweepAPI);
  const select = $('sweep-history');
  select.hidden = $('sweep-history-label').hidden = rows.length === 0;
  select.innerHTML = '<option value="">Choose a sweep</option>' + rows.map(row =>
    `<option value="${row.id}">${esc(row.title)} · ${row.done ?? 0}/${row.total ?? '?'} · ${row.id.slice(0, 7)}</option>`
  ).join('');
  select.value = sweepId() || '';
}

function openSweepDialog() {
  const spec = primary?.recipe || example;
  if (!spec || spec.fork || !catalog) return;
  sweepRecipe = spec;
  $('sweep-question').textContent = spec.title;
  $('sweep-choices').innerHTML = catalog.checkpoints.map(checkpoint => `<label class="checkbox">
    <input type="checkbox" value="${esc(checkpoint.id)}" data-kind="${esc(checkpoint.kind)}"${checkpoint.id === spec.checkpoint ? ' checked' : ''}>
    <span>${esc(checkpoint.label)} <code>${esc(checkpoint.id)}</code></span>
  </label>`).join('');
  $('sweep-all').disabled = !catalog.checkpoints.some(checkpoint => checkpoint.kind === 'bank');
  $('sweep-error').textContent = '';
  $('sweep-dialog').showModal();
}

function selectCheckpoints(bank) {
  for (const box of $('sweep-choices').querySelectorAll('input')) box.checked = bank && box.dataset.kind === 'bank';
}

async function queueSweep(event) {
  event.preventDefault();
  const checkpoints = [...$('sweep-choices').querySelectorAll('input:checked')].map(input => input.value);
  if (!checkpoints.length) { $('sweep-error').textContent = 'Choose at least one checkpoint.'; return; }
  $('sweep-submit').disabled = true;
  try {
    const {id} = await json(sweepAPI, {recipe: sweepRecipe, checkpoints});
    $('sweep-dialog').close();
    location.hash = '#sweep/' + id;
  } catch (error) {
    $('sweep-error').textContent = error.message;
  } finally {
    $('sweep-submit').disabled = false;
  }
}

function drawSweep(report, id) {
  const {status, checkpoints, recipe: spec} = report;
  const claims = spec.checks || [];
  const header = claims.length ? claims.map(check => `<th scope="col">${esc(check.statement)}</th>`).join('')
    : '<th scope="col">Run</th>';
  const rows = checkpoints.map(checkpoint => {
    const url = '#run/' + checkpoint.run;
    const startingFrame = checkpoint.state === 'complete'
      ? `<a href="${url}/0/${checkpoint.trial}"><img class="sweep-thumb" loading="lazy" width="96" height="64"
        src="${artifact(checkpoint.trial + '/f0000.png', checkpoint.run)}" alt="View starting frame"></a>` : '';
    let cells;
    if (checkpoint.state !== 'complete') {
      cells = `<td colspan="${Math.max(claims.length, 1)}"><a href="${url}">Run failed</a> · ${esc(checkpoint.error)}</td>`;
    } else if (claims.length === 0) {
      cells = `<td><a href="${url}">Open recorded evidence</a></td>`;
    } else {
      cells = checkpoint.checks.map(check => {
        const condition = unmetRequirement(check);
        const evidence = condition || check;
        const {kind, index} = evidence.evidence;
        const target = `${url}/${evidence.to}/${evidence.trial}/${kind}/${index}`;
        return `<td><a class="sweep-verdict" href="${target}">
          <span class="claim-mark ${esc(check.state)}">${sweepMarks[check.state]}</span>
          ${esc(check.state)}<span class="claim-math">${condition
            ? esc(describeRequirement(condition)) : showValue(check.measurement) + ' ' + esc(check.sign) + ' ' + showValue(check.value)}</span>
        </a></td>`;
      }).join('');
    }
    return `<tr><td>${startingFrame}</td><th scope="row"><a href="${url}">${esc(checkpoint.label)}</a>
      <span class="claim-math">${esc(checkpoint.id)}${checkpoint.game_frame == null ? '' : ' · frame ' + checkpoint.game_frame}</span>
      </th>${cells}</tr>`;
  }).join('');
  const completed = checkpoints.length;
  $('sweep-progress').textContent = `${completed} of ${report.choices.length} checkpoints · ${status.message}`;
  $('sweep-export').href = sweepAPI + '/' + id + '/answers.tsv';
  $('sweep-contact').hidden = status.state !== 'complete';
  if (status.state === 'complete') $('sweep-contact').href = sweepAPI + '/' + id + '/checkpoints.png';
  $('sweep-ledger').innerHTML = `<table><thead><tr><th scope="col">Starting frame</th><th scope="col">Checkpoint</th>
    ${header}</tr></thead><tbody>${rows}</tbody></table>`;
}

async function openSweep() {
  const id = sweepId();
  if (!id) return;
  const token = ++loadToken;
  stopPlayback();
  running = false;
  $('preview').hidden = true;
  $('results').hidden = true;
  $('sweep-results').hidden = false;
  $('sweep-progress').textContent = 'Opening checkpoint results…';
  $('sweep-ledger').innerHTML = '';
  $('sweep-export').removeAttribute('href');
  $('sweep-contact').hidden = true;
  $('artifacts').innerHTML = '';
  try {
    for (;;) {
      const report = await json(sweepAPI + '/' + id);
      if (token !== loadToken || sweepId() !== id) return;
      primary = null;
      comparison = null;
      example = draft = report.recipe;
      header();
      $('question-context').textContent = 'The same recipe run from each selected checkpoint.';
      setStatus('');
      drawSweep(report, id);
      if (!['queued', 'running'].includes(report.status.state)) break;
      await pause(1400);
    }
    await refreshSweepHistory();
  } catch (error) {
    if (token === loadToken) $('sweep-progress').textContent = error.message;
  }
}

$('sweep-open').onclick = openSweepDialog;
$('sweep-form').onsubmit = queueSweep;
$('sweep-close').onclick = () => $('sweep-dialog').close();
$('sweep-all').onclick = () => selectCheckpoints(true);
$('sweep-clear').onclick = () => selectCheckpoints(false);
$('sweep-history').onchange = event => { if (event.target.value) location.hash = '#sweep/' + event.target.value; };
window.addEventListener('hashchange', openSweep);
refreshSweepHistory().catch(error => { console.error(error); });
openSweep();
