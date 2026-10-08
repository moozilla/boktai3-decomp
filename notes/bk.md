# bk notes (backlog retry, 45 matches)

## New general tricks
* **Bit-slot allocators/free-all loops: use real struct types** with the mask as a
  struct member and elements as `struct E e[N]`: `Tst(&b->mask, i)` (static inline u8 bool helper
  `if (*m & (1 << i)) return TRUE; return FALSE;`), then `e = &b->e[i]; b->mask |= 1 << i; e->idx = i; return e;`.
  This reproduces the reloaded mask and recomputed `1<<i` that pointer-cast versions (CSE'd) never gave.
  Declare `struct E *e;` at function top; if mask offset is large (>0x7c) agbcc hoists `&mask` into a register
  (declare `u32 *m = &b->mask;` first for free-all loops, order `s32 i = 0; u32 *m; struct E *e`).
  Free-all loops: `for (; i < N; e++, i++)` (e++ before i++ puts adds in orig order); returns `u32` with `return G = 0;` or
  `{ u32 z = 0; G = z; } return 0;` depending on whether the asm is `ldr; movs; str` or `movs r1; ldr; str; movs r0`.
* **Find loops** (`if (!b) return 0;` ... `one` register, two pointers): 080FF820/0806F3A8. Use `s32 one = 1;`
  passed to the bool helper (`m & (one << i)`) to hoist `movs r6,#1` after the mask load; use `u32 o = i*0x20;` for the
  returned address; a separate pointer `p += stride` only if the asm has two live pointers, otherwise index `b + i*stride`
  (08101E20: indexing `b + i * 0x19C + off` gives the two induction pointers the orig has).
* **Constant before address in a store** (`movs r0,#0xf; adds r1,r2,#0; adds r1,#0xaa; strb`): put the constant in a local
  `u32 v = 0xF; p->st = v;` inside the block (08024774 family).
* **Constant mask loaded before the ldrh** (`ldr r0,=0xFFFFF7FF; ldrh r1,[r2]; ands r0,r1`): helper
  `static inline void Clr(u16 *q, u32 m) { *q = m & *q; }` called as `Clr(ptr, 0xFFFFF7FF)` (080ED3A0, 08090A98, 08090E1C).
  For `ldrh r1; movs r0,#4; orrs r0,r1` use a struct-typed pointer `w->f = 4 | w->f;` (080E7620).
* Inverted bool branch: `if (Eq(a,b)) {...return 1;} ...return 0;` rather than `!Eq` (0810038C).

## Solved skips
080FFFE4 080FF820 0810033C 0810038C 08100BC8 08101E20 080D0F88 080E7620 080ED3A0 0806F400 0806A788 0806BC8C
0806F3A8 08065BEC 0808005C 08090A98 08090E1C (all of the priority-1 list), plus families:
* flag-take (`p[0xB0]` -> clear p[0xAF], state p[0xAA]=K, cnt++): 08024774 080235F4 0802367C 08023BA0 08024A9C 0802AFE8 0802B5C0 0817C358 0819E078
* free-all: 081280E4 0812A480 0812CD70 0812D4CC 081009B0 081230B8 08126918 08121AC8 081089D8 08126074 0812D960 0812975C
* alloc+memclear: 08127EFC 080FF7C4 08104220 08128B4C 0806DF60 0812876C 081084E4

## Permuter
Only run briefly on 08100BC8 (score 1355, not near 0) before the struct approach solved it. Needs `PERMUTER=/home/user/decomp-permuter`.

## Remaining (same families, not done)
* alloc with 5-arg memclear/stack arg: 0812A24C 0812B084. Per-slot callback loops calling 08249240 (08121A78 08121E28 ...),
  two-call free-all (080678B0 0812B27C 080695C8 081225A4), find loops without calls (08067148 08068480 ...),
  alloc loops 08121848 (copy of mask in r4) and its siblings. Generators used: build/fam/mk*.py (untracked scratch).
## Seeder skips
- sub_0811E8B0 (7): loop-invariant p+0x54 gets hoisted into a reg by agbcc, orig recomputes each iteration; 6 tries
- sub_0803FAC8 (6): arg2 copied into r4 around const-6 stack arg; couldn't force it, 4 tries
- sub_08249FA8 (4): epilogue is 'pop {r4,pc}' = compiled without -mthumb-interwork; build has no per-file flag (tooling gap). Rest matched except that
- sub_08086A2C (4): arg regs swapped (p in r5, n in r4 in orig; mine reversed), 7 tries
- sub_0813BA24 (4): ret value copied to r1 and 0x744 literal reused +4 for next addr; r/off regs differ, 9 tries
- sub_0822B4E8 (2): `ldr r1,=gUnk; ldrh r0,[r1,#0x3c]` (ptr in r1, value r0); every form gives ptr in r0; 4 tries
- sub_08019BB0 (2): arg copied to r2 (not r1) around ldr of global; 5 tries
- sub_08164738/sub_08165870 (3): `ldr r2,=0xD040; adds r1,r2,#0; strh` counter/copy pattern with p++ between stores; 7 tries
- sub_0816A25C (2): switch (a>>2) with case 2 body before case 1/3 but cmp-1 first; case order vs body order not reproducible, 5 tries
- sub_0814A854 (2): up-counting loop with `cmp r6,#2; ble` gets strength-reduced to downcount; 6 tries
- sub_08238470 (2): ptr?ldrsh:-1 compare; r4 temp used for ldrsh offset instead of r3; 3 tries
- fragments (not whole functions, tail-split): sub_0808A9EC sub_080C09F0 sub_0813436E sub_08219624 sub_0821962C sub_08249210 sub_08248970 sub_0808A210
- sub_081C674C (2): r2 holds copy of old counter value, r4/r5 pushed; 4 tries
- sub_0811EE68 (2): args shuffled (b->r0, n->r1) before cmp, like an inlined helper; 3 tries
- sub_080F4A0C (2): byte stores with last address computed as 0xf8+0x97 (reg reuse); mine uses literal pool; 3 tries
- sub_081B6810 (2): find-first-free-slot loop; out ptr in r4, stride r5, `bne next` layout; 3 tries
- sub_081FB82C (2): CpuSet(&zero) with stack zero stored after `adds r1,#0xc`; 3 tries
- sub_0813DB60 (2): p/d swapped regs (r3/r2), return-d layout; 3 tries
- sub_081A55D4 (3): loop with 5-arg stack call, copies r4/r5 allocation swapped; permuted decl order, 24 tries
- sub_0818E1BC (3): flag-take with 0x119 bit0, 'ands r0,r1' operand order; 5 tries
- sub_08210C5C (3): nested const-chain select; only r2/r3 swapped (p in r3, r in r2); 6 tries
- sub_080F6118 (3) same flag-take-0x119-bit0 as sub_0818E1BC (-2 mask not narrowed to 0xfe)
