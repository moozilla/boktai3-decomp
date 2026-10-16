# GCL: the game's scripts

Boktai 3's scene scripts are compiled **GCL**, the scripting language Konami
(KCEJ) made for Metal Gear Solid: the hash function, the command hashes
(`if`, `eval`, `return`, `chara`, `mesg`, `trap`, `load`, ...) and the
command-and-block interpreter all match (docs/ENGINE.md, "Command names";
docs/MGS_GCL_MATCHING.md). The compiled image is the `gcx` resource
(`08D5A85C`, `tools/assets/gcx.py`); the asset type's own hash is `gcl`.

This directory holds what we recover of the source:

* `names/`: names of hashed identifiers, one CSV per kind (below).
* `prints.csv`: English for the scripts' Japanese debug `print` strings
  (`text` as `gcx.py` prints it, `english`), shown as comments by `gcx.py dis`
  and in the Studio. `print` outputs nothing in this ROM: its handler
  (`0821B858`) evaluates the operands and converts strings to Shift-JIS into a
  stack buffer, then drops them; the output call was compiled out.
* later: `src/`, readable GCL that compiles back to the exact bytes.

## Names and hashes

GCL hashes a name with `h = rotl16(h, 5) + byte` (`sub_08219BD8`) over its
**EUC-JP** bytes. GCL source was EUC-JP (KCEJ GCL reference), and the actor
keys confirm it: the game's katakana words hit 46 registry keys hashed as
EUC-JP, against 12 as Shift-JIS, 6 as UTF-8 and 13 expected by chance.

| File | Keys | Used by |
|---|---|---|
| `names/chara.csv` | `data/actor_registry.csv` (722) | `chara KEY NAME ...` creates the entry's actor; `command KEY ...` calls its routine and keeps the result |
| `names/command.csv` | native command ids | the block interpreter |

Columns: `key, name, english, confidence, evidence`. `name` is the original
GCL spelling (Japanese where the original was), `english` a translation for
reading, using the fan translation's terms where it has them (the monster list,
strings 9838-9921). A key may have several rows when different words hash
alike; the first with the best confidence is shown, the rest stay as
alternatives.

* **high**: the hash and independent evidence agree: the sprite the actor
  loads, its callback's behavior, or a debug print in the same procedure
  that creates it.
* **medium**: a coherent game word plus weaker context (a monster-list name, a
  cluster of related callbacks such as the bank, loan and storage at
  `081BC`-`081BF`).
* **low**: a hash match only.

