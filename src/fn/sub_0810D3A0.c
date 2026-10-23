#include "global.h"
#ifdef NONMATCHING
// fuzzy: 89.1%
// fix.py best version (reprinted C)
// Open: GetSelectionCount sets its 0 result before the tests, and the clamped
// store reuses the &s->count address instead of recomputing it after the join.
#define SpriteGroup_Initialize PanelSequence_InitializeSprites
#define gPanelSequence         gUnk_020001C8
struct PanelSequence {
    u8 pad0[0x1C];
    void *palette;
    u8 pad20[0x304];
    u8 indicators[0x10C4];
    void (*update)(struct PanelSequence *);
    u32 source, context, names;
    u8 pad13F8[4];
    u8 transition;
    u8 count;
    u8 pad13FE[0xE];
    u8 entering;
    u8 pad140D[5];
    u16 option;
};
struct PanelSelection {
    u8 pad0[0x53];
    u8 type;
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
void LinkPanel_StartDescriptions(void *);
void PanelSequence_UpdateDelay(struct PanelSequence *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
static inline u8 *GetSelectionCount(void)
{
    struct PanelSelection *p = gUnk_020004B4;
    if (p && (p->type == 1))
        goto ret1;
    return 0;
ret1:
    return &p->count;
}

s32 PanelSequence_InitializeFromScript(struct PanelSequence *s)
{
    u8 *count;
    int n;
    typeof(*count) t1;
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
    t1 = *count;
    *((volatile u8 *)(&s->count)) = t1;
    n = (n > 10) ? (10) : (*((volatile u8 *)(&s->count)));
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
    LinkPanel_StartDescriptions(s);
    s->update = PanelSequence_UpdateDelay;
    s->entering = 1;
    s->transition = 0;
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    gUnk_020001C8 = s;
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", PanelSequence_InitializeFromScript);
#endif
