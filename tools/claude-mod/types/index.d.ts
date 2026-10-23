export type DecompBar = {
  name: string
  /** Functions matched and in total. */
  done: number
  total: number
  /** Percent of code bytes matched. */
  pct: number
}

export type DecompUnit = DecompBar & {
  /** Code bytes in the unit, from decomp.dev's report; absent when unknown. */
  bytes?: number
}

export type DecompCategory = DecompBar & {
  section: string
  units: DecompUnit[]
  /** Functions still to match. */
  left: number
  /** Unmatched functions with a draft at 95% or better. */
  near: number
}

export type DecompPr = {
  number: number
  title: string
  mergedAt: string
  matches?: number
  progress?: number
}

export type DecompSnapshot = {
  fetchedAt: string
  pct: number
  bytesDone: number
  bytesTotal: number
  fnDone: number
  fnTotal: number
  cleanPct: number
  pinnedPct: number
  drafts: { count: number; pct: number; fuzzy: number }
  asmBytes: number
  regions: DecompBar[]
  sections: DecompBar[]
  categories: DecompCategory[]
  prs: DecompPr[]
}

export type DecompTab = 'overview' | 'treemap' | 'recent'

/** One function of decomp.dev's report. */
export type DecompFunction = {
  name: string
  /** ROM address. */
  addr: number
  /** Code bytes. */
  size: number
  /** Fuzzy match percent: 100 is matched. */
  pct: number
}

/** One unit of decomp.dev's report, `Category/Unit` split. */
export type DecompReportUnit = {
  category: string
  name: string
  /** Code bytes in all and percent matched. */
  bytes: number
  pct: number
  done: number
  total: number
  functions: DecompFunction[]
}

/** Where the treemap is drilled to: everything, a category, or one unit in it. */
export type DecompLevel = { category?: string; unit?: string }

/** The function the source pane shows, and what was found for it. */
export type DecompSource = {
  fn: DecompFunction
  unit: string
  state: 'loading' | 'found' | 'missing'
  /** Repository-relative path, the text shown and its first line's number. */
  path?: string
  text?: string
  startLine?: number
  /** Read from the session's checkout or from GitHub's main branch. */
  from?: 'checkout' | 'github'
}
