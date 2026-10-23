import { expect, mock, test } from 'claude-code/testing'

const PROGRESS = `# Decompilation progress

**Code matched in C: 64.50%** (1,554,878 / 2,410,598 bytes) — **10578 / 11780 functions**

Without register pinning: **47.570%**. Register-pinned: **16.932%** of total code (included above).

Nonmatching drafts (\`#ifdef NONMATCHING\`, not counted above): **475 functions**, 14.542% of code, mean fuzzy match 87.7%.

Intentional assembly (GBA header, crt0, SDK assembly; not counted above): 4,756 bytes.

| \`08000000–08040000\` | 1726 | 1631 | 79.3% | \`████\` |

## By subsystem

| **Gameplay** | 5253/6063 | 57.4% | \`███\` |
| &nbsp;&nbsp;Player | 630/737 | 63.3% | \`███\` |
| **Libraries** | 298/298 | 100.0% | \`███\` |
| &nbsp;&nbsp;Libraries | 298/298 | 100.0% | \`███\` |

### Gameplay

<details><summary><b>Player</b>: 630/737 functions, 63.3% of bytes</summary>

| **Player** | 616/721 | 63.1% | \`█\` |
| &nbsp;&nbsp;Player cue | 110/128 | 48.6% | \`█\` |
| Target registry | 14/16 | 83.4% | \`█\` |

Still to match (107):

- Player: \`A\` 72 B, \`B\` 104 B (97%)

</details>

### Libraries

<details><summary><b>Libraries</b>: 298/298 functions, 100.0% of bytes</summary>

| MP2K (m4a) | 61/61 | 100.0% | \`█\` |

</details>
`

const PULLS = JSON.stringify([
  { number: 546, title: 'Engine core: 6 matches', body: 'Progress: 64.50% (10578/11780).', merged_at: '2026-10-22T12:30:01Z' },
  { number: 545, title: 'Link: 5 matches', body: 'Progress: 64.46%', merged_at: '2026-10-22T11:45:27Z' },
  { number: 544, title: 'Unmerged', body: null, merged_at: null },
])

const PAGE = '<script>window.units=[{"name":"Player/Player cue","total_code":4000},{"name":"Player/Target registry","total_code":900},{"name":"Libraries/MP2K (m4a)","total_code":7000}];</script>'

const REPORT = JSON.stringify({
  units: [
    {
      name: 'Player/Player cue',
      measures: { total_code: '4000', matched_code_percent: 48.6, total_functions: 128, matched_functions: 110 },
      functions: [
        { name: 'PlayerCue_Update', size: '120', fuzzy_match_percent: 100, metadata: { virtual_address: '134351204' } },
        { name: 'sub_08020964', size: '64', fuzzy_match_percent: 87.5, metadata: { virtual_address: '134351204' } },
      ],
    },
    { name: 'Player/Target registry', measures: { total_code: '900', matched_code_percent: 83.4, total_functions: 16, matched_functions: 14 }, functions: [] },
    { name: 'Libraries/MP2K (m4a)', measures: { total_code: '7000', matched_code_percent: 100, total_functions: 61, matched_functions: 61 }, functions: [] },
    { name: 'Libraries/crt0 (assembly)', measures: { total_code: '300' }, functions: [] },
  ],
})

const SOURCE = 'void PlayerCue_Update(void)\n{\n}\n'

const fetched = (url: string) =>
  url.includes('PROGRESS.md')
    ? PROGRESS
    : url.includes('api.github.com')
      ? PULLS
      : url.includes('mode=report')
        ? REPORT
        : url.includes('raw.githubusercontent.com')
          ? SOURCE
          : PAGE

const PANE = {
  plugin: 'decomp-progress',
  component: 'Pane',
  requestId: 'decomp-progress',
  props: {
    title: 'Decomp progress',
    isFocused: true,
    bodyColumns: 60,
    placement: 'inline',
    scroll: { offset: 0, bodyRows: 34 },
    view: {},
  },
} as const

