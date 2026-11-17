#include "scene_actor_lifecycle.h"
#include "ram.h"
int SceneActorManager_Init(struct SceneActorManager *m)
{
    struct Player *player;
    gSceneActorListManager = m;
    m->ticks = 0;
    m->head = 0;
    m->buffer = 0;
    m->count[0] = 0;
    m->count[1] = 0;
    player = gPlayers[0];
    m->player = player;
    if (player)
        m->target = &player->position;
    else
        m->target = 0;
    return 0;
}
