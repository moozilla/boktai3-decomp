#ifndef GUARD_MOTION_0818_H
#define GUARD_MOTION_0818_H

#include "global.h"

/* Observed layout for the 0818D754 -> D8E0 -> D960 -> DB10 callback chain.
 * This address-qualified type does not identify an actor species. See
 * docs/MOTION_0818.md for consumers, phase-dependent fields and sibling limits.
 */
struct Motion0818Object {
    u8 pad0[6];
    u8 rotation; /* +006: wrapping orientation byte */
    u8 pad7[5];
    void *drawable; /* +00c: palette resource used by the damage callback */
    u8 pad10[0xC];
    s16 x, y, z; /* +01c: current position */
    u8 pad22[0x30];
    u16 field52;
    u8 pad54[0x16];
    u16 speed; /* +06a: copied from the attacking collider */
    u8 pad6C[0x3A];
    u16 fieldA6;
    u8 padA8[0x38];
    u16 invulnerability; /* +0e0: suppresses damage while nonzero */
    u8 padE2[0x1E];
    s16 velocityX; /* +100: per-update X displacement */
    u16 pad102;
    s16 velocityZ; /* +104: per-update Z displacement */
    u8 pad106[4];
    s16 health; /* +10a: damage callback's remaining health */
    u8 pad10C[0xD];
    u8 flags119, angle, pad11B[3], byte11E, byte11F;
    u8 pad120[8];
    u16 paletteTimer; /* +128: hit palette restoration delay */
    u8 pad12A[0xA];
    u16 endX, endY, endZ, pad13A;       /* +134: endpoint */
    u16 startX, startY, startZ, pad142; /* +13c: segment origin */
    s16 dx, dy, dz;
    u16 pad14A;
    u16 fx, fy, fz, pad152; /* +14c: initial owner-relative offset */
    u16 a, b, c, pad15A;    /* +154: phase-dependent scratch vector */
    u8 *parent;             /* +15c: owner; position at +5ac */
    u8 pad160[0x6C];
    void (*paletteCallback)(struct Motion0818Object *); /* +1cc */
    void (*callback)(struct Motion0818Object *); /* +1d0 */
    u16 pad1D4;
    s16 timer;                 /* +1d6 */
    u8 targetRotation, pad1D9; /* +1d8: turn target */
    s8 turnDirection;          /* +1da: wrapping turn increment */
    u8 done, pad1DC;           /* +1db: phase-dependent signed turn-step count */
    s8 byte1DD;                /* +1dd: signed spin increment */
};

/* Packed temporary owner displacement used by the final motion segment. */
struct Motion0818Displacement {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
};

#endif
