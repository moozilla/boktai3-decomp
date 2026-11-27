#include "global.h"
void *WirelessResultPanel_GetInstance(void);
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int WirelessResultPanel_Update(void *);
int WirelessResultPanel_Destroy(void *);
int WirelessResultPanel_Init(void *);
void Task_Kill(void *);
void *WirelessResultPanel_Create(void)
{
    void *p = WirelessResultPanel_GetInstance();
    if (p)
        return p;
    p = Task_Create(11, 0x930);
    if (p) {
        Task_SetCallbacks(p, WirelessResultPanel_Update, WirelessResultPanel_Destroy);
        if (WirelessResultPanel_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
