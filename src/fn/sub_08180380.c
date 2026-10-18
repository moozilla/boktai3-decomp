#include "global.h"
u8 *Task_Create(u32, u32);
void Script_GetKeywordValue(u32, u32);
u8 *SceneActors_GetBySlot(void);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0817FDE0(u8 *, u32);
void Task_Kill(u8 *);
void SceneActor_UpdateUnlessPaused(void);
void SceneActor_DestroyComponents(void);
u8 *BossSlot2_CreateFromScript(u32 a)
{
    u8 *p;
    Script_GetKeywordValue(0x74, 2);
    p = SceneActors_GetBySlot();
    if (p != 0)
        return p;
    p = Task_Create(9, 0xC68);
    if (p != 0) {
        Task_SetCallbacks(p, SceneActor_UpdateUnlessPaused, SceneActor_DestroyComponents);
        if (sub_0817FDE0(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
