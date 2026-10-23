import type { EngineInterface, Register } from 'claude-code'

import type {
  DecompCategory,
  DecompFunction,
  DecompLevel,
  DecompReportUnit,
  DecompSnapshot,
  DecompSource,
  DecompTab,
} from '../types'
import {
  DECOMP_DEV_URL,
  PROGRESS_URL,
  PULLS_URL,
  RAW_URL,
  REPO,
  REPORT_URL,
  ago,
  attachUnitBytes,
  definitionHit,
  functionAt,
  parseProgress,
  parsePulls,
  parseReport,
  parseUnitBytes,
  shade,
  sourceNames,
  textBar,
  treemapSvg,
  type TreemapNode,
} from './data'

const PANE = 'decomp-progress'
const TITLE = 'Decomp progress'
const SOURCE_PANE = 'decomp-source'
const STALE_MS = 10 * 60 * 1000
/** Rows the treemap's list draws at most; the rest are summed in one line. */
const MAX_ROWS = 150

// What the popup draws. Module variables rather than $.state, which older
// builds (2.1.271) don't have; a reload starts over from the store.
let snapshot: DecompSnapshot | null = null
let status = ''
let tab: DecompTab = 'overview'
let showRegions = false
let level: DecompLevel = {}
// decomp.dev's report, with every function: fetched when the treemap is first
// drilled into, kept in memory only (it is a couple of megabytes).
let report: { units: DecompReportUnit[]; fetchedAt: number } | null = null
let reportStatus = ''
let source: DecompSource | null = null

function redraw($: EngineInterface): void {
  $.ui.invalidate('ui.render')
}
function setStatus($: EngineInterface, value: string): void {
  status = value
  redraw($)
}
function setTab($: EngineInterface, value: DecompTab): void {
  tab = value
  redraw($)
  if (value === 'treemap') void loadReport($)
}
function toggleRegions($: EngineInterface): void {
  showRegions = !showRegions
  redraw($)
}
function drill($: EngineInterface, value: DecompLevel): void {
  level = value
  tab = 'treemap'
  redraw($)
  void loadReport($)
}

const BLUE = '#3b74d9'
const PINNED = '#2a4f8f'
const GREEN = '#3f9b4f'
const TRACK = '#3a3a3a'

const HEADERS = { 'User-Agent': 'decomp-progress-mod', Accept: 'application/vnd.github+json' }

async function refresh($: EngineInterface, force = false): Promise<void> {
  const have = snapshot
  if (!force && have && Date.now() - Date.parse(have.fetchedAt) < STALE_MS) return
  setStatus($, 'Updating…')
  try {
    const [md, pulls, page] = await Promise.all([
      $.http.fetch(PROGRESS_URL, { headers: { 'User-Agent': HEADERS['User-Agent'] } }),
      $.http.fetch(PULLS_URL, { headers: HEADERS }),
      $.http.fetch(DECOMP_DEV_URL, { headers: { 'User-Agent': HEADERS['User-Agent'] } }).catch(() => null),
    ])
    if (!md.ok) throw new Error(`PROGRESS.md: HTTP ${md.status}`)
    const parsed = parseProgress(md.text)
    if (page?.ok) {
      try {
        attachUnitBytes(parsed.categories, parseUnitBytes(page.text))
      } catch {
        // Without decomp.dev's sizes the treemap sizes units by function count.
      }
    }
    const prs = pulls.ok ? parsePulls(JSON.parse(pulls.text)).slice(0, 25) : (have?.prs ?? [])
    const next: DecompSnapshot = { ...parsed, prs, fetchedAt: new Date().toISOString() }
    snapshot = next
    await $.store.set('snapshot', next)
    setStatus($, pulls.ok ? '' : `GitHub API: HTTP ${pulls.status}`)
  } catch (err) {
    setStatus($, `Couldn't update: ${err instanceof Error ? err.message : String(err)}`)
  }
  if (force && report) void loadReport($, true)
}

