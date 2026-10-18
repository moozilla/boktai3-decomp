#include "global.h"
#include "player.h"
#define Player_SelectActionVariant sub_0814D02C
#define Player_CollectActionFlags  sub_0814A854
u32 sub_0814A854(struct Player *);
s32 sub_0813B480(struct Player *);
void PlayerAttackEffect_SetElementMode(u32);
void PlayerAttackEffect_SetDefaultPaletteSource(u32);
void sub_0814D02C(struct Player *p)
{
    u32 flags = sub_0814A854(p);
    if (p->currentStatB < sub_0813B480(p)) {
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
