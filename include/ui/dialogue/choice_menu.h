#ifndef GUARD_CHOICE_MENU_H
#define GUARD_CHOICE_MENU_H

#include "engine/engine.h"
#include "engine/video/anim_sprite.h"

/* One selectable entry; x/y are pixel positions, width in characters. */
struct Choice {
    u8 x, y, width, row;
};

struct ChoiceResource {
    u32 words[8];
};

struct ChoiceMenu {
    struct Task task;
    s8 selected, initial;
    u8 delay, count;
    u8 rowStart[8], rowEnd[8], rowCount[8];
    u8 rows, useScript, left, top;
    u8 maxWidth, maxRows;
    u8 width, textX, allowCancel, wakeDelay, repeatDelay, explicitWidth;
    u32 held;
    s32 arguments[4];
    u32 script;
    void *callback, *window;
    u32 stringBase;
    struct Choice choices[16];
    u8 scratch[64];
    struct AnimSpriteResource resource;
    struct AnimSprite leftCursor;
    struct AnimSprite rightCursor;
    u32 colorIndex;
    u16 palette[16];
};

/* 0x0200002C */
extern struct ChoiceMenu *gChoiceMenu;

void ChoiceMenu_DispatchSelection(struct ChoiceMenu *p, s32 selection);
void ChoiceMenu_Select(struct ChoiceMenu *p, s32 selection);
s32 ChoiceMenu_FindSelectableRow(struct ChoiceMenu *p, s32 a, s32 d);
s32 ChoiceMenu_HandleInput(struct ChoiceMenu *p);
u32 ChoiceMenu_Update(struct ChoiceMenu *p);
u32 ChoiceMenu_Destroy(struct ChoiceMenu *p);
void ChoiceMenu_PrintText(struct ChoiceMenu *p);
u8 *ChoiceMenu_ParseTag(struct ChoiceMenu *p, u8 *text);
void ChoiceMenu_LayoutText(struct ChoiceMenu *p, void *window);
s32 ChoiceMenu_Initialize(struct ChoiceMenu *p, void *window, s32 stringBase, s32 *options);
struct ChoiceMenu *ChoiceMenu_Create(void *window, s32 stringBase, void *callback, s32 *options);
struct ChoiceMenu *ChoiceMenu_CreateFromScript(void);
void ChoiceMenu_ReloadFromScript(void);

#endif
