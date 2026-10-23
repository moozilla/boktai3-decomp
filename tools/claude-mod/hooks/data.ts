// Parsing and drawing helpers for the decomp progress popup. Pure functions:
// no `$`, so the tests can call them directly.
import type {
  DecompBar,
  DecompCategory,
  DecompFunction,
  DecompPr,
  DecompReportUnit,
  DecompSnapshot,
} from '../types'

export const REPO = 'moozilla/boktai3-decomp'
export const PROGRESS_URL = `https://raw.githubusercontent.com/${REPO}/main/PROGRESS.md`
export const PULLS_URL = `https://api.github.com/repos/${REPO}/pulls?state=closed&sort=updated&direction=desc&per_page=40`
export const DECOMP_DEV_URL = `https://decomp.dev/${REPO}`
/** decomp.dev's copy of the objdiff report: every unit with its functions. */
export const REPORT_URL = `${DECOMP_DEV_URL}.json?mode=report`
export const RAW_URL = `https://raw.githubusercontent.com/${REPO}/main`

const num = (s: string) => Number(s.replace(/,/g, ''))
const ROW = /^\| (\*\*)?(?:&nbsp;&nbsp;)?([^|*][^|]*?)(\*\*)? \| (\d+)\/(\d+) \| ([\d.]+)% \|/

