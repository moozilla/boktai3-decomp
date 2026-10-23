// Parsing and drawing helpers for the decomp progress popup. Pure functions:
// no `$`, so the tests can call them directly.
import type {
  DecompBar,
  DecompCategory,
  DecompNode,
  DecompPr,
  DecompSnapshot,
  DecompTree,
} from '../types'

export const REPO = 'moozilla/boktai3-decomp'
export const PROGRESS_URL = `https://raw.githubusercontent.com/${REPO}/main/PROGRESS.md`
export const PULLS_URL = `https://api.github.com/repos/${REPO}/pulls?state=closed&sort=updated&direction=desc&per_page=40`
export const DECOMP_DEV_URL = `https://decomp.dev/${REPO}`
export const TREE_URL = `https://raw.githubusercontent.com/${REPO}/main/PROGRESS.json`
export const rawUrl = (path: string) => `https://raw.githubusercontent.com/${REPO}/main/${path}`
export const blobUrl = (path: string, line?: number) =>
  `https://github.com/${REPO}/blob/main/${path}${line ? `#L${line}` : ''}`

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

export function ago(iso: string, now: number): string {
  const s = Math.max(0, Math.round((now - Date.parse(iso)) / 1000))
  if (s < 60) return 'just now'
  if (s < 3600) return `${Math.round(s / 60)} min ago`
  if (s < 86400) return `${Math.round(s / 3600)} hr ago`
  return `${Math.round(s / 86400)} d ago`
}

/** The fill for a share matched: dark grey at nothing, blue as it grows, green when done. */
export function shade(pct: number): string {
  if (pct >= 100) return '#3f9b4f'
  const t = Math.max(0, Math.min(1, pct / 100))
  const mix = (a: number, b: number) => Math.round(a + (b - a) * t)
  return `rgb(${mix(52, 46)},${mix(54, 104)},${mix(60, 214)})`
}

// --- Function tree (PROGRESS.json) ------------------------------------------

/** The children of a treemap level: [] for the root, [category], [category, unit]. */
export function treeLevel(tree: DecompTree, path: string[]): DecompNode[] {
  const fnNode = (f: DecompTree['categories'][number]['units'][number]['functions'][number]): DecompNode => {
    const [name, address, bytes, pct, pinned, file, cName] = f
    return {
      name,
      bytes,
      doneBytes: pct >= 100 ? bytes : 0,
      done: pct >= 100 ? 1 : 0,
      total: 1,
      near: pct < 100 && pct >= 95 ? 1 : 0,
      fn: {
        address,
        pct,
        pinned: pinned === 1,
        file: file === -2 ? null : file === -1 ? `src/fn/sub_${address}.c` : (tree.files[file] ?? null),
        ...(cName ? { cName } : {}),
      },
    }
  }
  const sum = (name: string, kids: DecompNode[]): DecompNode => ({
    name,
    bytes: kids.reduce((s, k) => s + k.bytes, 0),
    doneBytes: kids.reduce((s, k) => s + k.doneBytes, 0),
    done: kids.reduce((s, k) => s + k.done, 0),
    total: kids.reduce((s, k) => s + k.total, 0),
    near: kids.reduce((s, k) => s + k.near, 0),
  })
  const unitNode = (u: DecompTree['categories'][number]['units'][number]) => sum(u.name, u.functions.map(fnNode))
  const [cat, unit] = path
  if (cat === undefined) return tree.categories.map(c => sum(c.name, c.units.map(unitNode)))
  const c = tree.categories.find(x => x.name === cat)
  if (!c) return []
  if (unit === undefined) return c.units.map(unitNode)
  return c.units.find(x => x.name === unit)?.functions.map(fnNode) ?? []
}

export function totalOf(nodes: DecompNode[], name: string): DecompNode {
  return {
    name,
    bytes: nodes.reduce((s, k) => s + k.bytes, 0),
    doneBytes: nodes.reduce((s, k) => s + k.doneBytes, 0),
    done: nodes.reduce((s, k) => s + k.done, 0),
    total: nodes.reduce((s, k) => s + k.total, 0),
    near: nodes.reduce((s, k) => s + k.near, 0),
  }
}

