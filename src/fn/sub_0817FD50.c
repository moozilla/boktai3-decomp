#include "global.h"
#include "engine.h"
#define SceneActor_DispatchAction      BossSlot2_ResetAndRunPhaseCallback
#define SceneActor_SelectStateCallback BossSlot2_InitStateDispatch
#define SceneActor_RunUnfrozenState    BossSlot2_RunUnpausedStateCallback
struct SceneActorPausedUpdate {
    u8 pad0[0x4f6];
    s16 pauseClass;
};
void BossSlot2_UpdateFrame(struct SceneActorPausedUpdate *);
void BossSlot2_ResetAndRunPhaseCallback(struct SceneActorPausedUpdate *);
u32 BossSlot2_InitStateDispatch(struct SceneActorPausedUpdate *);
void BossSlot2_RunUnpausedStateCallback(struct SceneActorPausedUpdate *);
void BossSlot2_UpdatePresentation(struct SceneActorPausedUpdate *);
s32 SceneActor_UpdateUnlessPaused(struct SceneActorPausedUpdate *p)
{
    if (!gEnginePauseFlags || (p->pauseClass != 1 && !(gEnginePauseFlags & 7))) {
        BossSlot2_UpdateFrame(p);
        SceneActor_DispatchAction(p);
        SceneActor_SelectStateCallback(p);
        SceneActor_RunUnfrozenState(p);
        BossSlot2_UpdatePresentation(p);
    }
    return 0;
}
