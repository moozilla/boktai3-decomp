#include "global.h"
#include "player.h"
u32 ActorFlags_GetResponseBits(struct Player *);
s32 ActorFlags_SelectResponse(struct Player *);
void PlayerAttackEffect_SetElementMode(u32);
void PlayerAttackEffect_SetDefaultPaletteSource(u32);
void Player_SelectActionVariant(struct Player *p)
{
    u32 flags = ActorFlags_GetResponseBits(p);
    if (p->currentStatB < ActorFlags_SelectResponse(p)) {
        PlayerAttackEffect_SetElementMode(0);
        p->actionVariant = (u8)p->statModifierContext.savedActionIndex;
    } else if (flags & 1) {
        PlayerAttackEffect_SetElementMode(1);
        p->actionVariant = 1;
    } else if (flags & 4) {
        PlayerAttackEffect_SetElementMode(2);
        p->actionVariant = 3;
    } else if (flags & 8) {
        PlayerAttackEffect_SetElementMode(3);
        p->actionVariant = 4;
    } else if (flags & 0x10) {
        PlayerAttackEffect_SetElementMode(4);
        p->actionVariant = 5;
    } else if (flags & 0x20) {
        PlayerAttackEffect_SetElementMode(5);
        p->actionVariant = 6;
    } else {
        PlayerAttackEffect_SetElementMode(0);
        p->actionVariant = (u8)p->statModifierContext.savedActionIndex;
    }
    PlayerAttackEffect_SetDefaultPaletteSource(p->actionVariantResource);
}
