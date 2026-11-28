#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

extern u32 gUnk_02000560;
void SceneActors_RunStateCallback(void *);

void Vanargand_UpdateStateCallback(struct VanargandPairAttack *p)
{
    u32 m;
    p->finished = 0;
    m = 4;
    if ((m & gUnk_02000560) == 0)
        SceneActors_RunStateCallback(p);
}