async function loadReport($: EngineInterface, force = false): Promise<void> {
  if (reportStatus === 'loading') return
  if (!force && report && Date.now() - report.fetchedAt < STALE_MS) return
  reportStatus = 'loading'
  redraw($)
  try {
    const res = await $.http.fetch(REPORT_URL, { headers: { 'User-Agent': HEADERS['User-Agent'] } })
    if (!res.ok) throw new Error(`HTTP ${res.status}`)
    report = { units: parseReport(JSON.parse(res.text)), fetchedAt: Date.now() }
    reportStatus = ''
  } catch (err) {
    reportStatus = `Couldn't load decomp.dev's report: ${err instanceof Error ? err.message : String(err)}`
  }
  redraw($)
}

async function openPopup($: EngineInterface): Promise<void> {
  await $.ui.open({ id: PANE, title: TITLE, focus: true, closeOnEscape: true, rows: 34, columns: 64 })
  void refresh($)
}

/** The function's C: from the session's checkout of the repository when it is one, else main on GitHub. */
async function findSource($: EngineInterface, fn: DecompFunction): Promise<Omit<DecompSource, 'fn' | 'unit'>> {
  const names = sourceNames(fn)
  const root = await $.session.root().catch(() => '')
  const isCheckout = root !== '' && (await $.fs.exists(`${root}/symbols/progress_categories.csv`).catch(() => false))
  if (isCheckout) {
    for (const name of names) {
      const path = `src/fn/${name}.c`
      if (await $.fs.exists(`${root}/${path}`)) {
        return { state: 'found', path, text: await $.fs.read(`${root}/${path}`), startLine: 1, from: 'checkout' }
      }
    }
    // Units of several functions live in one file under src/.
    const grep = await $.process
      .run(['git', 'grep', '-n', '-E', `^[A-Za-z_].*[^A-Za-z0-9_](${names.join('|')})[[:space:]]*\\(`, '--', 'src'], { cwd: root })
      .catch(() => null)
    const hit = grep && grep.exitCode === 0 ? definitionHit(grep.stdout) : undefined
    if (hit) {
      const found = functionAt(await $.fs.read(`${root}/${hit.path}`), hit.line)
      return { state: 'found', path: hit.path, ...found, from: 'checkout' }
    }
    return { state: 'missing' }
  }
  for (const name of names) {
    const path = `src/fn/${name}.c`
    const res = await $.http.fetch(`${RAW_URL}/${path}`, { headers: { 'User-Agent': HEADERS['User-Agent'] } }).catch(() => null)
    if (res?.ok) return { state: 'found', path, text: res.text, startLine: 1, from: 'github' }
  }
  return { state: 'missing' }
}

async function openSource($: EngineInterface, fn: DecompFunction, unit: string): Promise<void> {
  source = { fn, unit, state: 'loading' }
  redraw($)
  await $.ui.open({ id: SOURCE_PANE, title: fn.name, columns: 88 })
  try {
    const found = await findSource($, fn)
    if (source?.fn === fn) source = { fn, unit, ...found }
  } catch {
    if (source?.fn === fn) source = { fn, unit, state: 'missing' }
  }
  redraw($)
}

const fmt = (n: number) => n.toLocaleString('en-US')
const hex = (n: number) => n.toString(16).toUpperCase().padStart(8, '0')
/** A share matched, as a list row shows it: a tick when complete. */
const pctLabel = (pct: number) => (pct >= 100 ? '✓' : `${pct.toFixed(pct >= 99.95 ? 2 : 1)}%`)

