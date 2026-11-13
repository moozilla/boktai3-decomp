# Menus, link and hardware unit cleanup

Tags: #cleanup #menus #wireless #types

## Result tutorial: 0801D104–0801E000

Consolidated 30 single-function files into `src/result_tutorial.c`. The shared
`ResultMenu` task now exposes tutorial phases, timers, fixed-point player and target
positions, input state, prompt state, resources, fifteen existing `SceneInputWidget`
objects, and four marker sprites. Unknown regions remain explicitly unknown.
All functions already carried descriptive ResultTutorial names.

Every original function passed its baseline check; the merged unit matches all
3,836 bytes. Existing blink gotos remain: a direct interval expression compiles
to a range subtraction, and nested ifs redirect the early branch through the
boolean comparison. Both differ from the original. Named widget pointers preserve
the base-plus-flags addressing. A named difficulty pointer also preserves the
initializer store order without its former unused-local scaffold.

No concession added. Exact-code coverage gain: 0.0 percentage points; this is
source organization and type recovery. The coordinator runs the combined ROM build.
