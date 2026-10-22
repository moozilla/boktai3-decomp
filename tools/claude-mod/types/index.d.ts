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