/** The tiles of the treemap at `at`, with what a press on each does. */
function treemapLevel(snap: DecompSnapshot, at: DecompLevel) {
  const units: DecompReportUnit[] =
    report?.units ??
    snap.categories.flatMap(c =>
      c.units.map(u => ({ category: c.name, name: u.name, bytes: u.bytes ?? u.total * 200, pct: u.pct, done: u.done, total: u.total, functions: [] })),
    )
  const fnNode = (f: DecompFunction): TreemapNode => ({
    name: f.name,
    size: f.size,
    pct: f.pct,
    title: `${f.name} @ ${hex(f.addr)}: ${f.size} bytes, ${f.pct >= 100 ? 'matched' : `${f.pct.toFixed(1)}% fuzzy`}`,
  })
  const unitNode = (u: DecompReportUnit, withChildren: boolean): TreemapNode => ({
    name: u.name,
    size: u.bytes,
    pct: u.pct,
    title: `${u.category} / ${u.name}: ${u.pct.toFixed(1)}% of ${fmt(u.bytes)} bytes, ${u.done}/${u.total} functions`,
    children: withChildren ? u.functions.map(fnNode) : undefined,
  })

  if (at.category && at.unit) {
    const unit = units.find(u => u.category === at.category && u.name === at.unit)
    const fns = unit?.functions ?? []
    return {
      nodes: fns.map(fnNode),
      rows: fns.map(f => ({ name: f.name, bytes: f.size, pct: f.pct, detail: `${f.size} B`, fn: f })),
    }
  }
  if (at.category) {
    const inCat = units.filter(u => u.category === at.category)
    return {
      nodes: inCat.map(u => unitNode(u, true)),
      rows: inCat.map(u => ({ name: u.name, bytes: u.bytes, pct: u.pct, detail: `${u.done}/${u.total}`, go: { category: at.category, unit: u.name } })),
    }
  }
  const cats = snap.categories.map(c => {
    const inCat = units.filter(u => u.category === c.name)
    const bytes = inCat.reduce((s, u) => s + u.bytes, 0) || c.total * 200
    return { c, inCat, bytes }
  })
  return {
    nodes: cats.map(({ c, inCat, bytes }) => ({
      name: c.name,
      size: bytes,
      pct: c.pct,
      title: `${c.name}: ${c.pct}% of bytes, ${c.done}/${c.total} functions`,
      children: inCat.map(u => unitNode(u, false)),
    })),
    rows: cats.map(({ c, bytes }) => ({ name: c.name, bytes, pct: c.pct, detail: `${c.done}/${c.total}`, go: { category: c.name } })),
  }
}

