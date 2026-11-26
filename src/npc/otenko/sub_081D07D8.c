#include "npc/otenko/otenko_actor.h"
#include "ui/hud/scene_indicator.h"

void DisplayAnimation_SetFlags(void *, s32);
void Otenko_UpdateProjectedVisible(void);
void Otenko_EnterProjectedIdle(void *unused, struct OtenkoActor *npc)
{
    s32 st;
    struct OtenkoProjection *projection = &npc->projection;
    struct ContactPosition *position;
    void (*update)(void);
    DisplayAnimation_SetFlags(&npc->display, 0x10);
    position = &npc->body.position;
    projection->position.x = (position->x + gSceneCamera->x) << 12;
    projection->position.z = position->z << 12;
    projection->position.y = 0;
    projection->height = position->y << 12;
    projection->height = npc->body.position.y << 12;
    GroundEffect_Initialize(&npc->groundEffect, &projection->position, 0);
    st = 0x22;
    npc->stage = 0;
    npc->stageEntered = 0;
    npc->state = st;
    npc->timer = 0;
    npc->commandEntered = 1;
    do {
        update = Otenko_UpdateProjectedVisible;
        npc->prevState = 0x1f;
        npc->stateCallback = update;
        npc->stateFrame = 0;
        npc->animationEntered = 1;
    } while (0);
    npc->animationPhase = 0;
}
