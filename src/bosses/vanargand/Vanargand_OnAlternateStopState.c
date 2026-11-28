#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);

void Vanargand_OnAlternateStopState(struct VanargandPairAttack *p)
{
    u32 m = 4;
    p->flags |= m;
    if (p->pending != 0) {
        p->pending = 0;
        p->attackParameter5B8 = 0;
        p->attackParameter5BA = 0;
        SceneActors_StartUnmirroredAnimation(p, (u8 *)p + 0x18c, (u8 *)p + 0x1ec, 0xf, 0, 4);
    }
}