const SOURCE_PANE = {
  ...PANE,
  requestId: 'decomp-source',
  props: { ...PANE.props, title: 'PlayerCue_Update', bodyColumns: 88 },
} as const

test('the popup shows PROGRESS.md, the treemap down to functions, and recent PRs', async ($, on) => {
  mock.store(on)
  on('command.register', () => ({ value: { command: 'decomp' } }))
  on('ui.panes', () => ({ value: [] }))
  on('clock.every', () => ({ value: undefined }))
  const opened: string[] = []
  on('http.fetch', (_, e) => ({ value: { status: 200, ok: true, headers: {}, text: fetched(e.url) } }))
  on('ui.open', (_, e) => {
    opened.push(e.id)
    return { value: { isPlaced: true } }
  })
  on('session.root', () => ({ value: '/elsewhere' }))
  on('fs.exists', () => ({ value: false }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })

  for (const surface of ['terminal', 'desktop'] as const) {
    const ui = await $.ui.mount({ ...PANE, surface })
    expect(await ui.find({ type: 'Text', text: '64.50%' })).toBeDefined()
    expect(await ui.find({ type: 'Text', text: '10,578 / 11,780 functions' })).toBeDefined()
    // The details are drawn hidden, for the pointer to reveal.
    expect(await ui.find({ type: 'Text', text: /Nonmatching drafts: 475 functions/ })).toBeDefined()
    expect(await ui.find({ type: 'Text', text: /107 left · 1 draft ≥95%/ })).toBeDefined()

    // A category opens the treemap there, its units listed most-left first.
    await ui.press({ key: 'cat-Player' })
    expect(await ui.find({ key: 'crumb-category' })).toBeDefined()
    if (surface === 'desktop') expect((await ui.findAll({ type: 'Svg' })).length).toBe(1)
    expect(await ui.find({ key: 'go-Player cue' })).toBeDefined()

    // A unit lists its functions; one opens its C in the source pane.
    await ui.press({ key: 'go-Player cue' })
    expect(await ui.find({ type: 'Text', text: /64 B · 87\.5%/ })).toBeDefined()
    await ui.press({ key: 'go-PlayerCue_Update' })
    expect(opened).toContain('decomp-source')
    const src = await $.ui.mount({ ...SOURCE_PANE, surface })
    expect(await src.find({ type: 'Text', text: 'matched' })).toBeDefined()
    expect(await src.find({ type: 'Text', text: /src\/fn\/PlayerCue_Update\.c \(main\)/ })).toBeDefined()
    await src.unmount()

    await ui.press({ key: 'crumb-root' })
    expect(await ui.find({ key: 'go-Libraries' })).toBeDefined()

    await ui.press({ key: 'tab-recent' })
    expect(await ui.find({ type: 'Link', text: /#546 Engine core/ })).toBeDefined()
    expect(await ui.find({ type: 'Text', text: /\+6 · 64\.50% \(\+0\.04\)/ })).toBeDefined()

    await ui.press({ key: 'tab-overview' })
    await ui.unmount()
  }
})

test('the footer button shows the headline', async ($, on) => {
  mock.store(on)
  on('command.register', () => ({ value: { command: 'decomp' } }))
  on('clock.every', () => ({ value: undefined }))
  on('http.fetch', (_, e) => ({
    value: { status: 200, ok: true, headers: {}, text: fetched(e.url) },
  }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))
  on('ui.render', ($, e) => {
    const { Text } = $.ui.resolve(e)
    return <Text>focus</Text>
  })
  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })

  for (const surface of ['terminal', 'desktop'] as const) {
    const ui = await $.ui.mount({ plugin: 'decomp-progress', surface, component: 'SessionMode', requestId: 'mode', props: { modes: ['focus'] } })
    expect(await ui.find({ key: 'decomp-progress-open' })).toBeDefined()
    await ui.unmount()
  }
})
