#include "global.h"
extern u8 *gUnk_02000090;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_08018028(u8 *);
void Task_Kill(u8 *);
void Weather_Update(void);
void sub_08018014(void);
u8 *sub_080182FC(void)
{
    u8 *r;
    if (gUnk_02000090 != 0)
        return gUnk_02000090;
    r = Task_Create(9, 0x8ec);
    if (r) {
        Task_SetCallbacks(r, Weather_Update, sub_08018014);
        if (sub_08018028(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
