#include "breakable_barrier.h"

int sub_08012D20(struct BreakableBarrier *);
void sub_08012D08(struct BreakableBarrier *, u8);
void sub_08028DDC(void);
void sub_08003588(s32, s32, s32);
void sub_0821AD08(void *, s32);

static inline void disable(struct SceneCollider *c)
{
    c->state |= 4;
}

void sub_0801310C(struct BreakableBarrierManager *m, struct BreakableBarrier *p)
{
    struct BarrierExtent size;
    if (sub_08012D20(p))
        disable(&p->collider);
    if ((p->timer & 3) == 0) {
        s32 angle, count, speed;
        const u16 *table;
        u32 *indexPtr;
        u32 index, mask;
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
        count = (table[index] & 3) + 2;
        index = (index + 1) & mask;
        *indexPtr = index;
        speed = (table[index] & 3) + 20;
        sub_0806A160(&p->sprite.position, 1, &size, angle, count, speed, 2, 2);
        sub_08012B34(m, p);
        Sound_PlaySongChecked(0x339);
    }
    if (p->timer > 63) {
        sub_08028DDC();
        if (m->remaining != 0) {
            s32 step;
            m->remaining--;
            step = (4 - m->remaining) << 4;
            sub_08003588(step - 16, step, 64);
            if (m->remaining == 0 && m->script != 0) {
                sub_0821AD08((void *)m->script, 0);
                m->script = 0;
            }
        }
        sub_08012D08(p, 0);
    }
}
