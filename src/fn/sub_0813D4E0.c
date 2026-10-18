#include "global.h"
void SceneActor_StartRandomizedEffect(u8 *);
void Player_EmitQueuedEffects(u8 *p)
{
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
