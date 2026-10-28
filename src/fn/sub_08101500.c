#include "global.h"
#include "scene_vec.h"
#include "player.h"
struct Owner {
    u8 pad[0x30];
    struct SceneVec position;
};
void Player_SetPositionAndSave(void *, void *);
void TetherActor_CopyPosition(struct SceneVec *);
void TetherActor_SetPosition(struct SceneVec *);

void sub_08101500(void)
{
    struct SceneVec pos = (*(struct Owner **)gPlayers)->position;
    pos.x += 0x80;
    Player_SetPositionAndSave((*(struct Owner **)gPlayers), &pos);
    TetherActor_CopyPosition(&pos);
    pos.x += 0x80;
    TetherActor_SetPosition(&pos);
}
