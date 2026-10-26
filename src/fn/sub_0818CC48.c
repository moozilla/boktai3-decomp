#include "global.h"

// MATCHING_CONCESSION: register-pinning
#define gPlayerObject     gGameState
#define gRandomTableIndex gRandomIndex
#define gRandomTable      gUnk_0203B400
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
void Armor_BeginPartTravelToPlayer(struct ArmorPart *p)
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
        {
            u32 index = gRandomTableIndex << 1;
            register const u16 *table asm("r2") = gRandomTable;
            random = *(const u16 *)(index + (u32)table);
        }
        direction = 1;
        if (random & 1) {
            register s32 negative asm("r0") = -1;
            asm("" : "+r"(negative));
            direction = negative;
        }
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
