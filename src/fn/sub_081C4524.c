#include "global.h"

struct S {
    u8 pad[0x64];
    void (*f64)(void);
    s32 f68;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void Registry6A37_UpdateFadeOut(void);

void Registry6A37_StartFadeOut(struct S *p)
{
    ScreenTransition_SendCommand(1, 6, 4, 4, 4, 0xFFFF, 0);
    p->f64 = Registry6A37_UpdateFadeOut;
    p->f68 = 0;
}
