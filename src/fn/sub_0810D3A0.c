#include "global.h"
#ifdef NONMATCHING
// fuzzy: 83.7%
// foundry draft (worker best 83.7%); what differs is mostly register allocation and instruction order
#define PanelSequence_InitializeFromScript sub_0810D3A0
#define SpriteGroup_Initialize             PanelSequence_InitializeSprites
#define gPanelSequence                     gUnk_020001C8
struct PanelSequence {
    u8 pad0[0x1C];
    void *palette;
    u8 pad20[0x304];
    u8 indicators[0x10C4];
    void (*update)(struct PanelSequence *);
    u32 source, context, names;
    u8 pad13F8[4];
    u8 transition;
    volatile u8 count;
    u8 pad13FE[0xE];
    u8 entering;
    u8 pad140D[5];
    u16 option;
};
struct PanelSelection {
    u8 pad0[0x53];
    volatile u8 type;
    u8 pad54[0x1BC];
    u8 count;
};
extern struct PanelSelection *gUnk_020004B4;
extern struct PanelSequence *gUnk_020001C8;
s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
s32 Script_GetPc(void);
s32 Script_GetKeywordValue(u32, s32);
void Bg_ClearTilemap(s32);
void Registry30AC_InitBackgroundResources(void *);
void SlotMenu_InitializeAlternateEntryValues(void *);
void PanelSequence_InitializeSprites(void *);
void sub_0810CFF0(void *);
void sub_0810CF90(void *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void LinkPanel_CreateTextWindows(void *);
void sub_0810CDD8(void *);
void PanelSequence_UpdateDelay(struct PanelSequence *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
static inline u8 *GetSelectionCount(void)
{
    struct PanelSelection *p = gUnk_020004B4;
    if (!p)
        return 0;
    if ((p->type & 0xFF) != 1)
        return 0;
    return &p->count;
}
s32 sub_0810D3A0(struct PanelSequence *s)
{
    u8 *count;
    int n;
    int zero;
    if (Script_SeekToKeyword('s')) {
        s->source = Script_GetTextSourcePc();
        if (!s->source)
            return -1;
    }
    if (Script_SeekToKeyword('c')) {
        s->context = Script_GetTextSourcePc();
        if (!s->context)
            return -1;
    }
    if (Script_SeekToKeyword('n')) {
        s->names = Script_GetPc();
        if (!s->names)
            return -1;
    }
    s->option = Script_GetKeywordValue('e', 0);
    count = GetSelectionCount();
    if (!count)
        return -1;
    s->count = *count;
    n = s->count;
    if (n > 10)
        n = 10;
    zero = 0;
    s->count = n;
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    Registry30AC_InitBackgroundResources(s);
    SlotMenu_InitializeAlternateEntryValues(s);
    PanelSequence_InitializeSprites(s);
    sub_0810CFF0(s);
    sub_0810CF90(s);
    ModifierMenu_InitSelectionSnapshot(s->indicators, s->palette);
    LinkPanel_CreateTextWindows(s);
    sub_0810CDD8(s);
    {
        void (*next)(struct PanelSequence *) = PanelSequence_UpdateDelay;
        s->update = next;
        s->entering = 1;
        s->transition = zero;
    }
    if (Script_GetKeywordValue('f', 0)) {
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, zero);
    }
    gPanelSequence = s;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0810D3A0);
#endif
