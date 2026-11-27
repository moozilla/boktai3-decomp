#include "global.h"
#include "engine/engine.h"
#define SceneActor_DispatchAction      Sabata_ResetAndRunPhaseCallback
#define SceneActor_SelectStateCallback Sabata_InitStateDispatch
#define SceneActor_RunUnfrozenState    Sabata_RunUnpausedStateCallback
struct SceneActorPausedUpdate {
    u8 pad0[0x4f6];
    s16 pauseClass;
};
void Sabata_UpdateFrame(struct SceneActorPausedUpdate *);
void Sabata_ResetAndRunPhaseCallback(struct SceneActorPausedUpdate *);
u32 Sabata_InitStateDispatch(struct SceneActorPausedUpdate *);
void Sabata_RunUnpausedStateCallback(struct SceneActorPausedUpdate *);
void Sabata_UpdatePresentation(struct SceneActorPausedUpdate *);
s32 SceneActor_UpdateUnlessPaused(struct SceneActorPausedUpdate *p)
{
    if (!gEnginePauseFlags || (p->pauseClass != 1 && !(gEnginePauseFlags & 7))) {
        Sabata_UpdateFrame(p);
        SceneActor_DispatchAction(p);
        SceneActor_SelectStateCallback(p);
        SceneActor_RunUnfrozenState(p);
        Sabata_UpdatePresentation(p);
    }
    return 0;
}
