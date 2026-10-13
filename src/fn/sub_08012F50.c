#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Collider {
    u8 pad[6];
    u16 flags;
    u8 rest[76];
};
struct Barrier {
    u32 flags;
    u8 pad4[3], priority;
    u8 pad8[20];
    struct Vec position;
    u8 spriteTail[8], resource[28];
    struct Collider collider;
    u16 health;
    u8 state, entered, flash, hit, frame, pada3;
    u32 timer;
};
struct Manager {
    u8 task[24];
    u32 count, health, remaining, blend, script;
    u32 direction[4];
    struct Vec positions[4];
};
int sub_082151E4(void *, int);
void sub_082144A4(void *, void *, int);
void BreakableBarrier_SetPosition(void *, void *, int);
void sub_0821FEB4(void *, int, int, int, int, void *, void *);
void sub_0821FF24(void *, void *, int);
void sub_0821FF84(void *, void *, void *);
void sub_0821FF7C(void *, int, int, int);
void sub_0821FE40(void *);
void sub_08012D78(void *, void *, void *);
void sub_08012D08(void *, int);
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
int sub_08012D20(void *);
void sub_08215284(void *, int);
void sub_0806A160(void *, int, void *, int, int, int, int, int);
void sub_08012B34(void *, void *);
void sub_0822B3BC(int);
static inline int randomValue(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline void enable(struct Collider *c)
{
    u32 mask = ~4;
    c->flags = mask & c->flags;
}
static inline void disable(struct Collider *c)
{
    c->flags |= 4;
}
void BreakableBarrier_Update(struct Manager *m, struct Barrier *p)
{
    struct Packed size;
    if (sub_08012D20(p)) {
        p->flags &= ~1;
        sub_08215284(p->resource, 136);
    }
    p->frame++;
    if (p->frame == 7)
        *(u16 *)(p->pad8 + 8) = 1;
    else if (p->frame == 14) {
        *(u16 *)(p->pad8 + 8) = 0;
        p->frame = 0;
    }
    if (p->hit) {
        p->hit = 0;
        p->flash = 10;
        p->health--;
        if ((s16)p->health <= 0) {
            int angle, speed;
            register int count asm("r1");
            u16 *table;
            u32 *indexPtr;
            u32 mask;
            register u32 smallMask asm("r8");
            register u32 index asm("r0");
            size.x = 164;
            size.y = 16;
            size.z = 164;
            table = gUnk_0203B400;
            indexPtr = &gUnk_03005308;
            index = *indexPtr;
            index++;
            mask = 1023;
            index &= mask;
            angle = (u8)table[index];
            index = (index + 1) & mask;
            count = table[index];
            smallMask = 3;
            count = (count & smallMask) + 2;
            index = (index + 1) & mask;
            *indexPtr = index;
            speed = (table[index] & smallMask) + 20;
            sub_0806A160(&p->position, 3, &size, angle, count, speed, 2, 2);
            {
                u32 *ptr = indexPtr;
                index = (*ptr + 1) & mask;
            }
            angle = (u8)table[index];
            index = (index + 1) & mask;
            count = (table[index] & smallMask) + 2;
            index = (index + 1) & mask;
            *indexPtr = index;
            speed = (table[index] & smallMask) + 20;
            sub_0806A160(&p->position, 3, &size, angle, count, speed, 2, 2);
            sub_08012D08(p, 2);
            return;
        } else {
            sub_08012B34(m, p);
            sub_08012B34(m, p);
            sub_08012B34(m, p);
            sub_0822B3BC(232);
            sub_08215284(p->resource, 402);
        }
    }
    if (p->flash) {
        p->flash--;
        if (!p->flash) {
            sub_08215284(p->resource, 136);
            enable(&p->collider);
        } else
            disable(&p->collider);
    }
}
