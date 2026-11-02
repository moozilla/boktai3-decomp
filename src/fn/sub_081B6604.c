#define gActorEffectFadeFrames gUnk_0824F798
#include "vanargand_effect.h"
struct FadeFrames {
    s32 frames[3];
};
extern const struct FadeFrames gUnk_0824F798;
void ActorEffect_UpdateFadeFrame(struct VanargandEffect *effect)
{
    struct VanargandEffectSprite *sprite = &effect->sprite;
    s32 mode = effect->variant;
    s32 elapsed;
    switch (mode) {
        case 0:
            elapsed = 30 - effect->lifetime;
            if (elapsed <= 14) {
                struct FadeFrames frames = gUnk_0824F798;
                sprite->frame = frames.frames[elapsed / 5];
            }
            break;
        case 1:
        case 2:
        case 3:
            elapsed = 30 - effect->lifetime;
            if (elapsed <= 29)
                sprite->frame = elapsed / 5;
            break;
    }
}
