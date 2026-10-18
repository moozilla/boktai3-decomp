#include "global.h"

extern u8 *gUnk_02000138;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 SceneSpriteBurst_InitManager(u8 *);
void Task_Kill(u8 *);
void sub_08121E28(void);
void sub_08121E90(void);

u8 *SceneSpriteBurst_EnsureSingleton(void)
{
    u8 *p;
    if (gUnk_02000138 != 0)
        return gUnk_02000138;
    p = Task_Create(0xa, 0x27c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08121E28, sub_08121E90);
        if (SceneSpriteBurst_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
