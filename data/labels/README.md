# Labels from a researched pass (2026-10-16)

Codex (`codex exec`, gpt-6-luna) browsing the web in the user's Chrome. Its
sources were the GameFAQs walkthroughs for this game (Devis0r, GX01), the
Boktai wiki, and Darth Nemesis's guide in the translation project
(`boktai3trans/shinbok guide`: dungeon maps, monster list with locations,
item pages). It worked from rendered sprite sheets in exact palettes (the
default from the sprite data, variants the game selects:
`data/sprite_palettes.csv`), the enemy spawner scripts (palette = base + the
`C` option), and per-area room dossiers (strings, debug prints, doors).

| File | Rows | Columns |
|---|---|---|
| `areas.csv` | the 76 door-graph areas (`tools/assets/areas.py`) | area, place, story_order, confidence, evidence, source |
| `scenes.csv` | rooms, judged one by one | scene, area, place, floor, label, confidence, evidence, source |
| `sprites.csv` | the 377 large sprites (98F5) | hash, label, confidence, frames, evidence, source |
| `palettes.csv` | sprite palette pool entries, per sprite | pool, hash, label, confidence, evidence, source |

High and medium rows are also Studio notes in `data/notes.csv` (kinds `big`,
`pool`, `script`). Low rows are leads, not facts. A first pass of room labels
was discarded because it came from a keyword script; these rows were made
under rules against that.
