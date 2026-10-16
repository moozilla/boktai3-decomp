#include "global.h"
struct P {
    u8 f0[0xADC];
    u8 g[0xB18 - 0xADC];
    u32 v;
};
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0818A820(void);
void sub_0818ADB4(void);
s32 sub_0818C0BC(void *, s32, s32);
void SceneMessages_RegisterReceiver(void *, s32, s32);
struct P *Armor_Create(u16 a, u16 b)
{
    struct P *p = Task_Create(8, 0x1134);
    if (p != 0) {
        u32 *q = &p->v;
        *q = a;
        Task_SetCallbacks(p, sub_0818A820, sub_0818ADB4);
        if (sub_0818C0BC(p, a, b) < 0) {
            Task_Kill(p);
            return 0;
        }
        SceneMessages_RegisterReceiver(p->g, *q, 3);
    }
    return p;
}
