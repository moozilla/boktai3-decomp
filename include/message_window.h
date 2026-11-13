#ifndef GUARD_MESSAGE_WINDOW_H
#define GUARD_MESSAGE_WINDOW_H

#include "engine.h"
#include "scene_input_widget.h"
#include "contact_swarm_sprite.h"
#include "display_sequence.h"
#include "palette_resource.h"

/* Shared text interpreter storage embedded in the message-window task. */
struct MessagePlaybackState {
    u8 x, y, left, top, width, height, palette, stream;
    u8 characterTimer, characterDelay, skip, busy, defaultDelay, choiceCount;
    u8 hold, complete, unknown10, spacing, toggle, lastWidth, unknown14;
    s8 speaker;
    u8 instant, wrap;
    u32 frame;
    const u8 *pc, *streams[4];
    u8 unknown30[0x58];
    u32 numbers[16], choices[16];
    u8 depth, styles[7];
    const u8 *strings[16];
    u32 selectedCount;
    u8 selectedChoices[16];
};

struct MessageWindow {
    struct Task task;
    struct MessagePlaybackState state;
    u8 x, y, width, height;
    u16 messageIndex;
    u8 displayMode, fastPlayback;
    s8 previousSpeaker;
    u8 unknown185, autoAdvance, inputDelay;
    u16 autoAdvanceStep, autoAdvanceCounter;
    u32 pageAdvanceDelay, pageAdvanceTimer;
    u16 palette;
    s16 queuedMessage;
    u32 skipFlags, playbackCounter;
    u8 unknown1A0[4];
    void *messageTable;
    u16 messageFrame, messageCount;
    u16 messageFrames[32];
    struct PaletteResource *paletteData;
    struct SceneInputResource resource;
    struct SceneInputWidget widget;
    u8 portraitGraphics[0x1C];
    struct ContactSwarmSprite portrait;
    struct DisplaySequenceState portraitSequence;
    void *portraitResource;
    void *callback;
};

void *Script_GetTextSourcePc(void);
u32 TextState_SetString(void *, u32, u32);
void TextState_SetNumber(void *, int, int);
int TextState_StepPlayback(void *);
void TextState_ClearSelectedChoices(void *);
void TextState_RunSelectedProcedures(void *);
u32 sub_08227E9C(void);
u32 Script_GetValueSafe(void);
u32 MessageWindow_IsCreated(void);
int MessageWindow_LoadPalette(struct MessageWindow *window, int palette);
void MessageWindow_ApplyPalette(struct MessageWindow *window);
int MessageWindow_UpdateSpeakerPortrait(struct MessageWindow *window);
int MessageWindow_SetDisplayMode(int mode);
u32 MessageWindow_SetDisplayModeFromScript(void);
int MessageWindow_Clear(void);
u32 MessageWindow_ClearFromScript(u8 *window);
int MessageWindow_SetMessageTable(void *script);
s32 MessageWindow_SetTimedMessageTable(void *script, u32 count, u32 frame, u16 *frames);
u32 MessageWindow_SetMessageTableFromScript(void);
int MessageWindow_ShowMessage(int index);
int MessageWindow_SetWaitFromScript(void);
s32 MessageWindow_SetTextCursor(u32 value);
int MessageWindow_SetBounds(int x, int y, int width, int height);
int MessageWindow_SetBoundsFromScript(void);
int MessageWindow_SetNumberArgument(int a, int b);
u32 MessageWindow_SetNumberArgumentFromScript(void);
u32 MessageWindow_SetStringArgument(u32 argument, u32 text);
int MessageWindow_SetStringArgumentFromScript(void);
s32 MessageWindow_SetStringReferenceFromScript(void);
int MessageWindow_SetStringFromPc(void);
s32 MessageWindow_SetArgumentFromScript(void);
int MessageWindow_SetChoicesFromScript(void);
int MessageWindow_SetAutoAdvance(int mode, int step, int flags);
int MessageWindow_SetAutoAdvanceFromScript(void);
int MessageWindow_SetPalette(int palette);
u32 MessageWindow_SetPaletteFromScript(void);
int MessageWindow_OpenFromScript(void);
int MessageWindow_SetPageAdvanceDelay(u32 delay);
u32 MessageWindow_SetPageAdvanceDelayFromScript(void);
s32 MessageWindow_CheckExists(void);
const u8 *MessageWindow_GetStringAtOffset(struct MessageWindow *window, u32 i);
void MessageWindow_UpdatePageAdvance(struct MessageWindow *window);
void MessageWindow_UpdateFastPlayback(struct MessageWindow *window);
void MessageWindow_UpdateNormalPlayback(struct MessageWindow *window);
void MessageWindow_ClearTextCompletion(struct MessageWindow *window);
void MessageWindow_UpdateIdle(void);
void MessageWindow_UpdateImmediatePlayback(struct MessageWindow *window);
s32 MessageWindow_Update(struct MessageWindow *window);
int MessageWindow_OnDestroy(struct MessageWindow *window);

#endif
