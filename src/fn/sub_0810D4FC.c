#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 PanelSequence_InitializeFromScript(u8 *);
void Task_Kill(u8 *);
void Registry30AC_Update(void);
void Registry30AC_Destroy(void);

u8 *Registry30AC_Create(void)
{
    u8 *p;
    if (gPanelSequence != 0)
        return gPanelSequence;
    p = Task_Create(0xb, 0x1414);
    if (p != 0) {
        Task_SetCallbacks(p, Registry30AC_Update, Registry30AC_Destroy);
        if (PanelSequence_InitializeFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
