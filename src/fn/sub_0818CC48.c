
#ifdef NONMATCHING
// fuzzy: 93.5%
// foundry draft (worker best 92.9%); what differs is mostly register allocation and instruction order
#define Armor_BeginPartTravelToPlayer sub_0818CC48
#define gPlayerObject                 gUnk_02000710
#define gRandomTableIndex             gUnk_03005308
#define gRandomTable                  gUnk_0203B400
#include "global.h"
extern u32 gRandomTableIndex;
extern const u16 gRandomTable[];
struct ArmorTravelTarget {
    u8 pad0[0x30];
    u16 x, y, z;
};
extern struct ArmorTravelTarget *gPlayerObject;
struct ArmorBody {
    u8 pad0[0x5C2];
    u8 flags5C2;
};
struct ArmorPart {
    u8 pad0[6];
    u8 rotation;
    u8 pad7[0x1C - 7];
    u16 x, y, z;
    u8 pad22[0x134 - 0x22];
    u16 endX, endY, endZ;
    u8 pad13A[2];
    u16 startX, startY, startZ;
    u8 pad142[2];
    s16 dx, dy, dz;
    u8 pad14A[0x15C - 0x14A];
    struct ArmorBody *parent;
    u8 pad160[0x1D0 - 0x160];
    void (*callback)(struct ArmorPart *);
    u8 pad1D4[2];
    u16 timer;
    u8 pad1D8[2];
    s8 direction;
    u8 pad1DB[2];
    s8 rotationDelta;
};
void Armor_UpdatePartTravelXWave(struct ArmorPart *);
void Armor_UpdatePartTravelZWave(struct ArmorPart *);
void sub_0818CC48(struct ArmorPart *p)
{
    s32 delta;
    if (!(p->parent->flags5C2 & 3))
        p->rotationDelta--;
    p->rotation += p->rotationDelta;
    delta = p->rotationDelta;
    if (delta < 0)
        delta = -delta;
    if (delta > 9) {
        s32 dx, dz;
        u32 direction;
        u32 random;
        struct ArmorTravelTarget *target;
        p->startX = p->x;
        p->startY = p->y;
        p->startZ = p->z;
        target = gPlayerObject;
        p->endX = target->x;
        p->endY = target->y + 220;
        p->endZ = target->z;
        p->dx = p->endX - p->startX;
        p->dy = p->endY - p->startY;
        p->dz = p->endZ - p->startZ;
        p->timer = 0;
        gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
        random = *(const u16 *)((gRandomTableIndex << 1) + (u32)gRandomTable);
        direction = (random & 1) ? -1 : 1;
        p->direction = direction;
        dx = p->dx;
        if (dx < 0)
            dx = -dx;
        dz = p->dz;
        if (dz < 0)
            dz = -dz;
        if (dx < dz)
            p->callback = Armor_UpdatePartTravelXWave;
        else
            p->callback = Armor_UpdatePartTravelZWave;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818CC48);
#endif
