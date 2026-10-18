#include "global.h"

struct S {
    u8 pad[0x30];
    void (*f30)(void);
    s16 f34;
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void RegistryC281_UpdateClosingFade(void);

void SceneTransition_StartFadeOut(struct S *p)
{
    ScreenTransition_SendCommand(1, 6, 4, 4, 4, 0xFFFF, 0);
    p->f30 = RegistryC281_UpdateClosingFade;
    p->f34 = 0;
}
