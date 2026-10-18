#include "global.h"
struct Task {
    u8 pad0[0x18];
    void *listener;
    u16 state;
};
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void Wireless_RequestDestroyController(void);
void *WirelessExchange_Get(void);
void Task_Kill(void *);
int WirelessController_GetInstance(void);
void Script_Call(void *, int);
int WirelessExit_Update(struct Task *p)
{
    switch (p->state) {
        case 0:
            ScreenTransition_SendCommand(4, 0, 0, 0, 0, 65535, 0);
            Wireless_RequestDestroyController();
            {
                void *q = WirelessExchange_Get();
                if (q)
                    Task_Kill(q);
            }
            p->state++;
            break;
        case 1:
            if (!WirelessController_GetInstance())
                p->state++;
            break;
        case 2:
            if (p->listener)
                Script_Call(p->listener, 0);
            Task_Kill(p);
            break;
    }
    return 0;
}
