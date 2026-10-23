import type { EngineInterface, Register, RenderChildren } from 'claude-code'

import type { DecompNode, DecompSnapshot, DecompTab, DecompTree } from '../types'
import {
  DECOMP_DEV_URL,
  PROGRESS_URL,
  PULLS_URL,
  REPO,
  TREE_URL,
  ago,
  blobUrl,
  extractFunction,
  fnShade,
  parseProgress,
  parsePulls,
  pctOf,
  rawUrl,
  shade,
  stripLayout,
  totalOf,
  treeLevel,
} from './data'

const PANE = 'decomp-progress'
const SOURCE = 'decomp-source'
const STALE_MS = 10 * 60 * 1000

// What the panes draw. Module variables rather than $.state, which older
// builds (2.1.271) don't have; a reload starts over from the store.
let snapshot: DecompSnapshot | null = null
let tree: DecompTree | null = null
let status = ''
let tab: DecompTab = 'overview'
/** Where the treemap is: [], [category] or [category, unit]. */
let zoom: string[] = []
type Source = {
  name: string
  where: string[]
  node: DecompNode
  state: 'loading' | 'ready' | 'none' | 'error'
  text?: string
  line?: number
  isWhole?: boolean
  showWhole?: boolean
  file?: string
  whole?: string
  error?: string
}
let source: Source | null = null

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
}
function zoomTo($: EngineInterface, path: string[]): void {
  zoom = path
  tab = 'treemap'
  redraw($)
}
function resetView($: EngineInterface): void {
  zoom = []
  tab = 'overview'
  redraw($)
}
function toggleWhole($: EngineInterface): void {
  if (source) source = { ...source, showWhole: !source.showWhole }
  redraw($)
}

const BLUE = '#3b74d9'
const PINNED = '#2a4f8f'
const TRACK = '#3a3a3a'
const UA = { 'User-Agent': 'decomp-progress-mod' }

async function refresh($: EngineInterface, force = false): Promise<void> {
  const have = snapshot
  if (!force && have && tree && Date.now() - Date.parse(have.fetchedAt) < STALE_MS) return
  setStatus($, 'Updating…')
  try {
    const [md, pulls, json] = await Promise.all([
      $.http.fetch(PROGRESS_URL, { headers: UA }),
      $.http.fetch(PULLS_URL, { headers: { ...UA, Accept: 'application/vnd.github+json' } }),
      $.http.fetch(TREE_URL, { headers: UA }),
    ])
    if (!md.ok) throw new Error(`PROGRESS.md: HTTP ${md.status}`)
    const prs = pulls.ok ? parsePulls(JSON.parse(pulls.text)).slice(0, 25) : (have?.prs ?? [])
    snapshot = { ...parseProgress(md.text), prs, fetchedAt: new Date().toISOString() }
    if (json.ok) tree = JSON.parse(json.text) as DecompTree
    await $.store.set('snapshot', snapshot)
    if (tree) await $.store.set('tree', tree)
    setStatus($, !pulls.ok ? `GitHub API: HTTP ${pulls.status}` : !tree ? 'No PROGRESS.json on main yet' : '')
  } catch (err) {
    setStatus($, `Couldn't update: ${err instanceof Error ? err.message : String(err)}`)
  }
}

async function openPopup($: EngineInterface): Promise<void> {
  await $.ui.open({ id: PANE, title: 'Decomp progress', focus: true, closeOnEscape: true, rows: 34, columns: 64 })
  void refresh($)
}

