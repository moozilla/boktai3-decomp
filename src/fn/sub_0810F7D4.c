#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 RecordMenu_Initialize(u8 *);
void Task_Kill(u8 *);
void LinkLobbyPanel_OnTaskUpdate(void);
void LinkLobbyPanel_OnTaskDestroy(void);

u8 *LinkLobbyPanel_Create(void)
{
    u8 *p;
    if (gActiveRecordMenu != 0)
        return gActiveRecordMenu;
    p = Task_Create(0xb, 0x1504);
    if (p != 0) {
        Task_SetCallbacks(p, LinkLobbyPanel_OnTaskUpdate, LinkLobbyPanel_OnTaskDestroy);
        if (RecordMenu_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
