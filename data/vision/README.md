# Vision labeling pass (2026-10-16)

Raw output of six Haiku agents: four labeled the 381 large sprites from
rendered sheets (captured or code palettes where known, false colors
otherwise), two labeled the rooms from a door-graph grouping into 76 areas
(each with its maps, enemies, items, strings, debug prints and screenshots).
Web walkthroughs were mostly unreachable, so area labels rest on the game's
own strings and debug prints.

Only high and medium rows went into `data/notes.csv` (sprites as `big`,
rooms as `script` notes on the scene entry procedure, with the area label).
Low rows are unreviewed guesses kept here as leads. `area` numbers refer to
that one grouping (doors between scenes less than 60 ids apart), not to a
game structure.
