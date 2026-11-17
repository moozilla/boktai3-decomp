#include "scene_actor_lifecycle.h"
#include "ram.h"
int SceneActorManager_Init(struct SceneActorManager *m)
{
    int zero;
    struct Player *player;
    gSceneActorListManager = m;
    zero = 0;
    m->ticks = zero;
    m->head = zero;
    m->buffer = zero;
    m->count[0] = zero;
    m->count[1] = zero;
    player = gPlayers[0];
    m->player = player;
    if (player)
        m->target = &player->position;
    else
        m->target = 0;
    return 0;
}
