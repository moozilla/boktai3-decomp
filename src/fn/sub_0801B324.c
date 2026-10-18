#include "global.h"
// MATCHING_CONCESSION: register-pinning
typedef struct {
    u32 words[8];
} Resource;
Resource *ScriptProc_Start(int, int);
void ResourceHeader_Relocate(void *, void *);
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
u32 Script_GetTextSourcePc(void);
void sub_0801A218(void *);
void sub_0801A418(void *);
void sub_0801A6D0(void *);
void sub_0801A3C0(void *);
void SelectionMenu_InitHighlight(void *);
void MessageWindow_SetBounds(int, int, int, int);
void SelectionMenu_OpenActorPage(void *);
void SelectionMenu_SetCallback(void *, void (*)(void));
void SelectionMenu_Choose(void);
void ScrollingDialogue_RequestShow(void);
extern u32 gUnk_0300523C;
int SelectionMenu_Initialize(u8 *p)
{
    Resource *resource = ScriptProc_Start(0xcb05, 0x530d);
    if (!resource)
        goto fail;
    asm("" ::: "r0");
    *(Resource *)(p + 0x4c) = *resource;
    ResourceHeader_Relocate(p + 0x4c, resource);
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    if (!resource)
        goto fail;
    asm("" ::: "r0");
    *(Resource *)(p + 0x6c) = *resource;
    ResourceHeader_Relocate(p + 0x6c, resource);
    *(u16 *)(p + 0x40) = Script_GetKeywordValue('c', 0);
    *(u16 *)(p + 0x42) = Script_GetKeywordValue('e', 0);
    if (!Script_SeekToKeyword('w'))
        goto fail;
    if (!(*(u32 *)(p + 0xda8) = Script_GetTextSourcePc()))
        goto fail;
    if (!Script_SeekToKeyword('a'))
        goto fail;
    if ((*(u32 *)(p + 0xdac) = Script_GetTextSourcePc()))
        goto configure;
fail:
    return -1;
configure:
    if (!Script_SeekToKeyword('k'))
        goto fail;
    if (!(*(u32 *)(p + 0xdb0) = Script_GetTextSourcePc()))
        goto fail;
    sub_0801A218(p);
    sub_0801A418(p);
    sub_0801A6D0(p);
    sub_0801A3C0(p);
    SelectionMenu_InitHighlight(p);
    MessageWindow_SetBounds(0, 16, 30, 2);
    SelectionMenu_OpenActorPage(p);
    SelectionMenu_SetCallback(p, SelectionMenu_Choose);
    ScrollingDialogue_RequestShow();
    {
        int mask = 4;
        gUnk_0300523C |= mask;
    }
    return 0;
}
