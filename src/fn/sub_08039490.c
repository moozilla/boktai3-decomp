#include "global.h"
struct Task {
    u8 pad0[0x16];
    u8 active;
};
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int WirelessTradeMenu_Update(void *);
int WirelessTradeMenu_Destroy(void *);
int WirelessTradeMenu_InitFromScript(void *);
void Task_Kill(void *);
struct Task *WirelessTradeMenu_Create(void)
{
    struct Task *p = Task_Create(2, 276);
    if (p) {
        Task_SetCallbacks(p, WirelessTradeMenu_Update, WirelessTradeMenu_Destroy);
        if (WirelessTradeMenu_InitFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        p->active = 1;
    }
    return p;
}
