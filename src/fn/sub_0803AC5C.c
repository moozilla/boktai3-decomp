#include "global.h"
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int WirelessGameDisplay_Update(void *);
int WirelessGameDisplay_Destroy(void *);
int sub_0803AB28(void *);
void Task_Kill(void *);
void *WirelessGameDisplay_Create(void)
{
    void *p = Task_Create(11, 672);
    if (p) {
        Task_SetCallbacks(p, WirelessGameDisplay_Update, WirelessGameDisplay_Destroy);
        if (sub_0803AB28(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
