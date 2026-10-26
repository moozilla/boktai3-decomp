#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
    u32 offset = 0x418;
    u8 *action;
    asm("" : "+r"(offset));
    action = (u8 *)p + offset;
    if (*action != 5) {
        p->actionEighteenParameter = resource;
        p->actionEighteenDuration = 32;
    }
    *action = mode;
    if (gEngineFlags & 0x1000)
        p->updateCallback = Player_UpdateLinkModeMovement;
    else
        p->updateCallback = Player_UpdateActionModeMovement;
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
        p->unk419 = 0;
        *(void **)((u8 *)p + 0x54) = &p->animation;
        *(u32 *)((u8 *)p + 0x50) = 0;
        *(void **)((u8 *)p + 0x5C0) = &p->previousPosition;
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
