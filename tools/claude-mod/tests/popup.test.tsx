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

test('the popup shows PROGRESS.md, the treemap and recent PRs', async ($, on) => {
  mock.store(on)
  on('command.register', () => ({ value: { command: 'decomp' } }))
  on('ui.open', () => ({ value: { isPlaced: true } }))
  on('ui.panes', () => ({ value: [] }))
  on('clock.every', () => ({ value: undefined }))
  on('http.fetch', (_, e) => {
    const text = e.url.includes('PROGRESS.md') ? PROGRESS : e.url.includes('api.github.com') ? PULLS : PAGE
    return { value: { status: 200, ok: true, headers: {}, text } }
  })
  on('session.start', (_, e) => ({ cwd: e.cwd }))

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })

  for (const surface of ['terminal', 'desktop'] as const) {
    const ui = await $.ui.mount({ ...PANE, surface })
    expect((await ui.find({ type: 'Text', text: /Code matched in C/ }))?.text).toBe('Code matched in C')
    expect(await ui.find({ type: 'Text', text: /10,578 \/ 11,780 functions {2}64\.50%/ })).toBeDefined()

    await ui.press({ key: 'cat-Player' })
    expect(await ui.find({ type: 'Text', text: /Player cue/ })).toBeDefined()

    await ui.press({ key: 'tab-treemap' })
    if (surface === 'desktop') expect((await ui.findAll({ type: 'Svg' })).length).toBe(1)
    else expect(await ui.find({ type: 'Text', text: /Libraries 100\.0%/ })).toBeDefined()

    await ui.press({ key: 'tab-recent' })
    expect(await ui.find({ type: 'Link', text: /#546 Engine core/ })).toBeDefined()
    expect(await ui.find({ type: 'Text', text: /\+6 · 64\.50% \(\+0\.04\)/ })).toBeDefined()

    await ui.press({ key: 'tab-overview' })
    await ui.press({ key: 'cat-Player' })
    expect(await ui.find({ type: 'Text', text: /Player cue/ })).toBeUndefined()
    await ui.unmount()
  }
})

test('the footer button shows the headline', async ($, on) => {
  mock.store(on)
  on('command.register', () => ({ value: { command: 'decomp' } }))
  on('clock.every', () => ({ value: undefined }))
  on('http.fetch', (_, e) => ({
    value: { status: 200, ok: true, headers: {}, text: e.url.includes('PROGRESS.md') ? PROGRESS : e.url.includes('api.github') ? PULLS : PAGE },
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
