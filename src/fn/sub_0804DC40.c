#include "staged_effect.h"
#define StagedEffect_Tick sub_0804DC40
extern void (*const gUnk_08605A58[])(struct StagedEffect *);
void sub_0824923C(void *, void *);
s32 StagedEffect_Tick(struct StagedEffect *effect)
{
    s32 i;
    sub_0824923C(effect, gUnk_08605A58[effect->state]);
    if (effect->state != 0) {
        i = 0;
        do {
            struct StagedEffectSprite *sprite = &effect->sprites[i];
            s32 spread;
            u32 xy = effect->position.words[0];
            u32 z = effect->position.words[1];
            *(u32 *)&sprite->x = xy;
            *(u32 *)&sprite->z = z;
            spread = effect->spread;
            switch (i) {
                case 0:
                    sprite->y += effect->travel;
                    {
                        u32 scale = effect->scale >> 4;
                        sprite->scaleX = scale;
                        sprite->scaleY = scale;
                    }
                    break;
                case 1:
                    sprite->x -= spread;
                    sprite->z -= spread;
                    break;
                case 2:
                    sprite->x += spread;
                    sprite->z += spread;
                    break;
                case 3:
                    sprite->x -= spread;
                    sprite->z += spread;
                    break;
                case 4:
                    sprite->x += spread;
                    sprite->z -= spread;
                    break;
            }
            sprite->x += effect->shakeX;
            sprite->z += effect->shakeZ;
            i++;
        } while (i <= 4);
    }
    return 0;
}
