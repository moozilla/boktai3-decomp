#include "presentation_sequence.h"
#include "engine_globals.h"
#include "ram.h"
#include "monster_particle_random.h"
// Provisional name: Presentation_UpdateDropShakeBlink
void Anim_LoadKeyframe(void *, void *, s32, s32);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void Presentation_UpdateLayerAnimations(void *);
void Presentation_ClearTilemap(void);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline u32 ShakeSample(const u16 *table)
{
    u32 sample;
    do {
        gRandomIndex = (gRandomIndex + 1) & 1023;
    } while (0);
    PARTICLE_RANDOM_READ_SAMPLE(sample, table, gRandomIndex);
    return sample;
}
// Animate a presentation layer dropping, shaking, then blinking with its alternate.
void Presentation_UpdateDropShakeBlink(void *owner)
{
    struct PresentationSequence *p = owner;
    s16 *dest;
    if (p->dropTimer < 30) {
        p->layers[1].flags &= ~1;
        p->layers[1].position.x = 60;
        p->layers[1].position.y = -30;
        p->layers[3].flags |= 1;
    } else if (p->dropTimer < 45) {
        p->layers[1].flags &= ~1;
        p->layers[1].position.x = 60;
        {
            s32 y = p->dropTimer * 2 - 90;
            dest = &p->layers[1].position.y;
            *dest = y;
        }
        p->layers[3].flags |= 1;
    } else if (p->dropTimer < 50) {
        p->layers[1].position.x = Mod(ShakeSample(gRandomTable), 5) + 58;
        {
            s32 y = (45 - p->dropTimer) * 2 + Mod(ShakeSample(gRandomTable), 5) - 2;
            dest = &p->layers[1].position.y;
            *dest = y;
        }
    } else if (p->dropTimer < 55) {
        p->layers[1].position.x = Mod(ShakeSample(gRandomTable), 5) + 58;
        {
            s32 y = (p->dropTimer - 55) * 2 + Mod(ShakeSample(gRandomTable), 5) - 2;
            dest = &p->layers[1].position.y;
            *dest = y;
        }
    } else if (p->dropTimer < 60) {
        p->layers[1].position.x = Mod(ShakeSample(gRandomTable), 5) + 58;
        p->layers[1].position.y = Mod(ShakeSample(gRandomTable), 5) - 2;
        p->layers[3].flags |= 1;
    } else if (p->dropTimer < 80) {
        p->layers[1].position.x = Mod(ShakeSample(gRandomTable), 3) + 59;
        p->layers[1].position.y = Mod(ShakeSample(gRandomTable), 3) - 1;
        p->layers[3].flags |= 1;
    } else if (p->dropTimer < 90) {
        p->layers[1].position.x = 60;
        p->layers[1].position.y = 0;
    } else if (Div(p->dropTimer, 45) & 1) {
        p->layers[1].flags |= 1;
        p->layers[3].flags &= ~1;
    } else {
        p->layers[1].flags &= ~1;
        p->layers[3].flags |= 1;
    }
    p->dropTimer++;
}