export const register: Register = on => {
  on('session.start', async ($, e, next) => {
    await $.command.register({ name: 'decomp', description: 'Show Boktai 3 decomp progress in a popup' })
    if (!snapshot) {
      const kept = (await $.store.get('snapshot')) as DecompSnapshot | undefined
      if (kept?.fetchedAt) snapshot = kept
    }
    void refresh($)
    $.clock.every(STALE_MS, () => void refresh($))

    return next(e)
  })

  on('command.run', { command: 'decomp' }, async $ => {
    await openPopup($)

    return { text: 'Decomp progress opened.' }
  })

  // The button beside the footer's mode labels, at the right of the prompt.
  on('ui.render', { component: 'SessionMode' }, async ($, e, next) => {
    const engine = await next(e)
    const { Box, Button } = $.ui.resolve(e)
    const snap = snapshot
    const label = snap ? `◔ ${snap.pct.toFixed(2)}%` : '◔ decomp'

    return (
      <Box flexDirection="row" gap={1} alignItems="center">
        {engine}
        <Button key="decomp-progress-open" plain label={label} onPress={() => openPopup($)} />
      </Box>
    )
  })

  // The function a treemap row opened, beside the transcript.
  on('ui.render', { component: 'Pane', requestId: SOURCE_PANE }, async ($, e) => {
    const { Box, Text, Code, Link } = $.ui.resolve(e)
    const src = source
    if (!src) return <Text dimColor>Pick a function in the decomp treemap.</Text>
    const { fn } = src
    const state = fn.pct >= 100 ? 'matched' : src.state === 'found' ? `draft · ${fn.pct.toFixed(1)}% fuzzy` : `not decompiled · ${fn.pct.toFixed(1)}% fuzzy`
    const blob = src.path ? `https://github.com/${REPO}/blob/main/${src.path}${src.startLine && src.startLine > 1 ? `#L${src.startLine}` : ''}` : ''

    return (
      <Box flexDirection="column" gap={1}>
        <Box flexDirection="column">
          <Box flexDirection="row" justifyContent="space-between" gap={1}>
            <Text bold>{fn.name}</Text>
            <Text color={fn.pct >= 100 ? GREEN : undefined} dimColor={fn.pct < 100}>
              {state}
            </Text>
          </Box>
          <Text dimColor>
            {`${src.unit} · ${hex(fn.addr)} · ${fn.size} bytes${src.path ? ` · ${src.path}${src.from === 'github' ? ' (main)' : ''}` : ''}`}
          </Text>
        </Box>
        {src.state === 'loading' && <Text dimColor>Finding the source…</Text>}
        {src.state === 'missing' && (
          <Text dimColor>No C for this function yet{fn.pct >= 100 ? ' was found' : ''}.</Text>
        )}
        {src.state === 'found' && src.text !== undefined && (
          <Code key="source" source={src.text.slice(0, 90000)} language="c" startLine={src.startLine ?? 1} />
        )}
        <Box flexDirection="row" gap={2}>
          {blob ? <Link href={blob} label="GitHub →" /> : null}
          <Link href={`https://github.com/search?q=${encodeURIComponent(`repo:${REPO} ${fn.name}`)}&type=code`} label="Search →" />
        </Box>
      </Box>
    )
  })

  on('ui.render', { component: 'Pane', requestId: PANE }, async ($, e) => {
    const el = $.ui.resolve(e)
    const { Box, Text, Button, Link } = el
    const Svg = e.surface !== 'terminal' && 'Svg' in el ? el.Svg : undefined
    const snap = snapshot
    const note = status
    const current = tab
    const at = level
    const cols = Math.max(30, e.props.bodyColumns)
    const px = Math.max(260, Math.round(cols * 7.2))
    const now = Date.now()

    // A thin bar: SVG where the surface draws it, block characters elsewhere.
    const bar = (parts: { pct: number; color: string }[], key: string) => {
      if (Svg) {
        let x = 0
        const rects = parts
          .map(p => {
            const w = Math.max(0, Math.min(100 - x, p.pct))
            const r = `<rect x="${x}" y="0" width="${w}" height="4" fill="${p.color}"/>`
            x += w
            return r
          })
          .join('')
        const source =
          `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 4" preserveAspectRatio="none" width="${px}" height="4">` +
          `<rect width="100" height="4" rx="2" fill="${TRACK}"/>${rects}</svg>`
        const total = parts.reduce((s, p) => s + p.pct, 0)
        return <Svg key={key} source={source} alt={`${total.toFixed(1)}%`} width={px} height={4} />
      }
      return <Text color={parts[0]?.color ?? BLUE}>{textBar(parts.reduce((s, p) => s + p.pct, 0), Math.max(10, cols - 2))}</Text>
    }

    // Lines a row shows only while the pointer is on it.
    const details = (lines: string[]) => (
      <Box flexDirection="column" display="none" hover={{ display: 'flex' }} paddingLeft={2}>
        {lines.map(l => (
          <Text dimColor>{l}</Text>
        ))}
      </Box>
    )

    const header = (
      <Box flexDirection="row" justifyContent="space-between" alignItems="center">
        <Text dimColor>Boktai 3 decomp</Text>
        <Box flexDirection="row" gap={1} alignItems="center">
          <Text dimColor>{note || (snap ? `updated ${ago(snap.fetchedAt, now)}` : '')}</Text>
          <Button key="refresh" plain label="↻" onPress={() => refresh($, true)} />
        </Box>
      </Box>
    )

    const tabs = (
      <Box flexDirection="row" gap={1}>
        {(['overview', 'treemap', 'recent'] as DecompTab[]).map(t => (
          <Button
            key={`tab-${t}`}
            label={t === 'overview' ? 'Overview' : t === 'treemap' ? 'Treemap' : 'Recent PRs'}
            variant={t === current ? 'primary' : 'secondary'}
            onPress={() => setTab($, t)}
          />
        ))}
      </Box>
    )

    if (!snap) {
      return (
        <Box flexDirection="column" gap={1}>
          {header}
          <Text dimColor>{note || 'Loading PROGRESS.md…'}</Text>
        </Box>
      )
    }

    const categoryRow = (c: DecompCategory) => (
      <Box key={`row-${c.name}`} flexDirection="column">
        <Box flexDirection="row" justifyContent="space-between">
          <Button key={`cat-${c.name}`} plain label={c.name} onPress={() => drill($, { category: c.name })} />
          <Text color={c.pct >= 100 ? GREEN : undefined} dimColor={c.pct < 100}>
            {pctLabel(c.pct)}
          </Text>
        </Box>
        {bar([{ pct: c.pct, color: c.pct >= 100 ? GREEN : BLUE }], `bar-${c.name}`)}
        {details([
          `${c.done}/${c.total} functions${c.left ? ` · ${c.left} left` : ''}${c.near ? ` · ${c.near} ${c.near === 1 ? 'draft' : 'drafts'} ≥95%` : ''}`,
          `${c.units.length} units · click to open in the treemap`,
        ])}
      </Box>
    )

    let body
    if (current === 'overview') {
      const clean = snap.cleanPct
      const pinned = Math.max(0, snap.pct - clean)
      body = (
        <Box flexDirection="column" gap={1}>
          <Box key="headline" flexDirection="column">
            <Box flexDirection="row" justifyContent="space-between" alignItems="flex-end">
              <Text bold size="title">{`${snap.pct.toFixed(2)}%`}</Text>
              <Text dimColor>{`${fmt(snap.fnDone)} / ${fmt(snap.fnTotal)} functions`}</Text>
            </Box>
            {bar(
              [
                { pct: clean, color: BLUE },
                { pct: pinned, color: PINNED },
              ],
              'bar-headline',
            )}
            {details([
              `Code matched in C, by bytes: ${fmt(snap.bytesDone)} / ${fmt(snap.bytesTotal)}`,
              `Without register pinning ${clean.toFixed(2)}% · pinned ${snap.pinnedPct.toFixed(2)}% (darker)`,
              `Nonmatching drafts: ${snap.drafts.count} functions, ${snap.drafts.pct.toFixed(1)}% of code, fuzzy ${snap.drafts.fuzzy}%`,
              `Intentional assembly, not counted: ${fmt(snap.asmBytes)} bytes`,
            ])}
          </Box>
          {snap.sections.map(s => (
            <Box flexDirection="column" gap={1}>
              <Box flexDirection="row" justifyContent="space-between">
                <Text dimColor bold>
                  {s.name}
                </Text>
                <Text dimColor>{pctLabel(s.pct)}</Text>
              </Box>
              {snap.categories.filter(c => c.section === s.name).map(categoryRow)}
            </Box>
          ))}
          <Box flexDirection="column">
            <Button key="regions" plain dimColor label={`${showRegions ? '▾' : '▸'} ROM regions`} onPress={() => toggleRegions($)} />
            {showRegions &&
              snap.regions.map(r => (
                <Box flexDirection="column" paddingLeft={2}>
                  <Box flexDirection="row" justifyContent="space-between">
                    <Text dimColor>{r.name}</Text>
                    <Text dimColor>{`${r.done}/${r.total} · ${pctLabel(r.pct)}`}</Text>
                  </Box>
                  {bar([{ pct: r.pct, color: r.pct >= 100 ? GREEN : BLUE }], `bar-${r.name}`)}
                </Box>
              ))}
          </Box>
        </Box>
      )
    } else if (current === 'treemap') {
      const height = Math.round(px * (at.unit ? 0.5 : 0.72))
      const { nodes, rows } = treemapLevel(snap, at)
      const needsReport = at.unit !== undefined && !report
      // Most left to match first, then the complete ones.
      const sorted = [...rows].sort((a, b) => b.bytes * (100 - b.pct) - a.bytes * (100 - a.pct) || a.name.localeCompare(b.name))
      const shown = sorted.slice(0, MAX_ROWS)
      const crumbs = (
        <Box flexDirection="row" gap={1} flexWrap="wrap">
          <Button key="crumb-root" plain dimColor={!!at.category} label="All" onPress={() => drill($, {})} />
          {at.category && <Text dimColor>›</Text>}
          {at.category && (
            <Button key="crumb-category" plain dimColor={!!at.unit} label={at.category} onPress={() => drill($, { category: at.category })} />
          )}
          {at.unit && <Text dimColor>›</Text>}
          {at.unit && <Text>{at.unit}</Text>}
        </Box>
      )
      body = (
        <Box flexDirection="column" gap={1}>
          {crumbs}
          {needsReport ? (
            <Text dimColor>{reportStatus && reportStatus !== 'loading' ? reportStatus : 'Loading functions from decomp.dev…'}</Text>
          ) : Svg ? (
            <Svg
              key={`treemap-${at.category ?? ''}-${at.unit ?? ''}`}
              source={treemapSvg(nodes, px, height)}
              alt="Treemap sized by code bytes and shaded by bytes matched"
              width={px}
              height={height}
              isInteractive
            />
          ) : null}
          {reportStatus && reportStatus !== 'loading' && !needsReport && <Text dimColor>{reportStatus}</Text>}
          {!needsReport && (
            <Box flexDirection="column">
              {shown.map(r => (
                <Box key={`item-${r.name}`} flexDirection="row" justifyContent="space-between" gap={1}>
                  <Button
                    key={`go-${r.name}`}
                    plain
                    dimColor={r.pct >= 100}
                    label={`${Svg ? '' : `${textBar(r.pct, 6)} `}${r.name}`}
                    onPress={() => ('fn' in r && r.fn ? openSource($, r.fn, `${at.category} / ${at.unit}`) : 'go' in r && r.go ? drill($, r.go) : undefined)}
                  />
                  <Text color={r.pct >= 100 ? GREEN : shade(r.pct)}>{`${r.detail} · ${pctLabel(r.pct)}`}</Text>
                </Box>
              ))}
              {sorted.length > shown.length && <Text dimColor>{`… ${sorted.length - shown.length} more, all complete or small`}</Text>}
            </Box>
          )}
          <Text dimColor>
            {at.unit
              ? 'Tiles are functions. Click one in the list to open its C beside the chat.'
              : 'Sized by code bytes, blue deepens with bytes matched, green is complete. Hover a tile for numbers; click a row to go in.'}
          </Text>
        </Box>
      )
    } else {
      const dayAgo = now - 24 * 3600 * 1000
      const today = snap.prs.filter(p => Date.parse(p.mergedAt) >= dayAgo)
      const matches = today.reduce((s, p) => s + (p.matches ?? 0), 0)
      const withPct = snap.prs.filter(p => p.progress !== undefined)
      body = (
        <Box flexDirection="column" gap={1}>
          <Box flexDirection="row" justifyContent="space-between">
            <Text bold>Last 24 hours</Text>
            <Text dimColor>{`${today.length} PRs · ${matches} matches`}</Text>
          </Box>
          <Box flexDirection="column">
            {snap.prs.map(p => {
              const older = withPct.find(o => o.number !== p.number && Date.parse(o.mergedAt) < Date.parse(p.mergedAt))
              const delta = p.progress !== undefined && older?.progress !== undefined ? p.progress - older.progress : undefined
              const right = [
                p.matches ? `+${p.matches}` : '',
                p.progress !== undefined ? `${p.progress.toFixed(2)}%${delta !== undefined && delta > 0 ? ` (+${delta.toFixed(2)})` : ''}` : '',
                ago(p.mergedAt, now),
              ]
                .filter(Boolean)
                .join(' · ')
              return (
                <Box flexDirection="row" justifyContent="space-between" gap={1}>
                  <Box flexDirection="row" gap={1} flexShrink={1}>
                    <Text color={p.matches ? GREEN : '#777777'}>●</Text>
                    <Link href={`https://github.com/${REPO}/pull/${p.number}`} label={`#${p.number} ${p.title}`} />
                  </Box>
                  <Text dimColor>{right}</Text>
                </Box>
              )
            })}
          </Box>
        </Box>
      )
    }

    return (
      <Box flexDirection="column" gap={1}>
        {header}
        {tabs}
        {body}
        <Box flexDirection="row" justifyContent="space-between">
          <Link href={`${DECOMP_DEV_URL}`} label="decomp.dev →" />
          <Link href={`https://github.com/${REPO}/blob/main/PROGRESS.md`} label="PROGRESS.md →" />
        </Box>
      </Box>
    )
  })
}
