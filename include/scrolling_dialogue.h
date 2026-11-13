#ifndef GUARD_SCROLLING_DIALOGUE_H
#define GUARD_SCROLLING_DIALOGUE_H

#include "message_window.h"
#include "runtime.h"

struct ScrollingDialogue {
    struct Task task;
    struct MessagePlaybackState text;
    u8 active, showRequest, hideRequest, scrolling, delay, blocked;
    u8 unknown182[2];
    u32 frames, ticks;
    s32 column, row, wait;
    const u8 *script;
};

extern const u8 gUnk_086056D0[];
void BackgroundMap_ClearTextRect(int, int, int, int);
void TextState_Init(void *, int, int, int, int);
const u8 *TextState_GetActiveStream(void *);
int TextState_DrawNarrowGlyph(void *, int);
void ScrollingDialogue_ClearTextArea(struct ScrollingDialogue *dialogue);
int ScrollingDialogue_ResetPlayback(struct ScrollingDialogue *dialogue, int clear);
void ScrollingDialogue_UpdatePlayback(struct ScrollingDialogue *dialogue);
int ScrollingDialogue_Show(struct ScrollingDialogue *dialogue);
int ScrollingDialogue_Hide(struct ScrollingDialogue *dialogue);
int ScrollingDialogue_OnTaskUpdate(struct ScrollingDialogue *dialogue);
int ScrollingDialogue_OnTaskDestroy(struct ScrollingDialogue *dialogue);
int ScrollingDialogue_Init(struct ScrollingDialogue *dialogue);
struct ScrollingDialogue *ScrollingDialogue_Create(void);
int ScrollingDialogue_SetScript(const u8 *script);
u32 ScrollingDialogue_ClearScript(void);
int ScrollingDialogue_SetNumberArgument(int index, int value);
s32 ScrollingDialogue_SetNumberArgumentFromScript(void);
s32 ScrollingDialogue_SetStringArgument(u32 a, u32 b);
s32 ScrollingDialogue_SetStringFromScript(void);
s32 ScrollingDialogue_SetStringReferenceFromScript(void);
s32 ScrollingDialogue_SetChoicesFromScript(void);
s32 ScrollingDialogue_RequestShow(void);
int ScrollingDialogue_RequestHide(void);
s32 ScrollingDialogue_HideImmediately(void);

#endif