export const pctOf = (n: DecompNode) => (n.fn ? n.fn.pct : n.bytes ? (100 * n.doneBytes) / n.bytes : 0)

/**
 * Strip treemap: rows top to bottom, each a share of the height, its tiles a
 * share of the row's width. Two levels of boxes, so it draws with flex alone.
 * `aspect` is the box's width over its height.
 */
export function stripLayout<T>(
  items: { item: T; size: number }[],
  aspect: number,
): { height: number; size: number; tiles: { item: T; width: number; size: number }[] }[] {
  const sorted = items.filter(i => i.size > 0).sort((a, b) => b.size - a.size)
  const total = sorted.reduce((s, i) => s + i.size, 0)
  if (!total) return []
  const W = aspect
  const H = 1
  const rows: { item: T; size: number }[][] = []
  let row: { item: T; size: number }[] = []
  const worst = (r: { size: number }[]) => {
    const sum = r.reduce((s, i) => s + i.size, 0)
    const h = (sum / total) * H
    return Math.max(...r.map(i => {
      const w = (i.size / sum) * W
      return Math.max(w / h, h / w)
    }))
  }
  for (const it of sorted) {
    if (row.length && worst([...row, it]) > worst(row)) {
      rows.push(row)
      row = []
    }
    row.push(it)
  }
  if (row.length) rows.push(row)
  return rows.map(r => {
    const sum = r.reduce((s, i) => s + i.size, 0)
    return { height: (100 * sum) / total, size: sum, tiles: r.map(i => ({ item: i.item, width: (100 * i.size) / sum, size: i.size })) }
  })
}

/** A function's colour at the function level: green done, teal pinned, amber drafts, grey nothing yet. */
export function fnShade(pct: number, pinned: boolean): string {
  if (pct >= 100) return pinned ? '#2f7f78' : '#3f9b4f'
  if (pct <= 0) return '#3a3a3a'
  const t = Math.max(0, Math.min(1, (pct - 50) / 50))
  const mix = (a: number, b: number) => Math.round(a + (b - a) * t)
  return `rgb(${mix(110, 214)},${mix(84, 150)},${mix(40, 46)})`
}

/** The function's text in a C file, with its 1-based first line; the whole file if it can't be found. */
export function extractFunction(text: string, names: string[]): { source: string; line: number; isWhole: boolean } {
  const lines = text.split('\n')
  // `#define Name sub_XXXXXXXX` lets the definition use the friendly name.
  for (const m of text.matchAll(/^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$/gm)) if (names.includes(m[2]!)) names = [...names, m[1]!]
  for (const name of names) {
    const re = new RegExp(`\\b${name}\\s*\\(`)
    for (let i = 0; i < lines.length; i++) {
      const l = lines[i]!
      if (!re.test(l) || /^\s*(#|\/\/|\*|\/\*)/.test(l) || /;\s*$/.test(l) || /^\s/.test(l)) continue
      // A definition: the text from here reaches `{` before any `;`.
      const rest = lines.slice(i, i + 12).join('\n')
      const brace = rest.indexOf('{')
      const semi = rest.indexOf(';')
      if (brace < 0 || (semi >= 0 && semi < brace)) continue
      let start = i
      while (start > 0 && /^\s*(\/\/|\/\*|\*)/.test(lines[start - 1]!)) start--
      let depth = 0
      let seen = false
      for (let j = i; j < lines.length; j++) {
        for (const ch of lines[j]!) {
          if (ch === '{') {
            depth++
            seen = true
          } else if (ch === '}') depth--
        }
        if (seen && depth === 0) return { source: lines.slice(start, j + 1).join('\n'), line: start + 1, isWhole: false }
      }
    }
  }
  return { source: text.replace(/\n$/, ''), line: 1, isWhole: true }
}
