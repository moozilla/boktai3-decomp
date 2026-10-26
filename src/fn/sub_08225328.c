#include "global.h"
struct Task {
    u8 pad[16];
    u16 f10;
    u8 pad12[2];
    u8 f14, f15, f16, f17;
    s32 state, counter;
};
extern u32 gEngineFlags;
extern u32 gUnk_030053F0;
extern u32 gUnk_030053EC;
extern u32 gUnk_03005410;
extern u32 gUnk_030053E8;
extern u32 gUnk_030053DC;
extern u32 gUnk_03005414;
extern u32 gUnk_03005408;
extern u32 gUnk_03005400;
extern u32 gUnk_030053E0;
extern u32 gUnk_0300540C;
extern u32 gUnk_030053E4;
extern u32 gUnk_030053FC;
extern u32 gUnk_030053D8;
extern u32 gUnk_030054B0;
extern u8 *gGameState;
extern struct Task gUnk_030025A0;
void Script_InitActorRegistry(void);
void SceneLifecycle_ResetSceneRegistries(void);
void Script_RegisterEngineCommands(void);
void Script_RegisterBuiltinCommands(void);
void Settings_LoadOrInitializeDefaults(void);
void SceneLifecycle_ResetSubsystems(void);
void Random_InitValueTable(void);
void Hud_ResetGlobals(void);
s32 SceneLifecycle_UpdateTask(struct Task *);
void SceneLifecycle_DestroyTask(void);
void Task_SetCallbacks(struct Task *, s32 (*)(struct Task *), void (*)(void));
void Task_LinkToPriorityList(struct Task *);
void SceneLifecycle_InitManager(void)
{
    struct Task *task;
    Script_InitActorRegistry();
    SceneLifecycle_ResetSceneRegistries();
    Script_RegisterEngineCommands();
    Script_RegisterBuiltinCommands();
    gEngineFlags = 0;
    gUnk_030053F0 = 0;
    gUnk_030053EC = 0;
    gUnk_03005410 = 0;
    gUnk_030053E8 = 0;
    gUnk_030053DC = 0;
    gUnk_03005414 = 0;
    gUnk_03005408 = 0;
    gUnk_03005400 = 0;
    gUnk_030053E0 = 0;
    gUnk_0300540C = 0;
    gUnk_030053E4 = 0;
    gUnk_030053FC = 0;
    gUnk_030053D8 = 0;
    *(u32 *)(gGameState + 0x868) = 0;
    gUnk_030053E8 = 0;
    gUnk_030054B0 = 0;
    Settings_LoadOrInitializeDefaults();
    SceneLifecycle_ResetSubsystems();
    Random_InitValueTable();
    Hud_ResetGlobals();
    {
        u32 value = 2;
        *(u16 *)(gGameState + 0x12) = value;
    }
    task = &gUnk_030025A0;
    Task_SetCallbacks(task, SceneLifecycle_UpdateTask, SceneLifecycle_DestroyTask);
    task->f14 = 1;
    task->f16 = 1;
    task->f10 = 0;
    Task_LinkToPriorityList(task);
    task->state = 0;
    task->counter = 0;
}
