#define gActorEffectFadeFrames gUnk_0824F798
#include "global.h"
struct ActorEffectSprite {
    u8 pad0[0x10];
    u16 frame;
};
struct ActorEffect {
    u8 pad0[0x18];
    struct ActorEffectSprite sprite;
    u8 pad2A[0x9A - 0x2C];
    u16 fadeTimer;
    u16 mode;
};
struct FadeFrames {
    s32 frames[3];
};
extern const struct FadeFrames gUnk_0824F798;
void ActorEffect_UpdateFadeFrame(struct ActorEffect *effect)
{
    struct ActorEffectSprite *sprite = &effect->sprite;
    s32 mode = effect->mode;
    s32 elapsed;
    switch (mode) {
        case 0:
            elapsed = 30 - effect->fadeTimer;
            if (elapsed <= 14) {
                struct FadeFrames frames = gUnk_0824F798;
                sprite->frame = frames.frames[elapsed / 5];
            }
            break;
        case 1:
        case 2:
        case 3:
            elapsed = 30 - effect->fadeTimer;
            if (elapsed <= 29)
                sprite->frame = elapsed / 5;
            break;
    }
}
