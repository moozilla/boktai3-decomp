#define Nidhoggr_UpdateEffectPool sub_0818ECFC
#define Callback_Invoke           sub_0824923C
#include "global.h"

struct NidhoggrEffect {
    void *volatile callback;
    u8 pad04[0x3c];
    u16 position[3];
    u8 pad46[0x86];
    u8 collider[0x54];
    u8 collider2[0x54];
    u16 *anchor;
    u16 delta[3];
    u8 pad17e[3];
    u8 age;
    u8 pad182[6];
};
struct NidhoggrPool {
    struct {
        void *volatile callback;
        u8 pad04[0x68];
    } actors[24];
    u32 activeActors;
    struct NidhoggrEffect effects[48];
    u32 activeEffects[2];
};
extern u32 gUnk_03003A50;
s32 sub_0824923C(void *, void *);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);

void sub_0818ECFC(struct NidhoggrPool *pool)
{
    s32 remaining;
    u32 mask;
    typeof(pool->actors[0]) *actor;
    struct NidhoggrEffect *effect;
    volatile s32 index;
    u16 *pos;
    u16 *position;
    typeof(pool->effects[0].anchor) *anchor;
    actor = pool->actors;
    mask = 1;
    remaining = 24;
    do {
        {
            void *fn = actor->callback;
            if (fn)
                sub_0824923C(actor, fn);
        }
        if (!actor->callback)
            pool->activeActors &= ~mask;
        remaining--;
        actor++;
        mask <<= 1;
    } while (remaining > 0);
    effect = pool->effects;
    remaining = 48;
    index = 0;
    position = effect->position;
    anchor = &effect->anchor;
    do {
        {
            void *fn = effect->callback;
            if (fn)
                sub_0824923C(effect, fn);
        }
        if (!effect->callback) {
            s32 i = index;
            s32 word = i >> 5;
            s32 bit = i & 31;
            pool->activeEffects[word] &= ~(1 << bit);
        } else {
            if (*anchor) {
                position[0] = (*anchor)[0] + ((u16 *)(anchor + 1))[0];
                position[1] = (*anchor)[1] + ((u16 *)(anchor + 1))[1];
                position[2] = (*anchor)[2] + ((u16 *)(anchor + 1))[2];
            } else {
                position[0] += ((u16 *)(anchor + 1))[0];
                position[1] += ((u16 *)(anchor + 1))[1];
                position[2] += ((u16 *)(anchor + 1))[2];
            }
            {
                pos = effect->position;
                if (!((gUnk_03003A50 ^ remaining) & 1)) {
                    Collider_SetPosition(effect->collider, pos, 0);
                    Collider_SetPosition(effect->collider2, pos, 0);
                    Collider_Refresh(effect->collider);
                }
            }
            ((u8 *)anchor)[13]++;
        }
        remaining--;
        anchor = (void *)((u8 *)anchor + 0x188);
        position = (void *)((u8 *)position + 0x188);
        effect++;
        index++;
    } while (remaining > 0);
}
