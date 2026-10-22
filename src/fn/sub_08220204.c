#include "global.h"

struct Collider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x2C];
    u32 resist;
    u32 attr;
    u16 power;
    u16 damage;
    u8 pad40[2];
    u8 height;
    u8 hitKind;
};

// Computes the damage `def` takes from `atk` (attribute vs. resistance), once per hit.
void sub_08220204(struct Collider *atk, struct Collider *def)
{
    s32 dmg;
    u32 attr, defAttr, resist;

    if (def->damage != 0)
        return;
    {
        u32 m = 0x1000;
        if (atk->attr & m)
            dmg = atk->power;
        else
            dmg = atk->power - def->power;
    }
    if (dmg <= 0)
        dmg = 1;
    attr = atk->attr;
    if (attr & 0x2000)
        defAttr = 0;
    else
        defAttr = def->attr & 0x7F;
    resist = def->resist & 0x7F;
    attr &= 0x7F;
    if (attr == 0) {
        def->damage = dmg;
        def->hitKind = 1;
    } else if (defAttr == 0) {
        def->damage = (dmg * 5) >> 2;
        def->hitKind = 1;
    } else if (defAttr == 0x40) {
        def->damage = (dmg * 3) >> 2;
        def->hitKind = 2;
    } else if (attr == 0x40) {
        def->damage = (dmg * 5) >> 2;
        def->hitKind = 1;
    } else if (defAttr & attr) {
        def->damage = (dmg * 3) >> 2;
        def->hitKind = 2;
    } else if (resist & attr) {
        def->damage = (dmg * 3) >> 1;
        def->hitKind = 4;
    } else {
        def->damage = (dmg * 5) >> 2;
        def->hitKind = 1;
    }
    if ((def->flags & 0x100) && (atk->flags & 0x100)) {
        s32 d = def->height - atk->height;
        if (d < 0)
            d = -d;
        if (d <= 0x20) {
            def->damage = (def->damage * 3u) >> 1;
            def->hitKind |= 8;
        }
    }
    if (def->damage == 0)
        def->damage = 1;
}
