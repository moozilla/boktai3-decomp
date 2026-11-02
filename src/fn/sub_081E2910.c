#include "presentation_sequence.h"
#include "engine_globals.h"
#include "ram.h"
// Provisional name: Presentation_UpdateDropShakeBlink
// MATCHING_CONCESSION: register-pinning
void Anim_LoadKeyframe(void *, void *, s32, s32);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void Presentation_UpdateLayerAnimations(void *);
void Presentation_ClearTilemap(void);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline s32 random(u16 *input)
{
    register u16 *table asm("r6") = input;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
// Animate a presentation layer dropping, shaking, then blinking with its alternate.
void Presentation_UpdateDropShakeBlink(void *owner)
{
    struct PresentationSequence *p = owner;
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
            register s16 *dest asm("r3") = &p->layers[1].position.y;
            *dest = y;
        }
        p->layers[3].flags |= 1;
    } else if (p->dropTimer < 50) {
        p->layers[1].position.x = Mod(random(gRandomTable), 5) + 58;
        {
            s32 y = (45 - p->dropTimer) * 2 + Mod(random(gRandomTable), 5) - 2;
            register s16 *dest asm("r3") = &p->layers[1].position.y;
            *dest = y;
        }
    } else if (p->dropTimer < 55) {
        p->layers[1].position.x = Mod(random(gRandomTable), 5) + 58;
        {
            s32 y = (p->dropTimer - 55) * 2 + Mod(random(gRandomTable), 5) - 2;
            register s16 *dest asm("r3") = &p->layers[1].position.y;
            *dest = y;
        }
    } else if (p->dropTimer < 60) {
        p->layers[1].position.x = Mod(random(gRandomTable), 5) + 58;
        p->layers[1].position.y = Mod(random(gRandomTable), 5) - 2;
        p->layers[3].flags |= 1;
    } else if (p->dropTimer < 80) {
        p->layers[1].position.x = Mod(random(gRandomTable), 3) + 59;
        p->layers[1].position.y = Mod(random(gRandomTable), 3) - 1;
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