A random word hits one of the 722 keys about 1.1% of the time, so a hit alone
is a lead. `python3 tools/assets/gclnames.py check` verifies every row;
`crack chara` lists corpus words that hash to unnamed keys (the text bank and
the scripts' debug prints) with the chance rate; `find HASH` and `hash TEXT`
help by hand. The Studio shows the names on the Actors and Scripts pages, and
`gcx.py dis` prints them with the key and English in a comment:

```
chara 扇風砲マネージャ 0x9cc2  # 9CC2 Fan Cannon Manager
local1 = (({ command ランダム p: (arg2 << 4) /* 3469 Random */ } >> 4) % arg2)
```

## Options

A command's options are a letter and values, written Konami-style by
`gcx.py dis` (`-p 2688,512,2432 -d 128`). The compiled script keeps only the
letter, so letters are the source spelling (MGS1's GCL also used letters).
What a letter means belongs to the routine that reads it:
`tools/assets/gcloptions.py KEY|COMMAND` scans the routine's code for
`Script_SeekToKeyword('p')` / `Script_GetKeywordValue('d', 256)` calls (the
letter, its default, how many values) and takes names from the matched C
(`radius = Script_GetKeywordValue('d', 256)`). `names/option.csv` (owner,
letter, name, english, evidence) adds names by hand and wins. The comment
lists what is known:

```
chara 0x99ca 0x355f -p 2688,512,2432 -d 128 -t 15 -T 4  # 99CA OrbitingParticles_CreateFromScript: -p pos, -d radius, -t duration, -T type
trap arg1 プレイヤー -m 0x14c9 -s $:P_ブロック名  # F5EB Player: -m event, -s names, -e run block
command 0x1c64 -r text#1800  # 1C64 sub_08030D9C: -r text; text#1800 "Destroy the bats on the 4 windows in the north …"
```

Keys without a recovered name show their callback's function name. `text#N`
operands get a short excerpt of the string (English when
`build/text/english.tsv` exists). About a third of the option uses in the
scripts are explained so far; the two most used routines without names are
`1C64` (message window) and `7348` (`SceneMessages_ScriptStart`).

Operand roles worth knowing:

* `chara TYPE NAME`: the registry key, then the instance's own name hash
  (MGS's `chara TYPE NAME` too; `mesg` addresses instances by that name).
* `trap AREA WHO`: `WHO` is a registry key, usually プレイヤー (F5EB, 82 of 101
  traps). `-m` is the event, default `0DD2`; `？` (14C9) and `＊` (1516) are
  the EUC-JP hashes of the full-width wildcards the source wrote.
* "chara" is GCL's word for any scripted object (キャラ); the Studio's Actors
  page is the same registry.

## Konami's naming, from the game's own debug prints

The scripts' `print` strings are EUC-JP developer messages that often print a
variable with its source name, in GCL's own syntax. Each label comes right
before the variable it names, so they tie names to storage:

| Source form | Example | Storage it labels |
|---|---|---|
| `$f:NAME` | `$f:st02_53_燭台シャッターフラグ` | `r708[...]:t4` (flag) |
| `$b:NAME` | `$b:st05_140エリア脱出利用カウンタ` | `r708[...]:t2` (byte) |
| `$w:NAME` | `$w:story`, `$w:現在ステージ` | `r708[0x3c]:t1`, `gs[0x740]:t1` (word) |
| `$$f:` / `$$b:` | `$$f:切替レバー中央状態` | `r700[...]` (the `$$` variables live at `02000700`) |
| `$:P_NAME` | `$:P_ブロック名`, `$:P_階数` | `argN`: procedure parameters take a `P_` prefix |
| `$:NAME` / `$$:NAME` | `$:キャラ名`, `$$:燭台状態` | arguments and locals |
| `NAME[INDEX]` | `$f:燭台点灯フラグ[d:st02_燭台スイッチ番号:53_燭台１]` | an indexed flag array |
| `d:GROUP:NAME` | `d:STORY_05_130:氷パズル正解_終了` (= 5) | a define; values print beside it |
| `@NAME` | `@st01_43_ruins_after` | a procedure (`@` calls, as in MGS4 source) |

### Variables, defines and parameters from the prints

`python3 tools/assets/gclnames.py labels` walks every `print` in the image and
pairs a label at the end of a string operand with the operand that follows
it; `labels --write` regenerates the tables below, keeping any English
already in them (English comes from `prints.csv`, then hand edits).

| File | Rows | Columns |
|---|---|---|
| `names/var.csv` | `$x:` / `$$x:` labels on `r708`, `r700` and `gs` storage | `storage` (`r708:0x66.6:t4`; an indexed flag array as `r708:0x145.0:t4(16)`), `name`, `english`, `confidence`, `conflict`, `evidence` |
| `names/define.csv` | `d:GROUP:NAME` printed beside a literal, and the `d:` index labels of flag arrays (`[d:st02_燭台スイッチ番号:27_燭台１]` with `(16,1)` = 1) | `define`, `value`, ... (`evidence` names the array for index defines) |
| `names/param.csv` | `$:P_NAME` / `$:NAME` on `argN` and `$$:NAME` on `localN`, per procedure | `proc` (call id), `storage`, `name`, ... |

Pairing rules: the label must end its string (trailing spaces, `！？・…`
allowed; the sentence tails `です` and `が不正な値になっている` are cut and
lower the row to medium); the sigil must agree with the storage (`$$` =
`r700`, `$` = `r708` or `gs`; `f` = t4, `b` = t2, `w` = t1), or the row is low.
An array description (`$b:敵設置数登録配列[]の、`) and a label with no operand
(`$f:st04_08_重量スイッチオン`) are skipped. A slot with several names, or a
name on several slots, keeps every row and says so in `conflict` (defines
compare within their group, parameters within their procedure): `proc 4969`
prints `$:P_ブロック名` for `arg2` and, inside a `trap` event block, `arg11`.

The three `@` prints (`@st01_43_ruins_after`, `@st01_43伯爵戦前へ` in
`proc_3239` and `proc_3255`) describe a common-load destination; the code
does not tie them to one procedure, so there is no `proc.csv` yet.

With names, `gcx.py dis` (and the Studio's Scripts page) prints named storage
Konami-style, the raw storage and English in the trailing comment; an index
prints as its define when the procedure's prints (or a unique define) say
which, or as a named parameter:

```
$f:st02_27_燭台シャッターフラグ = 1  # r708[0x66.6]:t4 st02_27_candle shutter flag
if (($f:燭台点灯フラグ[$:P_燭台スイッチ番号] == 1)) {  # r708[0x145.0]:t4(16) candle-stand lit flag; arg2 candle switch no.
proc_1084 0xae2c 2816 2304 7 $f:床スイッチフラグ[d:st05_床スイッチ番号:東シャッター連動134] ...
```

`gcx.disassemble(..., names=False)`, which tools parse, is unchanged.

Patterns worth following when we name things:

* Stage-scoped names start with the stage and area: `st02_27_`, `st05_130`
  (stage 05, area 130), `tw00_09_town`, `rd00_J`.
* Names are mostly Japanese nouns with English loanwords in katakana:
  `...フラグ` (flag), `...状態` (state), `...番号` (number), `...種類` (kind),
  `...カウンタ`, `...マネージャ` (a manager actor: `デモマネージャ`,
  `扇風砲マネージャ`), `..._終了` (finished).
* Procedure names seen so far are ASCII (`st01_43_ruins_after`); actor and
  routine names are Japanese (`スライム`, `ランダム`, `名前入力`).

Community MGS sources show the same shapes: `proc NAME { ... }`, `@NAME`
calls, `-x` options, `$b:`/`$i:` typed variables, `d:` defines, `'...'`
strings, `#` comments ([KCEJ GCL examples](https://github.com/metalgeardev/KCEJ_Wiki/blob/master/common/gcl/examples.md)).
The readable GCL in `src/` will use that syntax, with placeholders where a
name is not recovered (`0x2502` keys, `r708[0x70]` variables, `proc_884`).

## Not names

Script procedures are table slots, not hashes: a procedure's number cannot be
cracked, so its name will be one we choose (in the style above) unless a debug
print gives the original. The font is not in JIS order either: its glyph
order is the game's own, roughly by radical (`data/charmap.tsv`).
