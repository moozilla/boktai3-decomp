#include "global.h"
#include "ram.h"
#include "runtime.h"
extern u32 gEngineFlags;
extern u32 gUnk_03005410;
extern u32 gUnk_03005414;
extern u32 gUnk_03005408;
extern u32 gUnk_0300540C;
extern u32 gUnk_030054B0;
extern u8 *gGameState;
void Script_InitActorRegistry(void);
void SceneLifecycle_ResetSceneRegistries(void);
void Script_RegisterEngineCommands(void);
void Script_RegisterBuiltinCommands(void);
void Settings_LoadOrInitializeDefaults(void);
void SceneLifecycle_ResetSubsystems(void);
void Random_InitValueTable(void);
void Hud_ResetGlobals(void);
s32 SceneLifecycle_UpdateTask(struct SceneRuntimeTask *);
void SceneLifecycle_DestroyTask(void);
void Task_SetCallbacks(struct SceneRuntimeTask *, s32 (*)(struct SceneRuntimeTask *), void (*)(void));
void Task_LinkToPriorityList(struct SceneRuntimeTask *);
void SceneLifecycle_InitManager(void)
{
    struct SceneRuntimeTask *task;
    Script_InitActorRegistry();
    SceneLifecycle_ResetSceneRegistries();
    Script_RegisterEngineCommands();
    Script_RegisterBuiltinCommands();
    gEngineFlags = 0;
    gScenePauseFlags = 0;
    gSceneControlWord1 = 0;
    gUnk_03005410 = 0;
    gSceneTransitionFlags = 0;
    gSceneControlWord0 = 0;
    gUnk_03005414 = 0;
    gUnk_03005408 = 0;
    gSceneInitWord1 = 0;
    gSceneInitWord0 = 0;
    gUnk_0300540C = 0;
    gRuntimeFrameCounter = 0;
    gSceneTransitionActive = 0;
    gPlayTimeCounterEnabled = 0;
    *(u32 *)(gGameState + 0x868) = 0;
    gSceneTransitionFlags = 0;
    gUnk_030054B0 = 0;
    Settings_LoadOrInitializeDefaults();
    SceneLifecycle_ResetSubsystems();
    Random_InitValueTable();
    Hud_ResetGlobals();
    {
        u32 value = 2;
        *(u16 *)(gGameState + 0x12) = value;
    }
    task = &gSceneLifecycleTask;
    Task_SetCallbacks(task, SceneLifecycle_UpdateTask, SceneLifecycle_DestroyTask);
    task->task.priority = 1;
    task->task.unk16 = 1;
    task->task.id = 0;
    Task_LinkToPriorityList(task);
    task->state = 0;
    task->counter = 0;
}
