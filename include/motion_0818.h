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
    u8 pad22[0xf7];
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
    u8 pad1D8[3], done, pad1DC;
    s8 byte1DD; /* +1dd: signed spin increment */
};

#endif
