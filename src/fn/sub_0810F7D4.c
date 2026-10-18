#include "global.h"

extern u8 *gUnk_020001D0;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0810F678(u8 *);
void Task_Kill(u8 *);
void LinkLobbyPanel_OnTaskUpdate(void);
void LinkLobbyPanel_OnTaskDestroy(void);

u8 *LinkLobbyPanel_Create(void)
{
    u8 *p;
    if (gUnk_020001D0 != 0)
        return gUnk_020001D0;
    p = Task_Create(0xb, 0x1504);
    if (p != 0) {
        Task_SetCallbacks(p, LinkLobbyPanel_OnTaskUpdate, LinkLobbyPanel_OnTaskDestroy);
        if (sub_0810F678(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
