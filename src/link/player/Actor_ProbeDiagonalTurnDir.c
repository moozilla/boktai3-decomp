#include "global.h"
#include "player/player.h"

/* tentative: probes tiles beside the facing and picks a diagonal direction */

s32 Player_IsTerrainStepBlocked(struct Player *, struct SceneVec *);

s32 Actor_ProbeDiagonalTurnDir(struct Player *p)
{
    struct SceneVec v;

    switch (p->unk48D[2]) {
        case 0:
            v = p->position;
            v.x -= 30;
            v.z -= 30;
            v.x -= 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 1;
            v.x += 100;
            v.z -= 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 7;
            v = p->position;
            v.x -= 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 1;
            return 7;
        case 2:
            v = p->position;
            v.x += 30;
            v.z -= 30;
            v.x += 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 1;
            v.x -= 100;
            v.z -= 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 3;
            v = p->position;
            v.x += 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 1;
            return 3;
        case 4:
            v = p->position;
            v.x += 30;
            v.z += 30;
            v.x += 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 5;
            v.x -= 100;
            v.z += 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 3;
            v = p->position;
            v.x += 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 5;
            return 3;
        case 6:
            v = p->position;
            v.x -= 30;
            v.z += 30;
            v.x -= 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 5;
            v.x += 100;
            v.z += 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 7;
            v = p->position;
            v.x -= 100;
            if (Player_IsTerrainStepBlocked(p, &v))
                return 5;
            return 7;
    }
    return -1;
}
