#include "animated_hazard.h"

void sub_0801204C(void *, struct AnimatedHazard *, u32);

void AnimatedHazard_WaitForPlayer(void *m, struct AnimatedHazard *p)
{
    if (gUnk_02000710->unk876 > 0)
        p->sprite.flags &= ~1;
    else
        p->sprite.flags |= 1;
    if (++p->timer > p->duration)
        sub_0801204C(m, p, 3);
    else {
        struct SceneVec *player = sub_0815F53C(0);
        struct SceneVec *pos = &p->sprite.position;
        int x = player->x - pos->x, z = player->z - pos->z;
        if (x * x + z * z < 14400)
            sub_0801204C(m, p, 3);
    }
}
