#include "global.h"

// confidence=low: alternates the displayed frame between zero and one on staggered timers.
#define RegistryF3E4_UpdateBlinkFrame sub_081C3950
struct BlinkEffect {
    u8 unknown0[0x34];
    u32 flags;
    u8 unknown38[0xC];
    s16 frame;
    u8 unknown46[0x56];
    s16 timer, state;
};
struct FrameView {
    u32 flags;
    u8 unknown4[12];
    s16 frame;
};
static inline void SetFrame(struct FrameView *sprite, s32 frame)
{
    sprite->frame = frame;
}

void RegistryF3E4_UpdateBlinkFrame(struct BlinkEffect *effect)
{
    u32 flags = effect->flags;
    u32 one = 1;
    flags &= one;
    if (!flags) {
        switch (effect->state) {
            case 0:
                if (++effect->timer > 1) {
                    {
                        s32 frame = effect->state;
                        SetFrame((void *)&effect->flags, frame);
                    }
                    effect->state = one;
                    effect->timer = flags;
                }
                break;
            case 1:
                if (++effect->timer > 0) {
                    {
                        s32 frame = effect->state;
                        SetFrame((void *)&effect->flags, frame);
                    }
                    effect->state = flags;
                    effect->timer = flags;
                }
                break;
        }
    }
}
