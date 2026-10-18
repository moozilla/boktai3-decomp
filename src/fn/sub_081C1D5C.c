#include "global.h"
struct S {
    u8 f[0x57a];
    u16 h;
};
void sub_081C1820(void *);
void ControlPanel_ShowInteractionDisplay(void *);
void ControlPanel_SetUpdate(void *, void (*)(void));
void sub_081C1FC0(void);
void ControlPanel_OnStartPromptComplete(struct S *p)
{
    sub_081C1820(p);
    ControlPanel_ShowInteractionDisplay(p);
    p->h = 0x20;
    ControlPanel_SetUpdate(p, sub_081C1FC0);
}
