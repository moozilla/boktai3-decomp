#include "ui/results/challenge_result.h"
#include "gba/io_reg.h"
#include "engine/math/monster_particle_random.h"

void TimeResult_SetState(struct ChallengeTimeResult *result,
                         void (*v)(struct ChallengeTimeResult *))
{
    result->stateCallback = v;
    result->timer = 0;
}

void TimeResult_LoadDigit(struct AnimSprite *digit, struct AnimSpriteResource *graphics, u32 value)
{
    if (value <= 9)
        Anim_LoadKeyframe(digit, graphics, (u16)value, 0);
}

void TimeResult_BeginFadeIn(struct ChallengeTimeResult *result)
{
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
    TimeResult_SetState(result, TimeResult_UpdateFadeIn);
}

void TimeResult_BeginFadeOut(struct ChallengeTimeResult *result)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
    TimeResult_SetState(result, TimeResult_UpdateFadeOut);
}

void TimeResult_BeginDigitReveal(struct ChallengeTimeResult *result)
{
    s32 i;
    for (i = 0; i < 6; i++)
        result->digits[i].flags &= ~1;
    TimeResult_SetState(result, TimeResult_UpdateDigitReveal);
}

s32 TimeResult_HandleInput(struct ChallengeTimeResult *result)
{
    u16 *keys = gPadState;
    if (keys[0] & 0x20)
        result->horizontalOffset--;
    else if (keys[0] & 0x10)
        result->horizontalOffset++;
    if (keys[0] & 0x40)
        result->verticalOffset--;
    else if (keys[0] & 0x80)
        result->verticalOffset++;
    if (keys[1] & 1)
        TimeResult_BeginFadeOut(result);
}

void TimeResult_UpdateResultHold(struct ChallengeTimeResult *result)
{
    result->timer++;
    if (result->timer > 0x3c || (gPadState[1] & 1) != 0) {
        Sound_PlaySong(0x167);
        if ((u32)(result->rank - 1) <= 4)
            result->category.flags &= ~1;
        TimeResult_SetState(result, TimeResult_UpdateInput);
    }
}

void TimeResult_UpdateDigitReveal(struct ChallengeTimeResult *result)
{
    if (++result->timer > 0x3c || (gPadState[1] & 1) != 0) {
        Sound_PlaySong(0x167);
        TimeResult_SetElapsedDigits(result);
        TimeResult_SetState(result, TimeResult_UpdateResultHold);
    } else {
        struct AnimSpriteResource *graphics = &result->digitGraphics;
        const u16 *table = gRandomTable;
        struct AnimSprite *digit = result->digits;
        s32 i;
        for (i = 0; i < 6; i++) {
            s32 value = ParticleRandom_ReadSample(table);
            TimeResult_LoadDigit(digit, graphics, sub_08249350(value >> 4, 10));
            digit++;
        }
        if (result->timer & 1) {
            Sound_PlaySong(0x273);
        }
    }
}

void TimeResult_UpdateFadeIn(struct ChallengeTimeResult *result)
{
    result->timer++;
    if (result->timer > 0x20)
        TimeResult_BeginDigitReveal(result);
}

void TimeResult_UpdateFadeOut(struct ChallengeTimeResult *result)
{
    result->timer++;
    if (result->timer > 0x20) {
        if (result->endScript) {
            Script_Call(result->endScript, 0);
            Task_Kill(result);
        }
    }
}

void TimeResult_UpdateInput(struct ChallengeTimeResult *result)
{
    TimeResult_HandleInput(result);
}

s32 TimeResult_ShowLabel(struct ChallengeTimeResult *result)
{
    struct AnimSprite *label = &result->label;
    label->flags &= ~1;
}

void TimeResult_UpdateBackground(struct ChallengeTimeResult *result)
{
    BgGfx_LoadTilemapAtPosition(2, 0, 0);
}

void TimeResult_Update(struct ChallengeTimeResult *result)
{
    TimeResult_UpdateBackground(result);
    TimeResult_ShowLabel(result);
    if (result->stateCallback != 0)
        Callback_InvokeActor(result, result->stateCallback);
}

void TimeResult_Destroy(struct ChallengeTimeResult *result)
{
    s32 i;
    for (i = 0; i < 6; i++)
        Anim_Remove(&result->digits[i]);
    Anim_Remove(&result->category);
    Anim_Remove(&result->label);
}

void TimeResult_InitializeBackground(struct ChallengeTimeResult *result)
{
    u16 *control = &gDispCntShadow;
    u32 mask = 0xfffff0ff;
    u32 flags;
    void *script;
    u32 layers;
    mask &= *control;
    flags = mask | 0x400;
    *control = flags;
    script = ScriptProc_Start(0xc091, 0x3536);
    result->backgroundResource = script;
    layers = 12;
    Bg_SetupLayers(1, 0, script, 0, 0, 1, &layers);
}

void TimeResult_LoadPalette(struct ChallengeTimeResult *result)
{
    struct TimeResultPalette *resource = ScriptProc_Start(0x92B3, 0x204);
    result->paletteSource = resource->colors;
    CpuSet(result->paletteSource, gBgPaletteBuffer, 0x04000028);
}