/** Opens the source pane on one function and fetches its C unit from main. */
async function openSource($: EngineInterface, node: DecompNode, where: string[]): Promise<void> {
  const fn = node.fn
  if (!fn) return
  source = { name: node.name, where, node, state: fn.file ? 'loading' : 'none', file: fn.file ?? undefined }
  await $.ui.open({ id: SOURCE, title: node.name, focus: true, closeOnEscape: true, rows: 40, columns: 90 })
  redraw($)
  if (!fn.file) return
  try {
    const res = await $.http.fetch(rawUrl(fn.file), { headers: UA })
    if (!res.ok) throw new Error(`HTTP ${res.status}`)
    const found = extractFunction(res.text, [node.name, ...(fn.cName ? [fn.cName] : []), `sub_${fn.address}`])
    if (source?.name !== node.name) return
    source = { ...source, state: 'ready', text: found.source, line: found.line, isWhole: found.isWhole, whole: res.text }
  } catch (err) {
    if (source?.name !== node.name) return
    source = { ...source, state: 'error', error: err instanceof Error ? err.message : String(err) }
  }
  redraw($)
}

const fmt = (n: number) => n.toLocaleString('en-US')
/** A percent share as flexGrow, which surfaces take only up to 10000. */
const grow = (p: number) => Math.max(0.01, Math.round(p * 100) / 100)
/** A share as a Box size: whole percent steps are all a surface takes. */
const share = (p: number) => `${Math.max(0, Math.min(100, Math.round(p)))}%`
const pct1 = (p: number) => `${p.toFixed(p >= 100 || p === 0 ? 0 : 1)}%`

function describe(n: DecompNode): string {
  if (n.fn) {
    const f = n.fn
    const state =
      f.pct >= 100 ? (f.pinned ? 'matched, register-pinned' : 'matched') : f.pct > 0 ? `draft at ${f.pct}%` : 'not started'
    return `${state} · ${fmt(n.bytes)} bytes · 0x${f.address}`
  }
  const left = n.total - n.done
  return `${fmt(n.done)}/${fmt(n.total)} functions · ${pct1(pctOf(n))} of ${fmt(n.bytes)} bytes${left ? ` · ${left} left` : ''}${n.near ? ` · ${n.near} drafts ≥95%` : ''}`
}

