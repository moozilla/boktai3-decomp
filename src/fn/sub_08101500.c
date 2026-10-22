#include "global.h"
#include "scene_vec.h"
struct Owner {
    u8 pad[0x30];
    struct SceneVec position;
};
extern struct Owner *gUnk_02000580;
void Player_SetPositionAndSave(void *, void *);
void TetherActor_CopyPosition(struct SceneVec *);
void TetherActor_SetPosition(struct SceneVec *);

void sub_08101500(void)
{
    struct SceneVec pos = gUnk_02000580->position;
    pos.x += 0x80;
    Player_SetPositionAndSave(gUnk_02000580, &pos);
    TetherActor_CopyPosition(&pos);
    pos.x += 0x80;
    TetherActor_SetPosition(&pos);
}
