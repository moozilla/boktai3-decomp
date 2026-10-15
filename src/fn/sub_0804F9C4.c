// MATCHING_CONCESSION: register-pinning
#include "repeated_effect.h"
#define RepeatedEffect_TickUpperTransition sub_0804F9C4
extern u32 gUnk_03005308;
void Sound_PlaySong(s32);
void sub_08015B84(u32, u32, u32, u32, struct RepeatedEffectPosition *);
void RepeatedEffect_TickUpperTransition(struct RepeatedEffect *effect)
{
    u16 *frame = &effect->stateFrame;
    register u32 value asm("r0");
    if (!*frame)
        Sound_PlaySong(0x15C);
    value = *frame + 1;
    *frame = value;

    value = (u16)value;
    if (value <= 7) {
        struct RepeatedEffectPosition position;
        effect->sprite.frame = 6;
        if (!(*frame & 3)) {
            position = effect->sprite.position;
            {
                u16 *random = (u16 *)RANDOM_TABLE_ADDR;
                u32 *rng = &gUnk_03005308;
                register u32 index asm("r2") = *rng + 1;
                u32 mask = 1023;
                u8 *sample;
                register u32 x asm("r1");
                register u32 z asm("r0");
                index &= mask;
                sample = (u8 *)(index * 2 + (u32)random);
                x = (u16)position.x;
                x -= 127;
                x += *sample;
                position.x = x;
                index = (index + 1) & mask;
                *rng = index;
                {
                    u8 *sampleZ = (u8 *)(index * 2 + (u32)random);
                    z = (u16)position.z;
                    z -= 127;
                    z += *sampleZ;
                    position.z = z;
                }
            }
            sub_08015B84(1, 0, 1, 64, &position);
        }
    } else if (value <= 15) {
        struct RepeatedEffectPosition position;
        effect->sprite.frame = 7;
        value = *frame + 1;
        *frame = value;

        value &= 3;
        if (!value) {
            position = effect->sprite.position;
            {
                u16 *random = (u16 *)RANDOM_TABLE_ADDR;
                u32 *rng = &gUnk_03005308;
                register u32 index asm("r2") = *rng + 1;
                u32 mask = 1023;
                u8 *sample;
                register u32 x asm("r1");
                register u32 z asm("r0");
                index &= mask;
                sample = (u8 *)(index * 2 + (u32)random);
                x = (u16)position.x;
                x -= 127;
                x += *sample;
                position.x = x;
                index = (index + 1) & mask;
                *rng = index;
                {
                    u8 *sampleZ = (u8 *)(index * 2 + (u32)random);
                    z = (u16)position.z;
                    z -= 127;
                    z += *sampleZ;
                    position.z = z;
                }
            }
            sub_08015B84(1, 0, 1, 64, &position);
        }
    } else {
        effect->sprite.frame = 8;
        sub_0804F7D0(effect, 2, sub_0804F970);
    }
}
