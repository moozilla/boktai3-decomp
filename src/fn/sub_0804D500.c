#include "tether_actor.h"
void TetherActor_InitKindFromScript(struct TetherActor *);
void TetherActor_InitDisplayFromScript(struct TetherActor *);
void TetherActor_InitBodyFromScript(struct TetherActor *);
void TetherActor_InitInteractionFromScript(struct TetherActor *);
void TetherActor_InitChargeCollider(struct TetherActor *);
void TetherActor_InitSecondaryCollider(struct TetherActor *);
void TetherActor_InitializeSegments(struct TetherActor *);
void TetherActor_InitStateParameters(struct TetherActor *);
extern struct TetherActor *gUnk_02000488;
#define TetherActor_InitializeFromScript sub_0804D500
s32 TetherActor_InitializeFromScript(struct TetherActor *actor)
{
    TetherActor_InitKindFromScript(actor);
    TetherActor_InitDisplayFromScript(actor);
    TetherActor_InitBodyFromScript(actor);
    TetherActor_InitInteractionFromScript(actor);
    TetherActor_InitChargeCollider(actor);
    TetherActor_InitSecondaryCollider(actor);
    TetherActor_InitializeSegments(actor);
    TetherActor_InitStateParameters(actor);
    actor->glowMode = 0;
    actor->glowFrame = 0;
    actor->contactCooldown = 0;
    actor->parameter = -1;
    actor->destroyRequested = 0;
    actor->blendRequested = 0;
    actor->actionRequested = 0;
    actor->actionAngle = 0;
    actor->actionMagnitude = 0;
    actor->requestStatus = 0;
    if (actor->kind == 9) {
        actor->requestMode = Script_GetKeywordValue('t', 0);
        actor->onActionScriptId = Script_GetKeywordValue('b', 0);
    } else
        actor->requestMode = 0;
    gUnk_02000488 = actor;
    return 0;
}
