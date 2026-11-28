#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/math/scene_vec.h"

struct SceneVec *Player_GetEffectivePosition(s32);

void Ratatosk_UpdatePointDistancesToPlayer(struct Ratatosk *self)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        struct SceneVec *player = Player_GetEffectivePosition(0);
        s32 x = self->orbPoints[i].position.x - player->x;
        s32 z = self->orbPoints[i].position.z - player->z;

        s32 distance = x * x + z * z;

        self->orbPoints[i].distance = distance;
    }
}
