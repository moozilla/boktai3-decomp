#define SlotMenu_Destroy sub_0810C724
#include "global.h"
struct SlotMenu {
    u8 unknown0[0xF74];
    u32 exitScript;
};
u32 ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void Bg_ClearTilemap(s32);
void RegistryF246_HideAnimations(struct SlotMenu *);
void Registry1745_NotifyMenuClosed(void);
void Script_Call(u32, void *);
void Task_Kill(void *);
void sub_0810C724(struct SlotMenu *p)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    RegistryF246_HideAnimations(p);
    Registry1745_NotifyMenuClosed();
    if (p->exitScript)
        Script_Call(p->exitScript, 0);
    Task_Kill(p);
}
