#include "scene_actor_control.h"
#include "player.h"
struct SceneActorControlPlayer {
    u8 unknown0[0x4F3];
    u8 parameter;
};
void TetherActor_SetParameter(s32);
void SceneActorControl_ClearUnusedParameters(struct SceneActorControlManager *p)
{
    struct SceneActorControlSlot *slots = p->slots;
    s32 first = 0, second = 0, i;
    for (i = 0; i < 8; i++) {
        if (SceneActorControl_IsUsed(p, i)) {
            if (slots[i].flags & 0x10)
                first = 1;
            if (slots[i].flags & 0x40)
                second = 1;
        }
    }
    if (!first)
        ((struct SceneActorControlPlayer **)gPlayers)[0]->parameter = 0xFF;
    if (!second)
        TetherActor_SetParameter(-1);
}