/** Reads PROGRESS.md into everything but the PRs. */
export function parseProgress(md: string): Omit<DecompSnapshot, 'prs' | 'fetchedAt'> {
  const head = md.match(
    /Code matched in C: ([\d.]+)%\*\* \(([\d,]+) \/ ([\d,]+) bytes\)[^*]*\*\*(\d+) \/ (\d+) functions/,
  )
  if (!head) throw new Error('PROGRESS.md has no headline')
  const clean = md.match(/Without register pinning: \*\*([\d.]+)%\*\*\. Register-pinned: \*\*([\d.]+)%/)
  const drafts = md.match(/\*\*(\d+) functions\*\*, ([\d.]+)% of code, mean fuzzy match ([\d.]+)%/)
  const asm = md.match(/Intentional assembly[^:]*: ([\d,]+) bytes/)

  const regions: DecompBar[] = []
  const sections: DecompBar[] = []
  const categories: DecompCategory[] = []
  let part: 'top' | 'subsystems' | 'details' = 'top'
  let section = ''
  let current: DecompCategory | undefined

  for (const line of md.split('\n')) {
    if (line.startsWith('## By subsystem')) part = 'subsystems'
    const region = line.match(/^\| `([0-9A-F]+)–([0-9A-F]+)` \| (\d+) \| (\d+) \| ([\d.]+)% \|/)
    if (region) {
      regions.push({ name: `${region[1]}–${region[2]}`, total: num(region[3]!), done: num(region[4]!), pct: num(region[5]!) })
      continue
    }
    const heading = line.match(/^### (.+)/)
    if (heading) {
      part = 'details'
      section = heading[1]!.trim()
      continue
    }
    const summary = line.match(/^<details><summary><b>(.+?)<\/b>/)
    if (summary) {
      current = categories.find(c => c.name === summary[1])
      continue
    }
    if (line.startsWith('</details>')) {
      current = undefined
      continue
    }
    const left = line.match(/^Still to match \((\d+)\)/)
    if (left && current) {
      current.left = num(left[1]!)
      continue
    }
    if (line.startsWith('- ') && current) {
      for (const m of line.matchAll(/ \((\d+)%\)/g)) if (num(m[1]!) >= 95) current.near += 1
      continue
    }
    const row = line.match(ROW)
    if (!row) continue
    const bar = { name: row[2]!.trim(), done: num(row[4]!), total: num(row[5]!), pct: num(row[6]!) }
    const isBold = row[1] === '**'
    const isIndented = line.includes('&nbsp;')
    if (part === 'subsystems') {
      if (isBold) {
        section = bar.name
        sections.push(bar)
      } else if (isIndented) {
        categories.push({ ...bar, section, units: [], left: bar.total - bar.done, near: 0 })
      }
    } else if (part === 'details' && current && !isBold) {
      current.units.push(bar)
    }
  }

  return {
    pct: num(head[1]!),
    bytesDone: num(head[2]!),
    bytesTotal: num(head[3]!),
    fnDone: num(head[4]!),
    fnTotal: num(head[5]!),
    cleanPct: clean ? num(clean[1]!) : 0,
    pinnedPct: clean ? num(clean[2]!) : 0,
    drafts: drafts ? { count: num(drafts[1]!), pct: num(drafts[2]!), fuzzy: num(drafts[3]!) } : { count: 0, pct: 0, fuzzy: 0 },
    asmBytes: asm ? num(asm[1]!) : 0,
    regions,
    sections,
    categories,
  }
}

/** Unit sizes in code bytes, keyed `Category/Unit`, from decomp.dev's page. */
export function parseUnitBytes(html: string): Record<string, number> {
  const m = html.match(/window\.units\s*=\s*(\[.*?\]);/s)
  if (!m) return {}
  const out: Record<string, number> = {}
  for (const u of JSON.parse(m[1]!) as { name: string; total_code: number }[]) out[u.name] = u.total_code
  return out
}

export function attachUnitBytes(categories: DecompCategory[], bytes: Record<string, number>): void {
  for (const c of categories) for (const u of c.units) {
    const b = bytes[`${c.name}/${u.name}`]
    if (b) u.bytes = b
  }
}

type GithubPull = { number: number; title: string; body: string | null; merged_at: string | null }

/** Merged PRs, newest first, with the match count and progress they report. */
export function parsePulls(pulls: GithubPull[]): DecompPr[] {
  return pulls
    .filter(p => p.merged_at)
    .sort((a, b) => b.merged_at!.localeCompare(a.merged_at!))
    .map(p => {
      const text = `${p.title}\n${p.body ?? ''}`
      const matches =
        p.title.match(/(\d+)\s+(?:exact\s+|natural\s+)?match(?:es)?\b/i) ??
        p.title.match(/\bMatch(?:ed)?\s+(?:about\s+)?(\d+)\b/i) ??
        p.title.match(/\((\d+) functions\)/i) ??
        text.match(/adds (\d+) match(?:es)?\b/i) ??
        text.match(/(\d+)\s+exact\s+match(?:es)?\b/i)
      const progress =
        text.match(/Progress(?: is)?:?\s*\**([\d.]+)%/i) ??
        text.match(/\(([\d.]+)%(?: overall)?,\s*\d+\s*\/\s*\d+ functions/) ??
        text.match(/([\d.]+)% overall/)
      const pr: DecompPr = { number: p.number, title: p.title, mergedAt: p.merged_at! }
      if (matches) pr.matches = num(matches[1]!)
      if (progress) pr.progress = num(progress[1]!)
      return pr
    })
}

type ReportJson = {
  units: {
    name: string
    measures: { total_code?: string; matched_code_percent?: number; total_functions?: number; matched_functions?: number }
    functions?: { name: string; size: string; fuzzy_match_percent?: number; metadata?: { virtual_address?: string } }[]
  }[]
}

/** decomp.dev's report as units of functions; assembly units are left out. */
export function parseReport(report: ReportJson): DecompReportUnit[] {
  return report.units
    .filter(u => !u.name.endsWith(' (assembly)'))
    .map(u => {
      const cut = u.name.indexOf('/')
      const functions: DecompFunction[] = (u.functions ?? []).map(f => ({
        name: f.name,
        addr: Number(f.metadata?.virtual_address ?? 0),
        size: Number(f.size),
        pct: f.fuzzy_match_percent ?? 0,
      }))
      return {
        category: cut < 0 ? '' : u.name.slice(0, cut),
        name: cut < 0 ? u.name : u.name.slice(cut + 1),
        bytes: Number(u.measures.total_code ?? 0),
        pct: u.measures.matched_code_percent ?? 0,
        done: u.measures.matched_functions ?? 0,
        total: u.measures.total_functions ?? functions.length,
        functions,
      }
    })
}

/** The names a function's source may go by: its report name, then `sub_<address>`. */
export function sourceNames(fn: DecompFunction): string[] {
  const sub = `sub_${fn.addr.toString(16).toUpperCase().padStart(8, '0')}`
  return fn.name === sub ? [sub] : [fn.name, sub]
}

/** Picks the definition among `git grep -n` lines (`path:line:text`): not a prototype. */
export function definitionHit(grep: string): { path: string; line: number } | undefined {
  for (const l of grep.split('\n')) {
    const m = l.match(/^([^:]+):(\d+):(.*)$/)
    if (m && !m[3]!.trimEnd().endsWith(';')) return { path: m[1]!, line: Number(m[2]) }
  }
  return undefined
}

/** The function starting at 1-based `line` of `text`, through its closing brace at column 0. */
export function functionAt(text: string, line: number): { text: string; startLine: number } {
  const lines = text.split('\n')
  let start = line - 1
  // Take the comment block right above the definition with it.
  while (start > 0 && /^\s*(\/\/|\/\*|\*)/.test(lines[start - 1]!)) start--
  let end = line - 1
  while (end < lines.length - 1 && lines[end] !== '}') end++
  return { text: lines.slice(start, end + 1).join('\n'), startLine: start + 1 }
}

export function ago(iso: string, now: number): string {
  const s = Math.max(0, Math.round((now - Date.parse(iso)) / 1000))
  if (s < 60) return 'just now'
  if (s < 3600) return `${Math.round(s / 60)} min ago`
  if (s < 86400) return `${Math.round(s / 3600)} hr ago`
  return `${Math.round(s / 86400)} d ago`
}

// --- Treemap ---------------------------------------------------------------

export type Rect = { x: number; y: number; w: number; h: number }
type Sized<T> = { item: T; size: number }

/** Squarified treemap layout: each item's rectangle, in the order given. */
export function squarify<T>(items: Sized<T>[], box: Rect): { item: T; rect: Rect }[] {
  const total = items.reduce((s, i) => s + i.size, 0)
  if (total <= 0 || box.w <= 0 || box.h <= 0) return []
  const scale = (box.w * box.h) / total
  const queue = [...items].filter(i => i.size > 0).sort((a, b) => b.size - a.size)
  const out: { item: T; rect: Rect }[] = []
  let { x, y, w, h } = box

  const worst = (row: number[], side: number) => {
    const sum = row.reduce((a, b) => a + b, 0)
    const max = Math.max(...row)
    const min = Math.min(...row)
    return Math.max((side * side * max) / (sum * sum), (sum * sum) / (side * side * min))
  }

  while (queue.length) {
    const side = Math.min(w, h)
    const row: Sized<T>[] = [queue.shift()!]
    while (queue.length) {
      const next = queue[0]!
      const areas = row.map(r => r.size * scale)
      if (worst([...areas, next.size * scale], side) > worst(areas, side)) break
      row.push(queue.shift()!)
    }
    const sum = row.reduce((s, r) => s + r.size * scale, 0)
    if (w >= h) {
      const rw = sum / h
      let cy = y
      for (const r of row) {
        const rh = (r.size * scale) / rw
        out.push({ item: r.item, rect: { x, y: cy, w: rw, h: rh } })
        cy += rh
      }
      x += rw
      w -= rw
    } else {
      const rh = sum / w
      let cx = x
      for (const r of row) {
        const rw = (r.size * scale) / rh
        out.push({ item: r.item, rect: { x: cx, y, w: rw, h: rh } })
        cx += rw
      }
      y += rh
      h -= rh
    }
  }
  return out
}

const esc = (s: string) => s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;')
const r1 = (n: number) => Math.round(n * 10) / 10
/** Cuts a label to the width it has, at a rough glyph width. */
const fit = (s: string, width: number, glyph: number) =>
  s.length * glyph <= width ? s : s.slice(0, Math.max(1, Math.floor(width / glyph) - 1)) + '…'

/** The fill for a share matched: dark grey at nothing, blue as it grows, green when done. */
export function shade(pct: number): string {
  if (pct >= 100) return '#3f9b4f'
  const t = Math.max(0, Math.min(1, pct / 100))
  const mix = (a: number, b: number) => Math.round(a + (b - a) * t)
  return `rgb(${mix(52, 46)},${mix(54, 104)},${mix(60, 214)})`
}

/** A treemap tile: sized by code bytes, shaded by the share matched, `title` its hover text. */
export type TreemapNode = { name: string; size: number; pct: number; title: string; children?: TreemapNode[] }

/** One level of the treemap as one SVG: each node a tile, its children tiled inside it. */
export function treemapSvg(nodes: TreemapNode[], width: number, height: number): string {
  const parts: string[] = []
  const placed = squarify(
    nodes.map(n => ({ item: n, size: n.size })),
    { x: 0, y: 0, w: width, h: height },
  )
  const leaf = (n: TreemapNode, r: Rect, stroke: string) => {
    const fits = r.w > 44 && r.h > 13
    parts.push(
      `<g><title>${esc(n.title)}</title>` +
        `<rect x="${r1(r.x)}" y="${r1(r.y)}" width="${r1(r.w)}" height="${r1(r.h)}" fill="${shade(n.pct)}" stroke="${stroke}" stroke-width="0.75"/>` +
        (fits ? `<text x="${r1(r.x + 3)}" y="${r1(r.y + 10)}" font-size="8" fill="#ffffffcc">${esc(fit(n.name, r.w - 6, 4.6))}</text>` : '') +
        '</g>',
    )
  }
  for (const { item: n, rect } of placed) {
    if (!n.children?.length) {
      leaf(n, rect, '#141414')
      continue
    }
    const label = rect.w > 60 && rect.h > 26
    const inner = { x: rect.x + 1, y: rect.y + (label ? 14 : 1), w: rect.w - 2, h: rect.h - (label ? 15 : 2) }
    parts.push(
      `<g><title>${esc(n.title)}</title>` +
        `<rect x="${r1(rect.x)}" y="${r1(rect.y)}" width="${r1(rect.w)}" height="${r1(rect.h)}" fill="#1f1f1f" stroke="#141414"/>`,
    )
    if (label) {
      parts.push(
        `<text x="${r1(rect.x + 4)}" y="${r1(rect.y + 10.5)}" font-size="9.5" fill="#d8d8d8" font-weight="600">${esc(fit(`${n.name} · ${n.pct.toFixed(1)}%`, rect.w - 8, 5.6))}</text>`,
      )
    }
    parts.push('</g>')
    for (const { item: c, rect: cr } of squarify(n.children.map(c => ({ item: c, size: c.size })), inner)) leaf(c, cr, '#1f1f1f')
  }
  return (
    `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" ` +
    `font-family="-apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif">` +
    `<style>g:hover > rect { stroke: #fff; stroke-width: 1.5 }</style>${parts.join('')}</svg>`
  )
}

/** Matched share as a text bar, for surfaces that draw no SVG. */
export function textBar(pct: number, cells: number): string {
  const full = Math.round((Math.max(0, Math.min(100, pct)) / 100) * cells)
  return '█'.repeat(full) + '░'.repeat(Math.max(0, cells - full))
}
