#include "global.h"

void EffectGroup_Reset(u8 *);
void SceneActor_StartRandomizedEffect(u8 *);

void Player_FlushQueuedEffects(u8 *p)
{
    if (p[0x5B9] == 1) {
        EffectGroup_Reset(p + 0x5B8);
    } else {
        u8 t = p[0x5BB];
        s32 n;

        if (t != 0) {
            n = t;
            do {
                SceneActor_StartRandomizedEffect(p);
                n--;
            } while (n != 0);
        }
    }
}
