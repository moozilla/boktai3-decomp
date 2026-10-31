#include "player_cue.h"
void SceneMessages_UnregisterReceiver(struct SceneEventReceiver *);
void Player_UnregisterSceneMessageReceiver(struct Player *p)
{
    SceneMessages_UnregisterReceiver(&PlayerCue_Messages(p)->receiver);
}
