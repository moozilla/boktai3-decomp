#include "global.h"

struct Pos {
    s16 x, y, z, w;
} __attribute__((aligned(4)));
s32 Div(s32, s32);

s32 sub_0821D178(struct Pos *out, struct Pos *from, struct Pos *to)
{
    struct Pos p;
    s32 x0, z0, x1, z1;
    s32 dx, dz;
    s32 fx, fz, rx, rz;
    u32 flags;

    dx = to->x - from->x;
    dz = to->z - from->z;
    p.y = from->y;
    fx = (u8)from->x;
    fz = (u8)from->z;
    rx = 0x100 - fx;
    rz = 0x100 - fz;
    x0 = from->x & 0xFF00;
    z0 = from->z & 0xFF00;
    x1 = x0 + 0x100;
    z1 = z0 + 0x100;
    if (to->x >= x0 && to->x <= x1 && to->z >= z0 && to->z <= z1) {
        *out = *to;
        return 1;
    }
    if (dx == 0) {
        if (dz == 0) {
            *out = *to;
            return 1;
        }
        if (dz > 0)
            p.z = z1;
        else
            p.z = z0;
        p.x = from->x;
        *out = p;
        return dz > 0 ? 2 : 3;
    }
    if (dz == 0) {
        if (dx > 0)
            p.x = x1;
        else
            p.x = x0;
        p.z = from->z;
        *out = p;
        return dx > 0 ? 5 : 4;
    }
    flags = 0x100;
    if (dx > 0)
        flags = 0x1000;
    if (dz > 0)
        flags |= 0x10;
    else
        flags |= 1;
    if (flags & 0x10) {
        p.x = from->x + Div(rz * dx, dz < 0 ? -dz : dz);
        p.z = z1;
        if (p.x >= x0 && p.x <= x1) {
            if ((p.x & 0xFF00) > (from->x & 0xFF00))
                p.x = from->x | 0xFF;
            *out = p;
            return 3;
        }
    }
    if (flags & 1) {
        p.x = from->x + Div(fz * dx, dz < 0 ? -dz : dz);
        p.z = z0 - 1;
        if (p.x >= x0 && p.x <= x1) {
            if ((p.x & 0xFF00) > (from->x & 0xFF00))
                p.x = from->x | 0xFF;
            *out = p;
            return 2;
        }
    }
    if (flags & 0x100) {
        p.x = x0 - 1;
        p.z = from->z + Div(dz * fx, dx < 0 ? -dx : dx);
        if (p.z >= z0 && p.z <= z1) {
            if ((p.z & 0xFF00) > (from->z & 0xFF00))
                p.z = from->z | 0xFF;
            *out = p;
            return 4;
        }
    }
    if (flags & 0x1000) {
        p.x = x1;
        p.z = from->z + Div(dz * rx, dx < 0 ? -dx : dx);
        if (p.z >= z0 && p.z <= z1) {
            if ((p.z & 0xFF00) > (from->z & 0xFF00))
                p.z = from->z | 0xFF;
            *out = p;
            return 5;
        }
    }
    return 0;
}