export const register: Register = on => {
  on('session.start', async ($, e, next) => {
    await $.command.register({ name: 'decomp', description: 'Show Boktai 3 decomp progress in a popup' })
    if (!snapshot) {
      const kept = (await $.store.get('snapshot')) as DecompSnapshot | undefined
      if (kept?.fetchedAt) snapshot = kept
      const keptTree = (await $.store.get('tree')) as DecompTree | undefined
      if (keptTree?.categories) tree = keptTree
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
    const label = snapshot ? `◔ ${snapshot.pct.toFixed(2)}%` : '◔ decomp'

    return (
      <Box flexDirection="row" gap={1} alignItems="center">
        {engine}
        <Button key="decomp-progress-open" plain label={label} onPress={() => openPopup($)} />
      </Box>
    )
  })

  on('ui.render', { component: 'Pane', requestId: PANE }, async ($, e) => {
    const { Box, Text, Button, Link } = $.ui.resolve(e)
    const snap = snapshot
    const cols = Math.max(30, e.props.bodyColumns)
    const now = Date.now()

    const bar = (parts: { pct: number; color: string }[]) => {
      let used = 0
      return (
        <Box flexDirection="row" height={1} width="100%" backgroundColor={TRACK}>
          {parts.map(p => {
            const w = Math.max(0, Math.min(100 - used, p.pct))
            used += w
            return <Box width={share(w)} height={1} backgroundColor={p.color} />
          })}
        </Box>
      )
    }
    const spread = (left: RenderChildren, right: RenderChildren) => (
      <Box flexDirection="row" justifyContent="space-between">
        {left}
        {right}
      </Box>
    )

    const header = spread(
      <Text bold>Boktai 3 decomp</Text>,
      <Box flexDirection="row" gap={1}>
        <Text dimColor>{status || (snap ? ago(snap.fetchedAt, now) : '')}</Text>
        <Button key="refresh" plain label="↻" onPress={() => refresh($, true)} />
      </Box>,
    )
    const tabs = (
      <Box flexDirection="row" gap={1}>
        {(['overview', 'treemap', 'recent'] as DecompTab[]).map(t => (
          <Button
            key={`tab-${t}`}
            label={t === 'overview' ? 'Overview' : t === 'treemap' ? 'Treemap' : 'Recent PRs'}
            variant={t === tab ? 'primary' : 'secondary'}
            onPress={() => setTab($, t)}
          />
        ))}
      </Box>
    )

    if (!snap) {
      return (
        <Box flexDirection="column" gap={1}>
          {header}
          <Text dimColor>{status || 'Loading PROGRESS.md…'}</Text>
        </Box>
      )
    }

    let body
    if (tab === 'overview') {
      const pinned = Math.max(0, snap.pct - snap.cleanPct)
      body = (
        <Box flexDirection="column" gap={1}>
          <Box flexDirection="column">
            {spread(<Text>Code matched in C</Text>, <Text bold>{snap.pct.toFixed(2)}%</Text>)}
            {bar([
              { pct: snap.cleanPct, color: BLUE },
              { pct: pinned, color: PINNED },
            ])}
            <Text dimColor wrap="truncate">
              {`${fmt(snap.fnDone)} of ${fmt(snap.fnTotal)} functions · ${snap.cleanPct.toFixed(1)}% without pins · ${snap.drafts.count} drafts`}
            </Text>
          </Box>
          <Box flexDirection="column">
            {snap.categories.map(c => (
              <Box flexDirection="column" marginTop={1}>
                {spread(
                  <Button key={`cat-${c.name}`} plain label={c.name} onPress={() => zoomTo($, [c.name])} />,
                  <Text dimColor>{c.pct >= 100 ? '✓ 100%' : `${c.pct.toFixed(1)}%`}</Text>,
                )}
                {bar([{ pct: c.pct, color: c.pct >= 100 ? shade(100) : BLUE }])}
              </Box>
            ))}
          </Box>
          <Text dimColor>Click a category to open it in the treemap.</Text>
        </Box>
      )
    } else if (tab === 'treemap') {
      if (!tree) {
        body = <Text dimColor>{status || 'Loading PROGRESS.json…'}</Text>
      } else {
        const level = treeLevel(tree, zoom)
        const here = totalOf(level, zoom[zoom.length - 1] ?? 'All code')
        const isFunctions = zoom.length === 2
        const rows = Math.max(14, Math.min(40, e.props.scroll.bodyRows - 9))
        // Cells are about 2.3 times as tall as wide.
        const layout = stripLayout(level.map(n => ({ item: n, size: n.bytes })), cols / (rows * 2.3))
        const crumbs = ['All', ...zoom]
        body = (
          <Box flexDirection="column" gap={1}>
            <Box flexDirection="row" flexWrap="wrap">
              {crumbs.map((c, i) =>
                i === crumbs.length - 1 ? (
                  <Text bold>{c}</Text>
                ) : (
                  <Box flexDirection="row">
                    <Button key={`crumb-${i}`} plain label={c} onPress={() => zoomTo($, zoom.slice(0, i))} />
                    <Text dimColor> › </Text>
                  </Box>
                ),
              )}
            </Box>
            <Text dimColor wrap="truncate">
              {describe(here)}
            </Text>
            <Box flexDirection="column" height={rows} width="100%">
              {layout.map((row, r) => (
                <Box flexDirection="row" height={0} flexGrow={grow(row.height)} flexShrink={1} width="100%">
                  {row.tiles.map(({ item: n, width }) => {
                    const color = n.fn ? fnShade(n.fn.pct, n.fn.pinned) : shade(pctOf(n))
                    const showPct = (width / 100) * cols > n.name.length + 6
                    return (
                      <Box
                        key={`tile-${r}-${n.name}`}
                        width={0}
                        flexGrow={grow(width)}
                        flexShrink={1}
                        height="100%"
                        backgroundColor={color}
                        borderStyle="single"
                        borderColor="#1b1b1b"
                        overflow="hidden"
                        flexDirection="column"
                        hover={{ borderColor: '#f0f0f0' }}
                      >
                        <Button
                          key={`open-${n.name}`}
                          plain
                          label={showPct ? `${n.name} ${pct1(pctOf(n))}` : n.name}
                          onPress={() => (isFunctions ? openSource($, n, zoom) : zoomTo($, [...zoom, n.name]))}
                        />
                      </Box>
                    )
                  })}
                </Box>
              ))}
            </Box>
            <Text dimColor wrap="truncate">
              {isFunctions
                ? 'Green matched · teal pinned · amber drafts · grey assembly. Click one to see its C.'
                : 'Sized by bytes, blue deepens as it matches, green is done. Click to zoom in.'}
            </Text>
          </Box>
        )
      }
    } else {
      const dayAgo = now - 24 * 3600 * 1000
      const today = snap.prs.filter(p => Date.parse(p.mergedAt) >= dayAgo)
      const matches = today.reduce((s, p) => s + (p.matches ?? 0), 0)
      body = (
        <Box flexDirection="column" gap={1}>
          {spread(<Text>Last 24 hours</Text>, <Text bold>{`${matches} matches · ${today.length} PRs`}</Text>)}
          <Box flexDirection="column">
            {snap.prs.map(p => (
              <Box flexDirection="column" marginTop={1}>
                <Link href={`https://github.com/${REPO}/pull/${p.number}`} label={`#${p.number} ${p.title}`} />
                <Text dimColor>
                  {[p.matches ? `+${p.matches} matches` : '', p.progress !== undefined ? `${p.progress.toFixed(2)}%` : '', ago(p.mergedAt, now)]
                    .filter(Boolean)
                    .join(' · ')}
                </Text>
              </Box>
            ))}
          </Box>
        </Box>
      )
    }

    return (
      <Box flexDirection="column" gap={1}>
        {header}
        {tabs}
        {body}
        {spread(<Link href={DECOMP_DEV_URL} label="decomp.dev →" />, <Link href={blobUrl('PROGRESS.md')} label="PROGRESS.md →" />)}
      </Box>
    )
  }).catch(($, e) => {
    // A view the surface refuses must not leave the popup blank: offer a way back.
    const { Box, Text, Button } = $.ui.resolve(e)
    return (
      <Box flexDirection="column" gap={1}>
        <Text>Couldn't draw this view.</Text>
        <Button key="reset" label="Back to overview" onPress={() => resetView($)} />
      </Box>
    )
  })

  on('ui.render', { component: 'Pane', requestId: SOURCE }, async ($, e) => {
    const { Box, Text, Button, Link, Code } = $.ui.resolve(e)
    const s = source
    if (!s) return <Text dimColor>Click a function in the treemap to see its source.</Text>
    const f = s.node.fn
    return (
      <Box flexDirection="column" gap={1}>
        <Text bold>{s.name}</Text>
        <Text dimColor wrap="truncate">
          {`${s.where.join(' › ')} · ${describe(s.node)}`}
        </Text>
        {s.file && (
          <Box flexDirection="row" justifyContent="space-between">
            <Link href={blobUrl(s.file, s.state === 'ready' && !s.isWhole ? s.line : undefined)} label={s.file} />
            {s.state === 'ready' && !s.isWhole && (
              <Button key="whole" plain label={s.showWhole ? 'Just this function' : 'Whole file'} onPress={() => toggleWhole($)} />
            )}
          </Box>
        )}
        {s.state === 'loading' && <Text dimColor>Loading…</Text>}
        {s.state === 'error' && <Text color="#e06c6c">{`Couldn't load it: ${s.error}`}</Text>}
        {s.state === 'none' && <Text dimColor>No C for this yet: it is still assembly in the build.</Text>}
        {s.state === 'ready' && s.isWhole && f && f.pct <= 0 && (
          <Text dimColor>Not started: the unit holds it as an INCLUDE_ASM placeholder.</Text>
        )}
        {s.state === 'ready' && (
          <Code
            source={s.showWhole ? (s.whole ?? '').replace(/\n$/, '') : (s.text ?? '')}
            language="c"
            startLine={s.showWhole ? 1 : s.line}
          />
        )}
      </Box>
    )
  })
}
