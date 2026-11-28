#include "npc/otenko/scene_npc_transition.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "link/wireless/random.h"

s32 Mod(s32, s32);
void Task_Kill(void *);

s32 Otenko_UpdateTransitionEffect(struct TransitionEffect *effect)
{
    if (!effect->enter)
        effect->stateFrame++;
    effect->sprite.position = effect->origin;
    switch (effect->state) {
        case 0:
            effect->enter = 0;
            if (effect->stateFrame > 55) {
                effect->state = 1;
                effect->stateFrame = 0;
                effect->enter = 1;
            }
            break;
        case 1: {
            u32 entered = effect->enter;
            const u16 *table;
            effect->enter = 0;
            if (entered)
                effect->sprite.priority = 2;
            table = gRandomTable;
            effect->sprite.position.x +=
                Mod(table[gRandomIndex = (gRandomIndex + 1) & 0x3FF], 40) - 20;
            effect->sprite.position.y +=
                Mod(table[gRandomIndex = (gRandomIndex + 1) & 0x3FF], 40) - 20;
            effect->sprite.position.z +=
                Mod(table[gRandomIndex = (gRandomIndex + 1) & 0x3FF], 40) - 20;
            if (effect->stateFrame > 58) {
                effect->state = 2;
                effect->stateFrame = 0;
                effect->enter = 1;
            }
            break;
        }
        case 2:
            effect->enter = 0;
            effect->origin.y -= 20;
            if ((s8)effect->sprite.palette > 48) {
                effect->sprite.palette -= 2;
                effect->sprite.paletteVariant -= 2;
            }
            if (effect->stateFrame > 6) {
                effect->state = 3;
                effect->stateFrame = 0;
                effect->enter = 1;
            }
            break;
        case 3:
            effect->enter = 0;
            if ((s8)effect->sprite.palette < 64) {
                effect->sprite.palette += 4;
                effect->sprite.paletteVariant += 4;
            }
            effect->origin.y += 100;
            if (effect->stateFrame > 2 && Mod(effect->stateFrame, 3) == 0)
                Otenko_SpawnTransitionParticle(effect, -1);
            if (effect->stateFrame > 29) {
                effect->state = 4;
                effect->stateFrame = 0;
                effect->enter = 1;
            }
            break;
        case 4: {
            u32 entered = effect->enter;
            effect->enter = 0;
            if (entered)
                effect->sprite.flags |= 1;
            if (effect->stateFrame > 300)
                Task_Kill(effect);
            return 0;
        }
    }
    switch (Mod(effect->frame, 3)) {
        case 0:
        case 1:
            effect->sprite.frame = 1;
            break;
        default:
            effect->sprite.frame = 0;
            break;
    }
    Otenko_UpdateTransitionParticles(effect);
    effect->frame++;
    return 0;
}
