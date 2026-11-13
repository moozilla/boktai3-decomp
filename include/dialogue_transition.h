#ifndef GUARD_DIALOGUE_TRANSITION_H
#define GUARD_DIALOGUE_TRANSITION_H

#include "message_window.h"

struct DialogueTransition {
    struct Task task;
    u8 state, entered, index, lastIndex;
    u32 parameter, ticks;
    u16 flags, savedDisplayControl;
    void *script;
    void *entries;
    u16 completionProcedure, skipProcedure;
    s16 delays[64];
    u16 fadeIn[64], fadeOut[64], x[64], y[64], width[64], height[64];
};

extern void *gUnk_086056D8[];
int Div(int, int);
void Script_SetPc(void *);
void DialogueTransition_ApplyEntryBounds(struct DialogueTransition *transition);
void DialogueTransition_SetState(struct DialogueTransition *transition, u8 v);
u8 DialogueTransition_ConsumeStateEntry(struct DialogueTransition *transition);
void DialogueTransition_UpdateFadeIn(struct DialogueTransition *transition);
void DialogueTransition_UpdateHold(struct DialogueTransition *transition);
void DialogueTransition_UpdateFadeOut(struct DialogueTransition *transition);
void DialogueTransition_UpdateSkipFadeOut(struct DialogueTransition *transition);
int DialogueTransition_OnTaskUpdate(struct DialogueTransition *transition);
int DialogueTransition_OnTaskDestroy(struct DialogueTransition *transition);
int DialogueTransition_LoadFromScript(struct DialogueTransition *transition);
int DialogueTransition_Initialize(struct DialogueTransition *transition);
struct DialogueTransition *DialogueTransition_CreateFromScript(void);

#endif
