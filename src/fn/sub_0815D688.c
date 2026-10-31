#include "player_cue.h"
void SceneMessages_RegisterReceiver(struct SceneEventReceiver *, u32, s32);
void Player_RegisterSceneMessageReceiver(struct Player *p)
{
    SceneMessages_RegisterReceiver(&PlayerCue_Messages(p)->receiver,
                                   PlayerCue_Messages(p)->receiverIdentifier, 2);
}
