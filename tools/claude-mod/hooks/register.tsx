import type { EngineInterface, Register } from 'claude-code'

import type { DecompCategory, DecompSnapshot, DecompTab } from '../types'
import {
  DECOMP_DEV_URL,
  PROGRESS_URL,
  PULLS_URL,
  REPO,
  ago,
  attachUnitBytes,
  parseProgress,
  parsePulls,
  parseUnitBytes,
  shade,
  textBar,
  treemapSvg,
} from './data'

const PANE = 'decomp-progress'
const TITLE = 'Decomp progress'
const STALE_MS = 10 * 60 * 1000

// What the popup draws. Module variables rather than $.state, which older
// builds (2.1.271) don't have; a reload starts over from the store.
let snapshot: DecompSnapshot | null = null
let status = ''
let tab: DecompTab = 'overview'
let expanded = ''

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
function toggleExpanded($: EngineInterface, name: string): void {
  expanded = expanded === name ? '' : name
  redraw($)
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
}

async function openPopup($: EngineInterface): Promise<void> {
  await $.ui.open({ id: PANE, title: TITLE, focus: true, closeOnEscape: true, rows: 34, columns: 64 })
  void refresh($)
}

const fmt = (n: number) => n.toLocaleString('en-US')

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
    const { Box, Button, Text } = $.ui.resolve(e)
    const snap = snapshot
    const label = snap ? `◔ ${snap.pct.toFixed(2)}%` : '◔ decomp'

    return (
      <Box flexDirection="row" gap={1} alignItems="center">
        {engine}
        <Button key="decomp-progress-open" plain label={label} onPress={() => openPopup($)} />
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
    const open = expanded
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

    const row = (left: string, right: string, opts: { bold?: boolean; dim?: boolean } = {}) => (
      <Box flexDirection="row" justifyContent="space-between">
        <Text bold={opts.bold} dimColor={opts.dim}>{left}</Text>
        <Text dimColor>{right}</Text>
      </Box>
    )

    const header = (
      <Box flexDirection="row" justifyContent="space-between" alignItems="center">
        <Text dimColor>Boktai 3 decomp · {REPO.split('/')[1]} main</Text>
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

    const categoryRow = (c: DecompCategory) => {
      const isOpen = open === c.name
      const color = c.pct >= 100 ? GREEN : BLUE
      return (
        <Box flexDirection="column">
          <Box flexDirection="row" justifyContent="space-between">
            <Button key={`cat-${c.name}`} plain label={`${isOpen ? '▾' : '▸'} ${c.name}`} onPress={() => toggleExpanded($, c.name)} />
            <Text dimColor>
              {`${c.done}/${c.total} · ${c.left ? `${c.left} left${c.near ? `, ${c.near} ≥95%` : ''} · ` : ''}${c.pct.toFixed(1)}%`}
            </Text>
          </Box>
          {bar([{ pct: c.pct, color }], `bar-${c.name}`)}
          {isOpen && (
            <Box flexDirection="column" paddingLeft={2} marginTop={1}>
              {[...c.units]
                .sort((a, b) => a.pct - b.pct)
                .map(u =>
                  row(
                    `${u.pct >= 100 ? '●' : '○'} ${u.name}`,
                    `${u.done}/${u.total}${u.bytes ? ` · ${fmt(u.bytes)} B` : ''} · ${u.pct.toFixed(1)}%`,
                    { dim: u.pct >= 100 },
                  ),
                )}
            </Box>
          )}
        </Box>
      )
    }

    let body
    if (current === 'overview') {
      const clean = snap.cleanPct
      const pinned = Math.max(0, snap.pct - clean)
      body = (
        <Box flexDirection="column" gap={1}>
          <Box flexDirection="column">
            {row('Code matched in C', `${fmt(snap.fnDone)} / ${fmt(snap.fnTotal)} functions  ${snap.pct.toFixed(2)}%`, { bold: true })}
            {bar(
              [
                { pct: clean, color: BLUE },
                { pct: pinned, color: PINNED },
              ],
              'bar-headline',
            )}
          </Box>
          <Box flexDirection="column">
            {row('Without register pinning', `${clean.toFixed(2)}%`)}
            {bar([{ pct: clean, color: BLUE }], 'bar-clean')}
          </Box>
          <Box flexDirection="column">
            {row('Nonmatching drafts', `${snap.drafts.count} functions · ${snap.drafts.pct.toFixed(1)}% of code · fuzzy ${snap.drafts.fuzzy}%`)}
            {row('Bytes matched', `${fmt(snap.bytesDone)} / ${fmt(snap.bytesTotal)} · pinned ${snap.pinnedPct.toFixed(2)}%`, { dim: true })}
            {row('Intentional assembly (not counted)', `${fmt(snap.asmBytes)} bytes`, { dim: true })}
          </Box>
          {snap.sections.map(s => (
            <Box flexDirection="column" gap={1}>
              {row(s.name, `${s.done}/${s.total} · ${s.pct.toFixed(1)}%`, { dim: true })}
              {snap.categories.filter(c => c.section === s.name).map(categoryRow)}
            </Box>
          ))}
          <Box flexDirection="column">
            <Button key="regions" plain label={`${open === '#regions' ? '▾' : '▸'} ROM regions`} onPress={() => toggleExpanded($, '#regions')} />
            {open === '#regions' &&
              snap.regions.map(r => (
                <Box flexDirection="column" paddingLeft={2}>
                  {row(r.name, `${r.done}/${r.total} · ${r.pct.toFixed(1)}%`)}
                  {bar([{ pct: r.pct, color: r.pct >= 100 ? GREEN : BLUE }], `bar-${r.name}`)}
                </Box>
              ))}
          </Box>
        </Box>
      )
    } else if (current === 'treemap') {
      const height = Math.round(px * 0.72)
      body = (
        <Box flexDirection="column" gap={1}>
          {Svg ? (
            <Svg
              key="treemap"
              source={treemapSvg(snap.categories, px, height)}
              alt="Treemap of categories and units, sized by code bytes and shaded by bytes matched"
              width={px}
              height={height}
              isInteractive
            />
          ) : (
            <Box flexDirection="column">
              {snap.categories.map(c => (
                <Text>
                  <Text color={shade(c.pct)}>{textBar(c.pct, 12)}</Text> {c.name} {c.pct.toFixed(1)}%
                </Text>
              ))}
            </Box>
          )}
          <Text dimColor>
            Tiles are units, sized by code bytes; blue deepens with bytes matched, green is complete. Hover a tile for numbers.
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
          {row('Last 24 hours', `${today.length} PRs · ${matches} matches`, { bold: true })}
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
