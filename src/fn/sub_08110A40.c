#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08110894(u8 *);
void Task_Kill(u8 *);
void SaveMenu_Update(void);
void SaveMenu_Destroy(void);

u8 *SaveMenu_CreateFromScript(void)
{
    u8 *p;
    if (gSaveMenu != 0)
        return gSaveMenu;
    p = Task_Create(0xb, 0x1034);
    if (p != 0) {
        Task_SetCallbacks(p, SaveMenu_Update, SaveMenu_Destroy);
        if (sub_08110894(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
