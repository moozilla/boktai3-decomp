#include "global.h"
#include "player/player.h"

void ActorSequence_Begin(struct Player *, s32, s32);

void Player_BeginResourceStationInteraction(struct Player *p, struct SceneVec *position, s32 facing)
{
    ActorSequence_Begin(p, 8, 0);
    p->terrainRecoveryCounter = 0;
    p->resourceStationPosition = *position;
    p->resourceStationFacing = facing;
}
