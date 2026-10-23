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

const TREE = JSON.stringify({
  version: 1,
  files: ['src/player.c'],
  categories: [
    {
      name: 'Player',
      units: [
        { name: 'Player cue', functions: [['PlayerCue_Run', '08150000', 400, 100, 0, 0], ['sub_08150190', '08150190', 120, 93.5, 0, -1]] },
        { name: 'Target registry', functions: [['Target_Add', '08160000', 64, 0, 0, -2]] },
      ],
    },
    { name: 'Libraries', units: [{ name: 'MP2K (m4a)', functions: [['m4aSoundInit', '08200000', 300, 100, 1, 0]] }] },
  ],
})

const PLAYER_C = `#include "global.h"

// Runs the cue.
void PlayerCue_Run(void)
{
    if (x) {
        y();
    }
}

void Other(void) {}
`

function fetchAnswer(url: string): string {
  if (url.includes('PROGRESS.md')) return PROGRESS
  if (url.includes('PROGRESS.json')) return TREE
  if (url.includes('api.github.com')) return PULLS
  if (url.includes('src/player.c')) return PLAYER_C
  return ''
}

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
  on('http.fetch', (_, e) => ({ value: { status: 200, ok: true, headers: {}, text: fetchAnswer(e.url) } }))
  on('session.start', (_, e) => ({ cwd: e.cwd }))

  await $.session.start({ cwd: '/', surface: 'desktop', isInteractive: true })

  for (const surface of ['terminal', 'desktop'] as const) {
    const ui = await $.ui.mount({ ...PANE, surface })
    expect((await ui.find({ type: 'Text', text: /Code matched in C/ }))?.text).toBe('Code matched in C')
    expect(await ui.find({ type: 'Text', text: /10,578 of 11,780 functions/ })).toBeDefined()

    // A category opens the treemap there; a unit zooms to its functions.
    await ui.press({ key: 'cat-Player' })
    expect(await ui.find({ type: 'Text', text: /^Player$/ })).toBeDefined()
    expect(await ui.find({ type: 'Text', text: /1\/3 functions · 68\.5% of 584 bytes · 2 left/ })).toBeDefined()
    await ui.press({ key: 'open-Player cue' })
    expect(await ui.find({ type: 'Text', text: /1\/2 functions/ })).toBeDefined()

    // A function opens its source in the sidebar pane.
    await ui.press({ key: 'open-PlayerCue_Run' })
    const src = await $.ui.mount({ ...PANE, requestId: 'decomp-source', props: { ...PANE.props, title: 'PlayerCue_Run' }, surface })
    const code = await src.find({ type: 'Code' })
    expect(JSON.stringify(code)).toContain('void PlayerCue_Run(void)')
    expect(JSON.stringify(code)).not.toContain('Other')
    await src.unmount()

    await ui.press({ key: 'crumb-0' })
    expect(await ui.find({ type: 'Text', text: /^All$/ })).toBeDefined()

    await ui.press({ key: 'tab-recent' })
    expect(await ui.find({ type: 'Link', text: /#546 Engine core/ })).toBeDefined()
    expect(await ui.find({ type: 'Text', text: /\+6 matches · 64\.50%/ })).toBeDefined()

    await ui.press({ key: 'tab-overview' })
    await ui.unmount()
  }
})

test('the footer button shows the headline', async ($, on) => {
  mock.store(on)
  on('command.register', () => ({ value: { command: 'decomp' } }))
  on('clock.every', () => ({ value: undefined }))
  on('http.fetch', (_, e) => ({ value: { status: 200, ok: true, headers: {}, text: fetchAnswer(e.url) } }))
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
