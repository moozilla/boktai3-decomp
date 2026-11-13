#ifndef GUARD_TEXT_SEQUENCE_H
#define GUARD_TEXT_SEQUENCE_H
#include "engine.h"
struct TextSequenceStep {
    s16 holdFrames;
    u16 fadeInFrames, fadeOutFrames;
    u16 x, y;
    s16 entry;
};
struct TextSequence {
    struct Task task;
    struct TextSequenceStep steps[64];
    void *display;
    s32 lastStep;
    s16 stepIndex, timer;
    u8 phase;
};
struct TextSequenceCallbacks {
    void *phase[6];
};
extern const struct TextSequenceCallbacks gUnk_0824FCA4;
void BestiaryDisplay_Hide(void *);
void BestiaryDisplay_Show(void *);
void BestiaryDisplay_SetEntry(void *, s32);
void BestiaryDisplay_SetPosition(void *, u32, u32);
void *BestiaryDisplay_Create(u32, u32);
void BestiaryDisplay_SetPresentationMode(void *, u16);
u8 *sub_08227E9C(void);
void TextSequence_SetState(struct TextSequence *p, u8 v);
void TextSequence_UpdateFadeIn(struct TextSequence *p, struct TextSequenceStep *e);
void TextSequence_UpdateHold(struct TextSequence *p, struct TextSequenceStep *step);
void TextSequence_UpdateFadeOut(struct TextSequence *p, struct TextSequenceStep *step);
void TextSequence_UpdateHiddenWait(struct TextSequence *p, struct TextSequenceStep *step);
void TextSequence_OnDestroy(struct TextSequence *p);
s32 TextSequence_Update(struct TextSequence *p);
s32 TextSequence_ReturnZero(void);
s32 TextSequence_InitializeFromScript(struct TextSequence *p, u32 arg);
struct TextSequence *TextSequence_CreateFromScript(void *a);
#endif
