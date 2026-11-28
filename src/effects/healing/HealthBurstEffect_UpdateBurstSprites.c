#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

void HealthBurstEffect_UpdateBurstSprites(struct HealthBurstEffect *effect)
{
    s32 i;

    for (i = 0; i < 2; i++) {
        if (effect->spriteState[i]) {
            struct HealthBurstSprite *sprite = &effect->sprites[i];

            if (effect->spriteState[i] == 1) {
                if ((sprite->position.y -= 50) <= effect->floorHeight[i]) {
                    Sound_PlaySong(0x1FF);
                    sprite->position.y = effect->floorHeight[i];
                    effect->spriteState[i] = 2;
                    effect->spriteTimer[i] = 0;
                } else {
                    if (++effect->spriteTimer[i] > 24) {
                        sprite->flags |= 1;
                        effect->spriteState[i] = 0;
                        effect->spriteTimer[i] = 0;
                    }
                }
            } else {
                if (++effect->spriteTimer[i] <= 11) {
                    sprite->position.y -= (effect->spriteTimer[i] >> 2) - 12;
                } else {
                    sprite->flags |= 1;
                    effect->spriteState[i] = 0;
                    effect->spriteTimer[i] = 0;
                }
            }
        }
    }
}
