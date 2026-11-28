#include "global.h"
#include "player/player.h"

extern const u8 gUnk_08E879D4[];
s32 LinkPlayer_GetElapsedActionTicks(struct Player *);
void sub_08239D38(struct Player *);

s32 sub_08239EB4(struct Player *p, s32 mode)
{
    s32 result = mode;

    if (p->movementModifierActive != 0) {
        if (LinkPlayer_GetElapsedActionTicks(p) >= gUnk_08E879D4[p->movementModifierKind - 1]) {
            sub_08239D38(p);
            p->savedEffectTick = p->facingChangeEffects;
        }
        if (p->movementModifierActive != 0) {
            if (p->movementModifierKind == 1)
                result = 3;
            else if (p->movementModifierKind == 2)
                result = 4;
            else if (p->movementModifierKind == 3)
                result = 5;
        }
    }
    if (result == mode) {
        if (p->visualFlashTimer != 0) {
            if (((u16)p->visualFlashTimer >> 2) & 1)
                result = p->savedVisualMode;
            p->visualFlashTimer--;
        }
    } else {
        p->savedVisualMode = result;
        p->visualFlashTimer = 0x40;
    }
    return result;
}
