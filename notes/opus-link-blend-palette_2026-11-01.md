# LinkPlayer_BlendPalette (08239848) and its Player twin sub_0813C9B4: matched without pins

Tags: #matching #twin #depin #palette #himode-narrowing #reload

**Matched.** `LinkPlayer_BlendPalette` (472 bytes) is byte-identical in
ordinary C. The same body replaces the pinned `sub_0813C9B4` in
`src/player_3.c` (the same code over `struct Player`), which removes 13 register
pins and barriers. Neither function has pins, barriers, selector macros or
compiler overrides.

## What decided it

The blend reads each source channel through the color pointer, as it already
did for the base color:

```c
red = *source & 31;
brightness = *brightnessPtr;
r = red + brightness;
...
g = ((*source >> 5) & 31) + brightness;
b = ((*source >> 10) & 31) + brightness;
br = (*base) & 31;
bg = ((*base) >> 5) & 31;
```

The ROM has two pseudos for the mask 31 in each loop. One (r7 in loop 1) serves
the four shifted channels. The other, unallocated, serves the two unshifted
reds, and reload rematerializes it into each and's output
(`movs r0, #0x1f; ands r0, r2`). Both pseudos come from how the u16 operations
expand:

* `c-typeck` narrows the u16 `&` and `>>` to HImode. `expand_binop` forces the
  expensive constant into an HImode pseudo. Thumb has no `andhi3`, so it widens
  the operation to SImode.
* `(*source >> n) & 31`: the HImode first operand is a SUBREG of the shift
  result, so `expand_binop` swaps the operands. The and then uses
  `(subreg:SI (reg:HI K))` directly. cse merges all four into the first HImode
  constant.
* `*source & 31`: `-fforce-mem` loads the operand into an HImode register, so
  no swap happens. The andsi3 expander copies the constant into an SImode
  pseudo. cse keeps it in the SImode class, apart from the HImode one, and the
  base red joins it.

The draft that took the color by value (`u16 source`, with
`packed = source << 16; g = (packed >> 21) & 31`) made g and b SImode. That
merged them into the reds' SImode constant, and every and tied to a copy of
the color.

The masked red also needs its own temporary before the brightness add.

The main function keeps the form found earlier:

* the parameter is copied into `source` after the early return (source goes
  to r8);
* `i = 0` is set before the palette pointer;
* a `u16 **palette` local;
* the brightness pointer locals;
* loop 2 is `while (1) { ...; if (++i > 15) break; }`, which keeps the
  up-count.

## Ruled out

Each variant is the match with one change. Numbers are scope's ROM / draft
differing instructions.

| Variant | Change | Differing |
|---|---|---|
| w2 | color by value, g and b from `packed = source << 16` | 15 / 17 (shape 6) |
| z3 | as w2, with the masked red in its own temporary | 6 / 8 (shape 6) |
| a1 | `brightness = *brightnessPtr; r = (*source & 31) + brightness;` | 26 / 26 (registers only) |
| a2 | `r = *source & 31; brightness = ...; r += brightness;` | 14 / 14 (registers only) |
| a4 | a temporary for every source channel | 24 / 24 (registers only) |
| a6 | `u8 brightness` | 37 / 37 |
| d1 | no `input` copy (the parameter is `source`) | 50 / 50 |
| d2 | `for (i = 0; ...)` after the palette pointer | 73 / 79 |
| d3 | no `palette` local | 6 / 6 |
| d4 | loop 2 as a `for` | 73 / 79 |
| d5, d6 | `&p->brightness` in the call, no brightness local | 6 / 6 (shape 12) |
| w1, w3 | the old twin's `channelMask` local without pins | 161 / 173 |

These variants match with the same code: `i == 5 || i == 6 || i == 13` (fold
turns the first two into `(u32)(i - 5) <= 1`), `i < 16`, and `base[0]`.
