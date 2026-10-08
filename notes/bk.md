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
