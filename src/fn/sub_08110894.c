#include "global.h"
#include "area_effect_link.h"
#include "ram.h"

s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
u32 Script_GetPc(void);
s32 Script_GetKeywordValue(u32, s32);
void Bg_ClearTilemap(s32);
void LinkScorePanel_LoadBackground(u8 *);
void sub_081100A0(u8 *);
void LinkScorePanel_InitGlyphFrames(u8 *);
void RecordList_InitializeSprites(u8 *);
void LinkMenu_LoadSelectionSprite(u8 *);
void sub_0810FADC(u8 *);
void sub_0810FDDC(u8 *, s32, s32);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void LinkScorePanel_CreateTextWindows(u8 *);
void LinkScorePanel_RefreshPlayerName(u8 *);
void sub_08110650(u8 *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

static inline s32 GetLocalPlayer(void)
{
    struct AreaEffectLinkState *state = gLinkSession;
    if (!state)
        return -1;
    return state->localPlayer;
}

s32 sub_08110894(u8 *p)
{
    if (Script_SeekToKeyword('s')) {
        if (!(*(u32 *)(p + 0xfe8) = Script_GetTextSourcePc()))
            return -1;
    }
    if (Script_SeekToKeyword('S')) {
        if (!(*(u32 *)(p + 0xff4) = Script_GetTextSourcePc()))
            return -1;
    }
    if (Script_SeekToKeyword('n')) {
        if (!(*(u32 *)(p + 0xff0) = Script_GetPc()))
            return -1;
    }
    if (Script_SeekToKeyword('F')) {
        if (!(*(u32 *)(p + 0xfec) = Script_GetTextSourcePc()))
            return -1;
    }
    *(s32 *)(p + 0xffc) = GetLocalPlayer();
    *(u16 *)(p + 0x102a) = Script_GetKeywordValue('e', 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    LinkScorePanel_LoadBackground(p);
    sub_081100A0(p);
    LinkScorePanel_InitGlyphFrames(p);
    RecordList_InitializeSprites(p);
    LinkMenu_LoadSelectionSprite(p);
    sub_0810FADC(p);
    sub_0810FDDC(p, 0, *(s16 *)((u8 *)gGameState + 0x7b6));
    sub_0810FDDC(p, 1, *(s16 *)((u8 *)gGameState + 0x7fa));
    sub_0810FDDC(p, 2, *(s16 *)((u8 *)gGameState + 0x7fc));
    ModifierMenu_InitSelectionSnapshot(p + 0x770, *(void **)(p + 0x20));
    LinkScorePanel_CreateTextWindows(p);
    LinkScorePanel_RefreshPlayerName(p);
    {
        void (*cb)(u8 *) = sub_08110650;
        u8 nine = 9;
        *(void (**)(u8 *))(p + 0xfe4) = cb;
        *(u8 *)(p + 0x1024) = 1;
        *(u8 *)(p + 0x1000) = nine;
    }
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xffff, 0);
    gSaveMenu = p;
    return 1;
}
