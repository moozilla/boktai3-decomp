# Worked example: the Sealed Prison, the first dungeon

The whole first dungeon, from the cell where Django wakes to the escape
cutscene: its 20 rooms, how they connect, what each teaches, and the story
progress values that drive it. Built from the door graph
(`python3 tools/assets/areas.py show 1`), the scripts, the cutscene tables
(`tools/assets/cutscenes.py`, [docs/CUTSCENES.md](../../docs/CUTSCENES.md)) and
story progress (`tools/assets/story.py`). Every room was loaded headless to
check it (`tests/scenarios/sealed_prison_rooms.txt`). In the Studio:
Places › Sealed Prison for the rooms, Story › Sealed Prison for the beats.

## The shape of it

The prison is a single chain. Each room has a door on to the next and a door
back, with two exceptions: room 2759 has a side room (2766), and the last two
rooms loop between the seal chamber and the escape. Master Otenko comes along
(he is the scene NPC `character 1` in the cutscenes) and stops Django the
first time each new thing appears: the Gun Del Sol, the skylight and the SUN
gauge, hint panels, wall clinging, spider webs, saving, levels, item thieves,
the map, weight switches, swords, crates, lever switches, Dark spirits, the
Solar Station. Then the dark seal, and the escape.

## How a room knows it is your first visit

Story progress (`r708[0x3C]`, `story` in the readable view) only goes up:
`advance_story(value=N)` sets `story = max(story, N)`. Each door picks an
entrance by testing it:

```
# scene 2698, the door to 2705
if story < 5:
    door(box=..., scene=proc_2705, entrance=57, ...)   # first visit: the lesson plays
else:
    door(box=..., scene=proc_2705, entrance=7, ...)    # every later visit
```

Entrance 57 runs the room's event (a cutscene) as you arrive, and the end of
that cutscene raises progress to the value the door tested, so the next time
the door leads to entrance 7. Entrance 8 is the way back. The rooms' progress
values follow the route: 4, 5, 6–7, 8, 9, 11, 12–14, 15, 17–18, 19–20, 21,
22, 23, 24, 25 and 27, then 26 and 28–30 in the escape. Values 10 and 16 are
never set; nothing tests them either.

## Room by room

| Room | Map | Cutscenes | Story set | Enemies | Items | What it is |
|---|---|---|---|---|---|---|
| 2698 | 7EFB | 5, 153, 172 | 4 | 1 ghoul | 2 Solar Fruit | Django wakes in his cell (cutscene 5, "Are you awake?"); first chest |
| 2705 | ED78 | 154 | 5 | 4 ghouls | 2 Solar Fruit | the Gun Del Sol and Solar Shots |
| 2712 | 2C6E | 155, 156, 157 | 6, 7 | 2 skeletons | Earth Fruit, Solar Fruit | a skylight and the SUN gauge; a hint panel |
| 2719 | E41D | | | 2 ghouls | | a corridor |
| 2724 | A6B5 | 158 | 8 | | | clinging to walls along a thin ledge; movable blocks |
| 2731 | 7EF2 | 159, 160, 173 | 9 | 1 ghoul, 2 spiders | | invisible spider webs; the Wizard frame for the Gun Del Sol (cutscene 160) |
| 2738 | 2C6E | 161 | 11 | 2 ghouls | 2 Earth Fruit, 2 Solar Fruit | saving |
| 2744 | BA0D | 162 | 12, 13, 14 | 2 skeletons | Solar Fruit | levels and Status Points |
| 2755 | 7EFB | 13, 14, 163 | 15 | | a chest | an item thief ("That thing just stole your items"); the dungeon map |
| 2759 | 6D6F | 164, 165 | 17, 18 | | | weight switches; a sword ("you can use a sword when you don't have any energy") |
| 2766 | 7EF2 | | | 4 spiders | Healing Potion | side room off 2759 |
| 2771 | E41D | | | 2 ghouls, 6 bats | | a corridor |
| 2776 | A6B5 | 166 | | | | crates the Gun Del Sol can't break; bamboo shoots and solar bugs |
| 2782 | 9C5B | 167, 168 | 19, 20 | 1 ghoul | | an iron grate and its lever switch |
| 2789 | 2C6E | 169, 170 | 21 | 3 ghouls | | a skylight, and some Ghouls |
| 2794 | 7EFB | 174 | 22 | 1 mimic | Earth Fruit, Speed Fruit | Dark spirits |
| 2799 | EF62 | 171 | 23 | | | a Solar Station and Solar Bamboo |
| 2803 | 5E57 | | 24 | 2 skeletons | | the last corridor |
| 2814 | 7EFB | 6, 7 | 25, 27 | 1 ghoul | | the dark seal: Otenko despairs (cutscene 6), the seal weakens (cutscene 7), break it |
| 2823 | 8EB1 | 8–12 | 26, 28–30 | | | the escape: a boy in a blue poncho, Otenko on Sabata, the Count returns |

Enemies and items are what the room's script places (`areas.py`,
`stages.py`); random item tables and drops are not counted.

## Two rooms in detail

**2744, levels.** Arriving sets story 12. The room arms an event
(`proc_1144(0, 46, ..., proc_2748, proc_2749, ...)`, not yet named) whose second
callback, `proc_2749`, sets 13; while story is below 13 a chest holds item 13
(0x0D, a Solar Fruit). Cutscene 162 ("Looks like you leveled up, Django") ends in
`proc_2615`, which sets 14.

**2814 and 2823, the seal and the escape.** Entering the seal chamber plays
cutscene 6 (story 25). Cutscene 7 ("The dark seal was weakened?") follows, and
its end sets 27. The door to 2823 tests a flag in `r708[0x6E]` to pick the entrance, and 2823's
door to San Miguel (scene 2569, entrance 117) is taken when progress is not 27
and none of three flags in `r708[0x6E]` is set; otherwise it leads back to 2814.
The escape's cutscenes (8–12) set 26, 28, 29 and 30. Story 30 is tested 14 times
across the game, more than any other prison value, which makes it most likely
the "prison finished" check.

## What this showed

* Story progress is the game's quest log: 173 values from 0 to 194, 165 of
  them set, most (125) at the end of a cutscene. `tools/assets/story.py` lists
  them all, and the Studio's **Story** tree shows the whole game by chapter.
* First visits are not flags but progress tests at the doors: the door before
  a lesson leads to entrance 57 until the lesson's value is reached.
* Still to name: `proc_1144` (a room event with callbacks, used in 2744 and 2803)
  and the item thief's script in 2755.
