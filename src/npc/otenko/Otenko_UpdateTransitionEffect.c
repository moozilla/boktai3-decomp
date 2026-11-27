#include "npc/otenko/scene_npc_transition.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

s32 Mod(s32, s32);
void Task_Kill(void *);

s32 Otenko_UpdateTransitionEffect(struct TransitionEffect *effect)
{
    if (effect->enter == 0)
        effect->stateFrame += 1;
    effect->sprite.position = effect->origin;
    switch (effect->state) {
        case 0:
            effect->enter = 0;
            if (effect->stateFrame > 0x37) {
                effect->state = 1;
                effect->stateFrame = 0;
                effect->enter = 1;
            }
            break;
        case 1: {
            u32 t = effect->enter;
            const u16 *tab;
            u32 *cur;
            effect->enter = 0;
            if (t != 0)
                effect->sprite.priority = 2;
            tab = gRandomTable;
            cur = &gRandomIndex;
            *cur = (*cur + 1) & 0x3FF;
            {
                s32 m = Mod(tab[*cur], 0x28);
                u32 v = (u16)effect->sprite.position.x;
                v -= 0x14;
                effect->sprite.position.x = v + m;
            }
            *cur = (*cur + 1) & 0x3FF;
            {
                s32 m = Mod(tab[*cur], 0x28);
                u32 v = (u16)effect->sprite.position.y;
                v -= 0x14;
                effect->sprite.position.y = v + m;
            }
            *cur = (*cur + 1) & 0x3FF;
            {
                s32 m = Mod(tab[*cur], 0x28);
                u32 v = (u16)effect->sprite.position.z;
                v -= 0x14;
                effect->sprite.position.z = v + m;
            }
            if (effect->stateFrame > 0x3a) {
                effect->state = 2;
                effect->stateFrame = 0;
                effect->enter = 1;
            }
            break;
        }
        case 2:
            effect->enter = 0;
            effect->origin.y -= 0x14;
            if ((s8)effect->sprite.palette > 0x30) {
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
            if ((s8)effect->sprite.palette <= 0x3f) {
                effect->sprite.palette += 4;
                effect->sprite.paletteVariant += 4;
            }
            effect->origin.y += 0x64;
            if (effect->stateFrame > 2 && Mod(effect->stateFrame, 3) == 0)
                Otenko_SpawnTransitionParticle(effect, -1);
            if (effect->stateFrame > 0x1d) {
                effect->state = 4;
                effect->stateFrame = 0;
                effect->enter = 1;
            }
            break;
        case 4: {
            u32 t = effect->enter;
            effect->enter = 0;
            if (t != 0)
                effect->sprite.flags |= 1;
            if (effect->stateFrame > 300)
                Task_Kill(effect);
            return 0;
        }
    }
    {
        s32 remainder = Mod(effect->frame, 3);
        u16 *frame = &effect->sprite.frame;
        if (remainder <= 1) {
            if (remainder >= 0)
                *frame = 1;
            else
                *frame = 0;
        } else
            *frame = 0;
    }
    Otenko_UpdateTransitionParticles(effect);
    effect->frame += 1;
    return 0;
}
