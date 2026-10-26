#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 Modifier_InitTableTask(u8 *);
void Task_Kill(u8 *);
void Modifier_OnTableTaskUpdate(void);
void Modifier_OnTableTaskDestroy(void);

u8 *Modifier_CreateTableTask(void)
{
    u8 *p;
    if (gScriptRecordTable != 0)
        return gScriptRecordTable;
    p = Task_Create(8, 0x23c);
    if (p != 0) {
        Task_SetCallbacks(p, Modifier_OnTableTaskUpdate, Modifier_OnTableTaskDestroy);
        if (Modifier_InitTableTask(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
