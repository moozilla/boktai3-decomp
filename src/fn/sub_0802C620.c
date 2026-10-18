#include "global.h"
u8 *Task_Create(u32, u32);
void Script_GetKeywordValue(u32, u32);
u8 *SceneActors_GetBySlot(void);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0802BFAC(u8 *, u32);
void Task_Kill(u8 *);
void sub_0802BF30(void);
void GridAttackActor_Destroy(void);
u8 *GridAttackActor_Create(u32 a)
{
    u8 *p;
    Script_GetKeywordValue(0x74, 3);
    p = SceneActors_GetBySlot();
    if (p != 0)
        return p;
    p = Task_Create(8, 0x924);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0802BF30, GridAttackActor_Destroy);
        if (sub_0802BFAC(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
