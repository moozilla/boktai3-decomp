#include "global.h"
struct P {
    u8 f[0x18];
    u32 w;
};
extern struct P *gUnk_02000218;
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_08199094(void);
void sub_08199314(void);
s32 sub_08199374(void *);
struct P *sub_08199410(u16 a)
{
    struct P *p;
    if (gUnk_02000218 != 0)
        return 0;
    p = Task_Create(8, 0x46C);
    if (p != 0) {
        p->w = a;
        Task_SetCallbacks(p, sub_08199094, sub_08199314);
        if (sub_08199374(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        gUnk_02000218 = p;
    }
    return p;
}
