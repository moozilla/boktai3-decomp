#ifndef GUARD_SCENE_ACTOR_DAMAGE_FLASH_H
#define GUARD_SCENE_ACTOR_DAMAGE_FLASH_H

#include "scene_actor_palette.h"

void AnimData_SelectPaletteBank(void *, s32);

/* A damaged actor switches to its alternate palette bank and flashes for eight frames. */
static inline void SceneActorDamage_Flash(struct ChaseActor *actor)
{
    u16 bank = actor->base.paletteAlternateBank;
    if (actor->base.drawableKind == 0)
        AnimData_SelectPaletteBank((u8 *)actor->base.drawable + 0x2C, bank);
    else
        ((struct SceneActorPaletteTileView *)actor->base.drawable)->palette =
            (const u16 *)(gPaletteBankData + bank * 32);
    {
        u32 frames = 8;
        actor->base.flashTimer = frames;
    }
}

#endif
