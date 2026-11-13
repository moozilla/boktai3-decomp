#ifndef GUARD_CHALLENGE_RESULT_H
#define GUARD_CHALLENGE_RESULT_H
#include "scene_input.h"
#include "state_actor.h"
#include "render.h"
struct ChallengeTimeResult {
    struct Task task;
    struct SceneInputResource categoryGraphics, digitGraphics;
    struct SceneInputWidget category, label, digits[6];
    s32 rank;
    void *backgroundResource;
    u16 *paletteSource;
    u32 endScript;
    s32 elapsedFrames;
    void (*stateCallback)(struct ChallengeTimeResult *);
    s32 timer;
    u16 horizontalOffset, verticalOffset;
};
struct TimeResultPalette {
    u32 header[5];
    u16 colors[80];
};
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
s32 sub_08249350(s32, s32);
void BgGfx_LoadTilemapAtPosition(u32, u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, void *);
extern const u16 gUnk_0824FA00[12];
void *memcpy(void *, const void *, unsigned long);
s32 Mod(s32, s32);
s32 ChallengeMenu_GetRankKeyframe(u32);
extern u32 gUnk_08612160[];
void TimeResult_SetState(struct ChallengeTimeResult *p, void (*v)(struct ChallengeTimeResult *));
void TimeResult_LoadDigit(struct SceneInputWidget *digit, struct SceneInputResource *graphics,
                          u32 value);
void TimeResult_BeginFadeIn(struct ChallengeTimeResult *p);
void TimeResult_BeginFadeOut(struct ChallengeTimeResult *p);
void TimeResult_BeginDigitReveal(struct ChallengeTimeResult *p);
s32 TimeResult_HandleInput(struct ChallengeTimeResult *p);
void TimeResult_UpdateResultHold(struct ChallengeTimeResult *p);
void TimeResult_UpdateDigitReveal(struct ChallengeTimeResult *p);
void TimeResult_UpdateFadeIn(struct ChallengeTimeResult *p);
void TimeResult_UpdateFadeOut(struct ChallengeTimeResult *p);
void TimeResult_UpdateInput(struct ChallengeTimeResult *result);
s32 TimeResult_ShowLabel(struct ChallengeTimeResult *p);
void TimeResult_UpdateBackground(struct ChallengeTimeResult *result);
void TimeResult_Update(struct ChallengeTimeResult *p);
void TimeResult_Destroy(struct ChallengeTimeResult *p);
void TimeResult_InitializeBackground(struct ChallengeTimeResult *p);
void TimeResult_LoadPalette(struct ChallengeTimeResult *result);
void TimeResult_SetElapsedDigits(struct ChallengeTimeResult *result);
void TimeResult_InitCategoryDisplay(struct ChallengeTimeResult *p);
void TimeResult_InitAnimations(struct ChallengeTimeResult *s);
s32 TimeResult_InitializeFromScript(struct ChallengeTimeResult *p);
struct ChallengeTimeResult *TimeResult_Create(void);
void Actor_UpdateStateTwo(u32 a, struct StateActor *p);
void Actor_UpdateStateThree(u32 a, struct StateActor *p);
void Actor_UpdateStateFour(u32 a, struct StateActor *p);
void Actor_DispatchPrimaryState(u32 a, struct StateActor *b);
#endif
