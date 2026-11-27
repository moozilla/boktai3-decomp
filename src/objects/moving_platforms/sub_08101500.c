#include "global.h"
#include "engine/math/scene_vec.h"
#include "player/player.h"
void Player_SetPositionAndSave(void *, void *);
void TetherActor_CopyPosition(struct SceneVec *);
void TetherActor_SetPosition(struct SceneVec *);

void sub_08101500(void)
{
    struct SceneVec pos = gPlayers[0]->position;
    pos.x += 0x80;
    Player_SetPositionAndSave(gPlayers[0], &pos);
    TetherActor_CopyPosition(&pos);
    pos.x += 0x80;
    TetherActor_SetPosition(&pos);
}
