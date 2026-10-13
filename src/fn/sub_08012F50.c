#include "breakable_barrier.h"
// MATCHING_CONCESSION: register-pinning

int sub_08012D20(struct BreakableBarrier *);
void sub_08012D08(struct BreakableBarrier *, u8);
static inline void enable(struct SceneCollider *c)
{
    u32 mask = ~4;
    c->state = mask & c->state;
}
static inline void disable(struct SceneCollider *c)
{
    c->state |= 4;
}
void BreakableBarrier_Update(struct BreakableBarrierManager *m, struct BreakableBarrier *p)
{
    struct BarrierExtent size;
    if (sub_08012D20(p)) {
        p->sprite.flags &= ~1;
        AnimData_SelectTiles(p->resource, 136);
    }
    p->frame++;
    if (p->frame == 7)
        p->sprite.frame = 1;
    else if (p->frame == 14) {
        p->sprite.frame = 0;
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
            sub_0806A160(&p->sprite.position, 3, &size, angle, count, speed, 2, 2);
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
            sub_0806A160(&p->sprite.position, 3, &size, angle, count, speed, 2, 2);
            sub_08012D08(p, 2);
            return;
        } else {
            sub_08012B34(m, p);
            sub_08012B34(m, p);
            sub_08012B34(m, p);
            Sound_PlaySongChecked(232);
            AnimData_SelectTiles(p->resource, 402);
        }
    }
    if (p->flash) {
        p->flash--;
        if (!p->flash) {
            AnimData_SelectTiles(p->resource, 136);
            enable(&p->collider);
        } else
            disable(&p->collider);
    }
}