void TimeResult_SetElapsedDigits(struct ChallengeTimeResult *result)
{
    u16 positions[6][2];
    s32 hours;
    s32 minutes;
    s32 seconds;
    s32 i;

    memcpy(positions, gUnk_0824FA00, sizeof(positions));
    hours = Div(result->elapsedFrames, 216000);
    minutes = Div(result->elapsedFrames, 3600) - hours * 60;
    seconds = Div(result->elapsedFrames, 60) - hours * 3600 - minutes * 60;
    if (hours > 99) {
        TimeResult_LoadDigit(&result->digits[5], &result->digitGraphics, 9);
        TimeResult_LoadDigit(&result->digits[4], &result->digitGraphics, 9);
        TimeResult_LoadDigit(&result->digits[3], &result->digitGraphics, 5);
        TimeResult_LoadDigit(&result->digits[2], &result->digitGraphics, 9);
        TimeResult_LoadDigit(&result->digits[0], &result->digitGraphics, 5);
        TimeResult_LoadDigit(&result->digits[2], &result->digitGraphics, 9);
    } else {
        TimeResult_LoadDigit(&result->digits[4], &result->digitGraphics, Mod(hours, 10));
        hours = Div(hours, 10);
        TimeResult_LoadDigit(&result->digits[5], &result->digitGraphics, hours);
        TimeResult_LoadDigit(&result->digits[2], &result->digitGraphics, Mod(minutes, 10));
        minutes = Div(minutes, 10);
        TimeResult_LoadDigit(&result->digits[3], &result->digitGraphics, minutes);
        TimeResult_LoadDigit(&result->digits[0], &result->digitGraphics, Mod(seconds, 10));
        seconds = Div(seconds, 10);
        TimeResult_LoadDigit(&result->digits[1], &result->digitGraphics, seconds);
    }
    for (i = 0; i < 6; i++) {
        result->digits[i].position.x = positions[i][0] + 0x90;
        result->digits[i].position.y = positions[i][1] + 0x38;
    }
}

void TimeResult_InitCategoryDisplay(struct ChallengeTimeResult *result)
{
    s32 r = ChallengeMenu_GetRankKeyframe(result->rank);
    if (r >= 0) {
        Anim_LoadKeyframe(&result->category, &result->categoryGraphics, (u16)r, 0);
        result->category.position.x = 0x6f;
        result->category.position.y = -26;
        Anim_LoadKeyframe(&result->label, &result->categoryGraphics, 0x5d, 0);
    }
}

void TimeResult_InitAnimations(struct ChallengeTimeResult *result)
{
    struct AnimSpriteResource *resource = ScriptProc_Start(0xCB05, 0x1289);
    s32 i;
    result->categoryGraphics = *resource;
    ResourceHeader_Relocate(&result->categoryGraphics, resource);
    resource = ScriptProc_Start(0xCB05, 0xE2AB);
    result->digitGraphics = *resource;
    ResourceHeader_Relocate(&result->digitGraphics, resource);
    AnimSprite_InitLinked(&result->category, &result->categoryGraphics, 0, 0x10, 1, 0, 0x3C, 0);
    AnimSprite_InitLinked(&result->label, &result->categoryGraphics, 0, 0x10, 2, 0, 0x3C, 0);
    for (i = 0; i < 6; i++)
        AnimSprite_InitLinked(&result->digits[i], &result->digitGraphics, 0, 0x31, 2, 0, 0x3C, 0);
    TimeResult_InitCategoryDisplay(result);
    TimeResult_SetElapsedDigits(result);
    result->category.flags |= 1;
}

s32 TimeResult_InitializeFromScript(struct ChallengeTimeResult *result)
{
    s32 left, right;
    if (Script_SeekToKeyword(0x65))
        result->endScript = Script_GetValue();
    if (Script_SeekToKeyword(0x6c))
        left = Script_GetValue();
    else
        left = 0;
    if (Script_SeekToKeyword(0x72))
        right = Script_GetValue();
    else
        right = 0;
    if (Script_SeekToKeyword(0x41)) {
        s32 category = Script_GetValue();
        result->rank = category;
        if ((u32)(category - 1) > 4)
            result->rank = 1;
    } else {
        result->rank = 1;
    }
    result->elapsedFrames = left - right;
    result->stateCallback = 0;
    result->timer = 0;
    TimeResult_InitializeBackground(result);
    TimeResult_InitAnimations(result);
    TimeResult_LoadPalette(result);
    TimeResult_BeginFadeIn(result);
    return 0;
}

struct ChallengeTimeResult *TimeResult_Create(void)
{
    struct ChallengeTimeResult *p = Task_Create(0xb, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))TimeResult_Update,
                          (void (*)(void *))TimeResult_Destroy);
        if (TimeResult_InitializeFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

STATE_COMMAND_GO(Actor_UpdateStateTwo, 2)

STATE_COMMAND_RESTART(Actor_UpdateStateThree, 3)

STATE_COMMAND_RESTART(Actor_UpdateStateFour, 4)

void Actor_DispatchPrimaryState(u32 context, struct StateActor *actor)
{
    sub_08249240(context, actor, gUnk_08612160[actor->state]);
}
