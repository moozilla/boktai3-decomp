#include "global.h"
#include "player.h"
#include "engine.h"

// confidence=low: consumes actionLatch465 and refreshes the selected action's presentation.
void Player_UpdateLinkModeMovement(struct Player *);
void Player_UpdateActionModeMovement(struct Player *);
void Player_InitVisualResources(struct Player *);
s32 Player_GetUsableEnchantment(struct Player *);
s32 Player_IsEnchantmentAvailable(struct Player *, s32);
static inline void ApplyActionMode(struct Player *p, u32 mode, u32 resource)
{
    if (p->actionMode != 5) {
        p->actionEighteenParameter = resource;
        p->actionEighteenDuration = 32;
    }
    p->actionMode = mode;
    if (gEngineFlags & 0x1000)
        p->updateCallback = Player_UpdateLinkModeMovement;
    else
        p->updateCallback = Player_UpdateActionModeMovement;
    p->unk419 = 0;
    p->activeAnimation = &p->animation;
    p->visualState = 0;
    p->visualPosition = &p->previousPosition;
}
void Player_ApplyLatchedActionMode(struct Player *p)
{
    u32 mode;
    s32 enchantment;
    if (p->actionLatch464) {
        mode = p->actionLatch465;
        if (mode == 1)
            ApplyActionMode(p, mode, 0x16F);
        else if (mode == 2)
            ApplyActionMode(p, mode, 0x39);
        else if (mode == 0)
            ApplyActionMode(p, mode, 0x2A);
        else
            ApplyActionMode(p, 1, 0x16F);
        Player_InitVisualResources(p);
        p->savedSpriteResource = 0xFFFF;
        enchantment = Player_GetUsableEnchantment(p) + 1;
        p->actionVariant = enchantment;
        p->baseActionVariant = enchantment + 1;
        p->spriteResource = p->baseSpriteResource;
        p->actionEnabled = Player_IsEnchantmentAvailable(p, *(s8 *)&p->thresholdMode);
        p->actionLatch464 = 0;
    }
}
