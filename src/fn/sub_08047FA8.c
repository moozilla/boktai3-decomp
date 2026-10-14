// MATCHING_CONCESSION: register-pinning
#include "contact_burst.h"
static inline s32 nextRandomHighByte(void)
{
    s32 sample;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    sample = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
    return sample >> 8;
}
void ContactBurst_UpdateContactAnimation(struct ContactBurst *effect)
{
    s32 phase = effect->frame & 7;
    if (effect->contactTimer) {
        u8 *contactFrame;
        if (!(effect->contactTimer & 7)) {
            s32 sample;
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            sample = ((const u8 *)RANDOM_TABLE_ADDR)[gUnk_03005308 * 2];
            if (sample <= 127)
                Sound_PlaySong(0x1BF);
            else
                Sound_PlaySong(0x1C0);
        }
        contactFrame = &effect->contactFrame;
        if (!phase)
            *contactFrame = Mod(nextRandomHighByte(), 3) * 2 + 32;
        if (phase <= 3) {
            u32 frame = *contactFrame;
            struct TransientEffectSprite *sprite = &effect->sprite;
            sprite->frame = frame;
        } else {
            u32 frame = *contactFrame;
            register struct TransientEffectSprite *sprite asm("r1") = &effect->sprite;
            sprite->frame = frame + 1;
        }
        --effect->contactTimer;
    } else {
        if (phase <= 3) {
            register struct TransientEffectSprite *sprite asm("r1") = &effect->sprite;
            sprite->frame = 27;
        } else {
            register struct TransientEffectSprite *sprite asm("r1") = &effect->sprite;
            sprite->frame = 28;
        }
    }
    ++effect->frame;
}
