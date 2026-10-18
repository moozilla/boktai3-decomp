#include "global.h"

struct SceneAnimationSlotFlags {
    u8 pad[0x71dc];
    u8 state[26];
    u8 counter[26];
    u8 done[26];
};
struct SceneAnimationFlagEntry {
    u32 flags;
    u8 pad[0x5c];
};
struct SceneAnimationEntries {
    u8 pad[0x5444];
    struct SceneAnimationFlagEntry slots[26];
};
struct SceneAnimationEffectPair {
    u32 a, b;
};
extern const struct SceneAnimationEffectPair gUnk_0824F354;
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void sub_08190274(void *, void *, void *, void *);

void SceneAnimation_UpdateSlotSequenceSevenEffect(u8 *p, int i)
{
    u8 *base = p + 0x71dc;
    u8 **base_pointer = &base;
    u8 *q = *base_pointer + i;
    u32 old = *q;
    switch (old) {
        case 0: {
            u32 off = i * 0x60;
            u8 *a;
            ((struct SceneAnimationEntries *)p)->slots[i].flags &= ~1;
            a = p + (off + 0x543c);
            Anim_SetKeyframes(a, p + 0x541c, 7, 2, 4);
            (*q)++;
            ((struct SceneAnimationSlotFlags *)p)->done[i] = old;
            break;
        }
        case 1:
            if (((struct SceneAnimationSlotFlags *)p)->counter[i] > 23) {
                struct SceneAnimationEffectPair v;
                *q = 2;
                v = gUnk_0824F354;
                sub_08190274(p + 0x54, (void *)(i * 0x60 + (u32)p + 0x545c), &v, p);
            }
            break;
    }
    if (Anim_Step(p + (i * 0x60 + 0x543c), p + 0x541c)) {
        ((struct SceneAnimationSlotFlags *)p)->state[i] = 0xff;
        ((struct SceneAnimationSlotFlags *)p)->done[i] = 1;
    }
}
