#include "global.h"

struct HealthBurstSprite {
    u32 flags;
    u8 unk_04[0x1A];
    s16 height;
    u8 unk_20[0xC];
};
struct HealthBurstEffect {
    u8 unk_00[0xE8];
    struct HealthBurstSprite sprites[2];
    u8 unk_140[0x1E];
    u8 states[2];
    u16 targetHeights[2];
    u16 timers[2];
};
void Sound_PlaySong(u32);

void HealthBurstEffect_UpdateBurstSprites(struct HealthBurstEffect *effect)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        if (effect->states[i]) {
            struct HealthBurstSprite *sprite = &effect->sprites[i];
            if (effect->states[i] == 1) {
                if ((sprite->height -= 50) <= effect->targetHeights[i]) {
                    Sound_PlaySong(0x1FF);
                    sprite->height = effect->targetHeights[i];
                    effect->states[i] = 2;
                    effect->timers[i] = 0;
                } else {
                    if (++effect->timers[i] > 24) {
                        sprite->flags |= 1;
                        effect->states[i] = 0;
                        effect->timers[i] = 0;
                    }
                }
            } else {
                if (++effect->timers[i] <= 11) {
                    sprite->height -= (effect->timers[i] >> 2) - 12;
                } else {
                    sprite->flags |= 1;
                    effect->states[i] = 0;
                    effect->timers[i] = 0;
                }
            }
        }
    }
}
