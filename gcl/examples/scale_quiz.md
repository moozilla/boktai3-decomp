# Worked example: the Blackbeard Brothers' scale quiz

A complete puzzle, decoded from the scripts and read end to end. The code
blocks are the readable view (`python3 tools/assets/gclview.py PROC`, or the
Studio's Scripts page), trimmed; `# ←` comments are mine. Names marked *ours*
are names we chose; the rest come from the game (debug prints, hashes).

## The story

On one floor of the Immortals' tower, three ghosts — the Blackbeard
Brothers, skeletons without beards (Johnny the ghost explains the joke) —
block a door. Each brother asks a question, and you answer with a scale:
push blocks down a hole in the east room until their weight matches what the
question asks for, then tell Johnny you're done.

* **Jack** (third brother, an *arithmetic* problem): match the weight of the
  monsters in the west room. Answer: **5**.
* **George** (second brother, a *logic* problem): the same, with other
  monsters. Answer: **7**.
* **John** (eldest, an *atonement* problem): match "the weight of the souls of
  all the monsters you've defeated". There is no right answer — every answer
  is accepted, and the brothers only comment on whether you pushed none, some
  or all of the blocks.

## The pieces

| Procedure | What it is |
|---|---|
| 5764 | the scale room's scene entry |
| 5756 | stage setup: map, palette, camera |
| 5765 | doors and entrances (which room you come back to depends on the quiz stage) |
| 5757 | the scale: six blocks and the trap that weighs them |
| 5737, 5741, 5745 | Jack, George, John ask their question and set the answer |
| 5761 | judging: compare the weight with the answer |
| 5762 / 5763 | the "wrong" and "right" cutscenes |
| 5752 | the west room: which monsters stand there for each question |
| 4949 | Johnny's lines, by conversation number |
| 5040, 5010-5012 | John's question: the brothers' reaction to none / some / all |
| 4932 | *ours*: "update an area's story flag" (from its debug prints) |

Two variables carry the whole puzzle. Their names are the developers' own,
printed by the scripts' debug messages:

* `scale_weight` — `$$b:はかり重量` "scale weight", `r700[0x2f4]`: the running
  total on the scale.
* `st05_440_area_story` — `$b:st05_440エリアストーリー`, `r708[0x158]`: how far
  you are in this area's story (saved with the game). Its values are named by
  defines the scripts print: question 1 asked = 2, solved = 5; question 2 asked
  = 6, solved = 8; question 3 asked = 9, solved = 11.
* `st05_440_area_correct_weight` — `$b:st05_440エリア正解重量` "correct weight",
  `r708[0x15b]`: the answer to the current question.

## 1. Entering the room (5764)

```python
proc_861(1)                 # ← every scene starts with these two
proc_864(0)
proc_553()
proc_1265()
proc_1()
if scene_is_set_up() == 0:  # ← *ours*; scene setup runs once
    proc_5756()             # stage
    proc_5765()             # doors
    proc_905()
    proc_5757()             # the scale
    proc_1048(33)
```

`5756` loads the stage the usual way:

```python
load_stage(map=0xee12, tileset=0x1ad5, hp=0xd5a3)
set_stage_palette(palette=0x2f53, 0, 0)
set_camera_area(box=0xa679)
```

## 2. The scale (5757)

```python
proc_4941(0x3438, 1, 0, 2)   # ← six pushable blocks, by instance name (and probably a cell)
proc_4941(0x3439, 1, 3, 2)
proc_4941(0x968d, 1, 1, 0)
proc_4941(0x968e, 1, 4, 0)
proc_4941(0x23d8, 1, 2, 4)
proc_4941(0x23d9, 1, 5, 4)
scale_weight = 0
trap(area=0x1123, who=ANY, event=0xdd2):     # ← box 0x1123: the hole in the east room
    on_trigger:
        match character_name:                 # ← which object fell in ($:キャラ名)
            case 0x3438:
                scale_weight = scale_weight + 1
                # log: Wooden box 00 -> {scale_weight}
            case 0x3439:
                scale_weight = scale_weight + 1   # wooden box 01
            case 0x968d:
                scale_weight = scale_weight + 3   # rock 00
            case 0x968e:
                scale_weight = scale_weight + 3   # rock 01
            case 0x23d8:
                scale_weight = scale_weight + 5   # iron box 00
            case 0x23d9:
                scale_weight = scale_weight + 5   # iron box 01
            case _:
                # log: What on earth fell down?
```

So the blocks weigh **wooden 1, rock 3, iron 5**: two of each, 18 in all. A
`trap` is GCL's trigger: when anything (`who=ANY`, the hash of `？`) enters
the hole, the block runs with the object's name, and the script adds its
weight. The debug prints spelled out every case in Japanese
(`木箱00 → $$b:はかり重量`), which is how the variable got its name.

## 3. A brother asks (5737, Jack)

```python
sub_08030D9C(text=text(5368))   # "I am the third Blackbeard Brother, Jack.
                                #  I have an arithmetic problem for you: Push blocks with
                                #  the same weight as the monsters in the west room..."
if st05_440_area_story < 2:
    proc_4932(23, 2)                    # story = 2: question 1 asked
    st05_440_area_correct_weight = 5    # ← the answer
```

George (5741) sets story 6 and answer **7**; John (5745) story 9 and answer 0
(a placeholder: see below). `sub_08030D9C` is the message window (`1C64`).

## 4. The west room (5752)

Which monsters stand in the west room depends on the question:

```python
if st05_440_area_story >= 11:
    # log: No more questions...
elif (st05_440_area_story >= 2) and (st05_440_area_story < 5):   # Jack's
    proc_1561(0x9e1a, 0x88a7, 2, 3, 1, 0, 0)    # ← slimes (proc 1561 places a slime by color)
    proc_1561(0x9e1b, 0x88a8, 2, 4, 0, 0, 0)
elif (st05_440_area_story >= 6) and (st05_440_area_story < 8):   # George's
    proc_1500(0x1032, 3, 0, 3, 0, 0x35c, 0x35c)  # ← Boks (ghouls)
    proc_1500(0x1033, 4, 0, 4, 0, 0x35c, 0x35c)
elif (st05_440_area_story >= 9) and (st05_440_area_story < 11):  # John's
    proc_1532(0xe4b3, 0, 2, 0, 3)
    proc_1578(0x4979, 1, 0, 0, 3)
```

How a monster's "weight" is meant to be read (5 for Jack's pair, 7 for
George's) is part of the riddle and isn't in these procedures; the answers
are what the script checks.

## 5. Judging (5761)

You talk to Johnny ("Ready for judgement?") and this runs:

```python
if arg1 == 0:                            # ← "Yes"
    if r700[0x2f5] == 10:                # ← question 3 (John's)
        if scale_weight == 0:
            st05_440_area_correct_weight = 0     # pushed nothing
        elif scale_weight < 18:
            st05_440_area_correct_weight = 1     # pushed some
        else:
            st05_440_area_correct_weight = 2     # pushed all 18
        # log: Question 3 forced correct!
        proc_4932(23, r700[0x2f5])       # story → 10
        proc_5763()                      # the "correct" cutscene
    elif scale_weight < st05_440_area_correct_weight:
        proc_5762(-1, 464)               # too light
    elif scale_weight > st05_440_area_correct_weight:
        proc_5762(1, 468)                # too heavy
    else:
        proc_4932(23, r700[0x2f5])       # right: story → 4 or 7
        proc_5763()
else:
    proc_4949(13, 0)                     # "You quitting?"
```

`r700[0x2f5]` is the story value a correct answer moves to. Johnny's
dialogue (5758) sets it with `proc_5759(4, 14)`, `(7, 16)` or `(10, 18)`
depending on the stage, along with the line he says afterwards. The prints'
"solved" defines (5, 8, 11) are reached later, when you go back to the
brothers.

For John's question the variable that held the answer is reused to remember
*how* you answered, and `5040` picks the brothers' reply from it:
`5010` "You didn't push down a single moving block... nobody can measure the
weight of something like a soul", `5011` "That's your answer? Isn't it a
little conceited to think you can measure the weight of a soul?", `5012` "You
pushed down every moving block... Guess you're self-conscious about the
weight of your sins?". All three end with the brothers asking you to help
Sabata.

## 6. The cutscenes (5762, 5763)

Both are small state machines: an actor (`sub_082284B4`, a script timer)
runs its block every few frames, and the block steps a counter and returns
how long to wait before the next step:

```python
r700[0x2f7] = 0
spawn sub_082284B4(name=0x9f84, delay=1, a=[arg1, arg2]):
    e:
        match r700[0x2f7]:
            case 0:
                SceneObjects_ScriptDeactivate(id=0xcb37)   # ← an object also set up by proc_1100 with door box CD23
                r700[0x2f7] = 1
                return 120                                  # wait 120 frames
            case 1:
                sub_0822AEF8(i=106)                         # a sound?
                r700[0x2f7] = 2
                return 30
            case 2:
                sub_082266D4(pos=(x=gs[0x30], y=gs[0x32] + (5120 * arg1), z=gs[0x34]), value=30)
                r700[0x2f7] = 3                             # ← something at the player's position,
                return 60                                   #   above or below by arg1 (light / heavy)
            case _:
                # log: Incorrect demo ended
                proc_957()
                return 0
```

`gs[0x30]`, `gs[0x32]`, `gs[0x34]` are the player's x, y, z. This
return-the-delay pattern is how GCL scripts sequence cutscenes.

## What this example taught the tools

* Debug prints name variables precisely; the readable view turns them into
  comments, and their labels into names.
* `trap ... who=` uses the wildcard hash `？` (ANY), not only actor keys.
* A `match` on an instance name (`character_name`) is how a trap tells
  objects apart — instance names are a namespace worth cracking next
  (`0x3438` = "wooden box 00"?).
* Still unnamed and worth naming: the timer actor `sub_082284B4` (`430D`'s
  sibling), `proc_4941` (place a pushable block), `proc_1016`/`1019`/`1025`
  (dialogue setup), `proc_957` (end a cutscene).
