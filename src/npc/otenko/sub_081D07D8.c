#include "npc/otenko/otenko_actor.h"
#include "ui/hud/scene_indicator.h"

void DisplayAnimation_SetFlags(void *, s32);
void Otenko_UpdateProjectedVisible(void);
void Otenko_EnterProjectedIdle(void *unused, struct OtenkoActor *npc)
{
    s32 zero, st;
    struct OtenkoProjection *projection = &npc->projection;
    struct ContactPosition *position;
    void (*update)(void);
    DisplayAnimation_SetFlags(&npc->display, 0x10);
    position = &npc->body.position;
    projection->position.x = (position->x + gSceneCamera->x) << 12;
    projection->position.z = position->z << 12;
    zero = 0;
    projection->position.y = zero;
    projection->height = position->y << 12;
    projection->height = npc->body.position.y << 12;
    GroundEffect_Initialize(&npc->groundEffect, &projection->position, 0);
    st = 0x22;
    npc->stage = zero;
    npc->stageEntered = zero;
    npc->state = st;
    npc->timer = zero;
    npc->commandEntered = 1;
    do {
        update = Otenko_UpdateProjectedVisible;
        npc->prevState = 0x1f;
        npc->stateCallback = update;
        npc->stateFrame = zero;
        npc->animationEntered = 1;
    } while (0);
    npc->animationPhase = zero;
}
