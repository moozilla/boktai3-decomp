#include "global.h"
#include "ram.h"
struct LinkPanel {
    u8 pad0[0x1C];
    void *resource;
    u8 pad20[0x304];
    u8 widget[0xC1C];
    void (*update)(void);
    u32 scriptS;
    u32 scriptC;
    u8 padF4C[0xC];
    u8 flagF58;
    u8 padF59[0x13];
    u8 flagF6C;
    u8 padF6D[7];
    s32 optionE;
};
s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
s32 Script_GetKeywordValue(u32, s32);
void Bg_ClearTilemap(s32);
void RegistryF246_InitBackgroundResources(struct LinkPanel *);
void SlotMenu_InitializeEntryValues(struct LinkPanel *);
void SceneMenu_InitializeDisplay(struct LinkPanel *);
void sub_0810C0BC(struct LinkPanel *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void SlotMenu_CreateTextWindows(struct LinkPanel *);
void RegistryF246_InitMessageDisplay(struct LinkPanel *);
void sub_0810C7F4(void);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 LinkPanel_InitFromScript(struct LinkPanel *p)
{
    if (Script_SeekToKeyword('s')) {
        p->scriptS = Script_GetTextSourcePc();
        if (!p->scriptS)
            return -1;
    }
    if (Script_SeekToKeyword('c')) {
        p->scriptC = Script_GetTextSourcePc();
        if (!p->scriptC)
            return -1;
    }
    p->optionE = Script_GetKeywordValue('e', 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    RegistryF246_InitBackgroundResources(p);
    SlotMenu_InitializeEntryValues(p);
    SceneMenu_InitializeDisplay(p);
    sub_0810C0BC(p);
    ModifierMenu_InitSelectionSnapshot(p->widget, p->resource);
    SlotMenu_CreateTextWindows(p);
    RegistryF246_InitMessageDisplay(p);
    {
        void (*update)(void) = sub_0810C7F4;
        u32 zero = 0;
        p->update = update;
        p->flagF6C = 1;
        p->flagF58 = zero;
        if (Script_GetKeywordValue('f', 0))
            ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, zero);
    }
    gLinkPanel = p;
    return 1;
}
