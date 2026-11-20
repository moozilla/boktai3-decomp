'use strict';

const rechargeEndpoint = API + '/recharge';
const rechargeTitle = 'How much faster does holding A recharge Django?';
let rechargeToken = 0;

function putRecharge(report) {
  const {status, result} = report;
  const active = ['queued', 'running'].includes(status.state);
  $('recharge-run').disabled = active;
  $('recharge-progress').textContent = active ? status.message + ` · ${status.done} of 11 complete` : status.message;
  const complete = status.state === 'complete';
  const full = complete && result?.rows.find(row => row.bars === 10);
  $('recharge-finding').textContent = full
    ? `Full SUN restored ${full.cells.hold.gain} ENE with A held and ${full.cells.ambient.gain} passively`
    : 'Energy gained over 240 frames';
  if (!result?.rows.length) {
    $('recharge-ledger').innerHTML = '';
    $('recharge-links').innerHTML = '';
    return active;
  }
  const bundle = rechargeEndpoint + '/' + result.id;
  $('recharge-ledger').innerHTML = `<table>
    <thead><tr><th scope="col">SUN bars</th><th scope="col">Intensity range</th>
      <th scope="col">Outside coverage</th><th scope="col">Passive</th><th scope="col">Hold A</th></tr></thead>
    <tbody>${result.rows.map(row => {
      const [lower, upper] = row.bounds;
      return `<tr><th scope="row"><a href="${esc(row.gauge_url)}">${row.bars}</a></th>
        <td><a href="${bundle}/bounds/${row.bars}" title="ROM words at ${lower.address} and ${upper.address}">${lower.value}–${upper.value}</a></td>
        ${Object.keys(result.arms).map(arm => {
          const cell = row.cells[arm];
          return `<td><a href="${esc(cell.url)}">+${cell.gain} ENE</a>${cell.writers.slice(0, 1).map(writer =>
            writer.function ? ` <a href="${esc(writer.function.url)}" class="claim-math" data-peek>Writer</a>` : '').join('')}</td>`;
        }).join('')}</tr>`;
    }).join('')}</tbody></table>`;
  const records = complete ? [
    ['contact.png', 'Captured frames'], ['recharge.tsv', 'TSV table'],
    ['tool-transcript.jsonl', 'Tool transcript'],
  ].map(([name, label]) => `<a href="${bundle}/${name}" target="_blank" rel="noopener">${label}</a>`).join('') : '';
  $('recharge-links').innerHTML = `<span>Calibrated baseline ${result.baseline}. Both recharge trials enable area coverage.</span>
    <a href="${esc(result.source.accumulator.function.url)}" data-peek>Recharge calculation</a>
    <a href="${esc(result.source.gauge.function.url)}" data-peek>Gauge calculation</a>
    <a href="${esc(result.tables.lower.url)}" data-peek>Lower bounds</a>
    <a href="${esc(result.tables.upper.url)}" data-peek>Upper bounds</a>${records}`;
  return active;
}

async function openRecharge() {
  if (!isRechargePage()) return;
  const token = ++rechargeToken;
  ++loadToken;
  stopPlayback();
  setStatus('');
  $('preview').hidden = true;
  $('results').hidden = true;
  $('sweep-results').hidden = true;
  $('recharge-study').hidden = false;
  $('parent-run').hidden = true;
  $('artifacts').innerHTML = '';
  $('title').textContent = rechargeTitle;
  $('question-context').textContent = 'Same save and light, three trials at every SUN level. Select a measurement to inspect its run.';
  header();
  pageTitle(rechargeTitle, 'Experiments');
  try {
    while (isRechargePage() && token === rechargeToken) {
      const report = await json(rechargeEndpoint);
      if (!isRechargePage() || token !== rechargeToken) return;
      if (!putRecharge(report)) break;
      await pause(1200);
    }
  } catch (error) {
    if (token === rechargeToken && isRechargePage()) $('recharge-progress').textContent = error.message;
  }
}

$('recharge-run').onclick = async () => {
  $('recharge-run').disabled = true;
  try {
    await json(rechargeEndpoint, {});
    await openRecharge();
  } catch (error) {
    $('recharge-progress').textContent = error.message;
    $('recharge-run').disabled = false;
  }
};
window.addEventListener('hashchange', openRecharge);
openRecharge();
