# depin-manual-5c worklog (2026-10-21)

List: wip/depin_list5.tsv (40 functions). Stopped early on coordinator request.

* depin.py --write over all 40: fewer hints for CharacterPreview_InitializePalette (2 -> 1), SceneHud_FinishActionSlide (9 -> 5), sub_08235B34 (4 -> 1). All committed, check.py MATCH.
* Nothing else reached clean C. SceneHud_FinishStatBRefill (0816F5B0) does not match as committed (skipped).
* sub_08235B34 with its last pin removed: 10 bytes differ (amount/step swap r5/r7); source also contains an uninitialised-read `if (step)` trick that is questionable.
* Permuter batch (stripped copies in wip/depin5/) was started and stopped after the first function; no results.
