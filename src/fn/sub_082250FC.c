#include "global.h"
#include "render.h"
#include "runtime.h"
struct Task {
    u8 pad[24];
    s32 state, counter;
};
extern u32 gUnk_03005408, gUnk_030054B0;
extern u32 gUnk_03005414, gUnk_0300540C;
extern u32 gEngineFlags;
extern u16 gUnk_03005260;
extern u8 *gGameState;
void System_ShutdownAndSoftReset(void);
void SceneLifecycle_InitSceneState(void);
void Script_ResetLocalVariables(void);
u32 Script_Call(u32, u32);
void Script_RunMainBlock(void);
s32 SceneLifecycle_IsTransitionBlocked(void);
void TaskSystem_KillBelowPersistenceLevel(u32);
void SceneLifecycle_OnTransitionCleanup(void);
void SceneLifecycle_ReleaseSceneResources(void);
void GameState_BackupActiveRegions(void);
void GameState_RestoreActiveRegions(void);
void Script_SetEntryProcedure(s32);
void SunClock_Refresh(void);
s32 SceneLifecycle_UpdateTask(struct Task *task)
{
    if (!gUnk_03005408) {
        if (!(gUnk_03005260 & 2))
            goto update;
        if (!(gUnk_03005260 & 1))
            goto update;
        if ((12 & gUnk_03005260) != 12)
            goto update;
        System_ShutdownAndSoftReset();
        return 0;
    } else
        gUnk_03005408 = 0;
update: {
    u32 state = task->state;
    switch (state) {
        case 0: {
            u16 procedure;
            u32 *active;
            u32 next;
            gUnk_030054B0 = state;
            active = &gSceneEntryActive;
            next = 1;
            *active = next;
            gSceneTransitionFlags = state;
            task->counter = state;
            SceneLifecycle_InitSceneState();
            Script_ResetLocalVariables();
            procedure = gScriptEntryProcedure;
            if (!procedure)
                goto no_procedure;
            Script_Call(procedure, 0);
            if ((u32)procedure << 16)
                goto ran_procedure;
        no_procedure:
            Script_RunMainBlock();
        ran_procedure:;
            *active = state;
            task->state = next;
            gSceneTransitionActive = state;
            break;
        }
        case 1:
            if (task->counter <= 0) {
                if (gSceneTransitionFlags) {
                    if (gEnginePauseFlags & 2)
                        break;
                    if (!SceneLifecycle_IsTransitionBlocked()) {
                        gSceneControlWord0 = 0;
                        gUnk_03005414 = 0;
                        gSceneTransitionActive = state;
                        {
                            u32 mask = ~14;
                            gEnginePauseFlags = mask & gEnginePauseFlags;
                        }
                        TaskSystem_KillBelowPersistenceLevel(1);
                        task->counter = 3;
                        {
                            u16 *ptr = &gDispCntShadow;
                            u32 mask = ~0xE00;
                            *ptr = mask & *ptr;
                        }
                        gFadeMultiplier = 0x40;
                        break;
                    }
                }
                if (!(gEnginePauseFlags & 2))
                    gUnk_0300540C++;
                break;
            }
            task->counter--;
            if (task->counter > 0)
                break;
            SceneLifecycle_OnTransitionCleanup();
            SceneLifecycle_ReleaseSceneResources();
            if (gEngineFlags & 0x200) {
                gEngineFlags &= ~0x200;
                if (gEnginePauseFlags & 8)
                    gEnginePauseFlags &= ~8;
            }
            if (gSceneTransitionFlags & 0x10)
                GameState_BackupActiveRegions();
            else if (gSceneTransitionFlags & 0x100) {
                GameState_RestoreActiveRegions();
                Script_SetEntryProcedure(*(s16 *)(gGameState + 0x5A4));
            }
            task->state = 0;
            break;
    }
}
    if (gSleepResumePending) {
        SunClock_Refresh();
        gSleepResumePending = 0;
    }
    if (*((u32 *)gSessionState) <= 0xFFFFFFFE)
        (*((u32 *)gSessionState))++;
    gRuntimeFrameCounter++;
    if (gPlayTimeCounterEnabled) {
        s32 *counter = (s32 *)(gGameState + 0x614);
        if (*counter <= 0x7FFFFFFE)
            (*counter)++;
    }
    return 0;
}
