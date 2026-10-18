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
    u8 pad7[0x15];
    s16 x, y, z; /* +01c: current position */
    u8 pad22[0x30];
    u16 field52;
    u8 pad54[0x52];
    u16 fieldA6;
    u8 padA8[0x58];
    s16 velocityX; /* +100: per-update X displacement */
    u16 pad102;
    s16 velocityZ; /* +104: per-update Z displacement */
    u8 pad106[0x13];
    u8 flags119, angle, pad11B[3], byte11E, byte11F;
    u8 pad120[0x14];
    u16 endX, endY, endZ, pad13A;       /* +134: endpoint */
    u16 startX, startY, startZ, pad142; /* +13c: segment origin */
    s16 dx, dy, dz;
    u16 pad14A;
    u16 fx, fy, fz, pad152; /* +14c: initial owner-relative offset */
    u16 a, b, c, pad15A;    /* +154: phase-dependent scratch vector */
    u8 *parent;             /* +15c: owner; position at +5ac */
    u8 pad160[0x70];
    void (*callback)(struct Motion0818Object *); /* +1d0 */
    u16 pad1D4;
    s16 timer; /* +1d6 */
    u8 targetRotation, pad1D9; /* +1d8: turn target */
    s8 turnDirection; /* +1da: wrapping turn increment */
    u8 done, pad1DC; /* +1db: phase-dependent signed turn-step count */
    s8 byte1DD; /* +1dd: signed spin increment */
};

/* Packed temporary owner displacement used by the final motion segment. */
struct Motion0818Displacement {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
};

#endif
