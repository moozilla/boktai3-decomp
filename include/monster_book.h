#ifndef GUARD_MONSTER_BOOK_H
#define GUARD_MONSTER_BOOK_H
#include "scene_input.h"
struct MonsterBookReward {
    u8 firstKind, firstId, secondKind, secondId;
};
struct MonsterBook {
    struct Task task;
    void *backgroundResource;
    u16 *paletteSource;
    u32 frame;
    u8 mode, category, entry, detailTab;
    s32 windows[8];
    u8 selectedRow;
    u8 playerName[3];
    struct SceneInputResource resource;
    struct SceneInputWidget selectionCursor, tabCursor, detailMarker, categoryBadge, categoryNumber,
        titleIcon;
    struct SceneInputWidget rowMarkers[7], entryDigits[7][2];
    struct SceneInputWidget unlockedDigits[2], countSeparator, totalDigits[2];
    struct SceneInputWidget completionBadge, groupBadge;
    u16 palette[16];
    s16 highlightFrame;
    u16 transitionFlags;
    const u8 *script;
    u8 counts[83];
    u8 pendingExit;
    u32 unknownDA8;
    struct MonsterBookReward rewards[83];
    const u8 *rewardScript, *labelScript, *emptyScript, *nameScript, *itemScript, *countScript,
        *dialogueScript, *rankScript;
    void *closeScript;
    void *display;
};
struct MonsterBookPalette {
    u32 header[5];
    u16 colors[80];
};
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Bg_SetPriority(u32, u32);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void sub_08215EB4(u32, u32, u32, u32, u32, u32, u32, u32);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_SetScript(u8 *);
u8 *sub_08227E9C(void);
void BestiaryDisplay_Hide(void *);
void BestiaryDisplay_Show(void *);
void BestiaryDisplay_SetEntry(void *, u32);
void *BestiaryDisplay_Create(u32, u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void MonsterBook_ClearWindows(struct MonsterBook *p);
void MonsterBook_DrawEntryDetails(struct MonsterBook *p);
void MonsterBook_UpdateDescription(struct MonsterBook *p);
void MonsterBook_UpdateHighlightPulse(struct MonsterBook *arg);
u32 MonsterBook_InitWidgets(struct MonsterBook *s);
u32 MonsterBook_SetupPageBackgrounds(struct MonsterBook *s, u32 value);
void MonsterBook_RefreshPage(struct MonsterBook *p);
void MonsterBook_SetPage(struct MonsterBook *p, u8 v);
u32 MonsterBook_UpdateNavigation(struct MonsterBook *p);
void MonsterBook_RemoveActiveAnimation(struct SceneInputWidget *p);
s32 MonsterBook_Destroy(struct MonsterBook *p);
s32 MonsterBook_Init(struct MonsterBook *s, void *context);
struct MonsterBook *MonsterBook_Create(void *a);
#endif
