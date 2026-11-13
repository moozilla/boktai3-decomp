#include "pause_menu.h"
#include "message_window.h"
#include "scrolling_dialogue.h"
#include "player_cue_sequence.h"

void PauseMenu_StartPaletteTransition(struct SceneHudState *hud, u16 sourceBank, u16 targetBank,
                                      s32 duration)
{
    u16 *source;
    u16 *destination;
    s32 i;
    hud->blendSourceBank = sourceBank;
    hud->blendTargetBank = targetBank;
    source = (u16 *)((u8 *)gPaletteBankData + (hud->blendSourceBank << 5));
    destination = hud->blendedPalette;
    i = 15;
    do {
        *destination = *source;
        source++;
        destination++;
        i--;
    } while (i >= 0);
    hud->blendDuration = duration;
    hud->blendFrame = 0;
    hud->blendActive = 1;
}

s32 PauseMenu_TickPaletteBlend(struct SceneHudState *hud)
{
    u16 *fromPalette;
    u16 *toPalette;
    s32 colorIndex;
    s32 remainingFrames;
    u8 *framep;
    u8 *durp;
    u32 channelShift = 5;
    u32 channelMask = 31;
    if (!hud->blendActive)
        return 1;
    fromPalette = (u16 *)(((u8 *)gPaletteBankData) + (hud->blendSourceBank << channelShift));
    toPalette = (u16 *)(((u8 *)gPaletteBankData) + (hud->blendTargetBank << channelShift));
    framep = &hud->blendFrame;
    ++*framep;
    durp = &hud->blendDuration;
    if (*framep >= *durp) {
        u16 *dst = hud->blendedPalette;
        colorIndex = 15;
        do {
            *dst = *toPalette;
            toPalette++;
            dst++;
            colorIndex--;
        } while (colorIndex >= 0);
        hud->blendActive = 0;
        return 1;
    }
    remainingFrames = (*durp) - (*framep);
    {
        for (colorIndex = 0; colorIndex <= 15; colorIndex++) {
            u8 *framep = &hud->blendFrame;
            u8 *durp = &hud->blendDuration;
            {
                if (colorIndex != 5 && colorIndex != 13) {
                    u32 fromColor = *fromPalette;
                    u32 fromRed;
                    u32 fromGreen;
                    u32 fromBlue;
                    u32 toColor;
                    u32 toRed;
                    u32 toGreen;
                    u32 toBlue;
                    s32 red;
                    s32 green;
                    s32 blue;
                    fromRed = channelMask;
                    fromRed &= fromColor;
                    fromGreen = (fromColor << 16) >> 21;
                    fromGreen &= channelMask;
                    fromBlue = ((fromColor << 16) >> 26) & channelMask;
                    toColor = *toPalette;
                    toRed = toColor & 31;
                    toGreen = ((toColor << 16) >> 21) & channelMask;
                    toBlue = ((toColor << 16) >> 26) & channelMask;
                    red = Div((fromRed * remainingFrames) + (toRed * (*framep)), *durp);
                    green = Div((fromGreen * remainingFrames) + (toGreen * (*framep)), *durp);
                    blue = Div((fromBlue * remainingFrames) + (toBlue * (*framep)), *durp);
                    hud->blendedPalette[colorIndex] =
                        ((blue << 10) | (green << channelShift)) | red;
                }
            }
            fromPalette++;
            toPalette++;
        }
    }
    return 0;
}

static inline s32 ScaleBend(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline s32 BendCosine(s32 angle, s32 bend)
{
    return ScaleBend(gUnk_086149C4[(angle + 0x40) & 255] * bend);
}

static inline s32 BendSine(s32 angle, s32 bend)
{
    const s16 *tbl = gUnk_086149C4;
    s32 wrapped = 255;
    wrapped &= angle;
    return ScaleBend(tbl[wrapped] * bend);
}

void PauseMenu_BeginSpriteMotion(struct SceneHudState *hud, u32 index, u32 keyframe, u32 palette,
                                 struct SceneVec *from, struct SceneVec *to, s32 bend, u32 duration)
{
    struct SceneHudWidgetMotion *motion = &hud->widgetMotions[index];
    struct SceneInputWidget *widget;
    motion->from = *from;
    motion->to = *to;
    if (bend != 0) {
        s32 angle = Math_Atan2(to->x - from->x, to->y - from->y);
        angle += 64;
        angle &= 255;
        motion->bendX = BendCosine(angle, bend);
        motion->bendY = BendSine(angle, bend);
    } else {
        motion->bendX = bend;
        motion->bendY = bend;
    }
    widget = &hud->motionWidgets[index];
    Anim_LoadKeyframe(widget, hud->motionResource, (u16)keyframe, 1);
    PauseMenu_SetSpritePalette(widget, palette);
    *(struct SceneVec *)&widget->position = motion->from;
    widget->flags &= ~1;
    motion->timer = 0;
    motion->duration = duration;
    motion->active = 1;
}

u32 PauseMenu_UpdateSpriteMotion(struct SceneHudState *hud, u32 index)
{
    struct SceneInputWidget *widget = &hud->motionWidgets[index];
    struct SceneHudWidgetMotion *motion = &hud->widgetMotions[index];
    s32 remaining;
    if (motion->active == 0)
        return 1;
    motion->timer++;
    if (motion->timer >= motion->duration) {
        *(struct SceneVec *)&widget->position = motion->to;
        motion->active = 0;
        return 1;
    }
    remaining = motion->duration - motion->timer;
    widget->position.x =
        Div(motion->from.x * remaining + motion->to.x * motion->timer, motion->duration);
    widget->position.y =
        Div(motion->from.y * remaining + motion->to.y * motion->timer, motion->duration);
    if (*(u32 *)&motion->bendX) {
        s32 wave = gUnk_086149C4[Div(motion->timer * 128, motion->duration) & 255];
        widget->position.x += ScaleBend(motion->bendX * wave);
        widget->position.y += ScaleBend(motion->bendY * wave);
    }
    return 0;
}

void PauseMenu_OpenPrompt(s32 messageTable, s32 message, s32 playSound)
{
    if (playSound != 0)
        Sound_PlaySong(0x107);
    ScrollingDialogue_ClearScript();
    MessageWindow_Clear();
    MessageWindow_SetMessageTable(messageTable);
    MessageWindow_SetBounds(0, 0x10, 0x1e, 2);
    MessageWindow_ShowMessage(message);
    MessageWindow_SetDisplayMode(1);
}

void PauseMenu_SetPromptIconPositions(struct SceneHudState *hud, u32 x, u32 y)
{
    hud->valueWidgets[0].position.x = x;
    hud->valueWidgets[0].position.y = y;
    hud->valueWidgets[1].position.x = x;
    hud->valueWidgets[1].position.y = y + 8;
}

s32 PauseMenu_GetPromptIconKeyframe(s32 icon)
{
    if (icon <= 1)
        return icon + 0x7e;
    return icon + 0x7f;
}

void PauseMenu_SetPairedIcons(struct SceneHudState *hud, s32 left, s32 right)
{
    if (left < 0) {
        PauseMenu_AddSpriteFlags(hud, 75, 1);
    } else {
        PauseMenu_ClearSpriteFlags(hud, 75, 1);
        Anim_LoadKeyframe(&hud->valueWidgets[0], hud->widgetResource,
                          (u16)PauseMenu_GetPromptIconKeyframe(left), 0);
    }
    if (right < 0) {
        PauseMenu_AddSpriteFlags(hud, 76, 1);
    } else {
        PauseMenu_ClearSpriteFlags(hud, 76, 1);
        Anim_LoadKeyframe(&hud->valueWidgets[1], hud->widgetResource,
                          (u16)PauseMenu_GetPromptIconKeyframe(right), 0);
    }
}

void PauseMenu_HidePromptIcons(struct SceneHudState *hud)
{
    PauseMenu_SetPairedIcons(hud, -1, -1);
}

void PauseMenu_CloseTextWindows(struct SceneHudState *hud)
{
    s32 *window = &hud->primaryTextWindow;
    s32 i = 7;
    do {
        TextWindow_Clear(*window++);
        i--;
    } while (i >= 0);
}

void PauseMenu_ShowPromptSprite(struct SceneHudState *hud, s16 x, s16 y)
{
    hud->promptSprite.flags &= ~1;
    hud->promptSprite.position.x = x;
    hud->promptSprite.position.y = y;
    hud->promptSprite.position.z = 0;
    Anim_SetSequence(&hud->promptSequence, hud->promptSequenceResource, 0, 0, 0);
    PlayerCue_StepSequence(&hud->promptSprite, &hud->promptSequence);
}

void PauseMenu_LoadCategoryIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                     s32 icon)
{
    Anim_LoadKeyframe(widget, hud->motionResource, (u16)(icon + 0xf0), 0);
}

void PauseMenu_LoadEquipmentIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                      s32 icon)
{
    Anim_LoadKeyframe(widget, hud->motionResource, (u16)(icon + 0xd9), 0);
}

void PauseMenu_LoadAttributeIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                      s32 icon)
{
    if (icon <= 5)
        Anim_LoadKeyframe(widget, hud->motionResource, (u16)(icon + 0x184), 1);
    else
        Anim_LoadKeyframe(widget, hud->motionResource, (u16)(icon + 0x19c), 1);
}

void PauseMenu_LoadStatusIconFrame(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                   s32 icon)
{
    Anim_LoadKeyframe(widget, hud->motionResource, (u16)(icon + 0xe7), 0);
}

void PauseMenu_LoadEmptySlotFrame(struct SceneHudState *hud, struct SceneInputWidget *widget)
{
    Anim_LoadKeyframe(widget, hud->motionResource, 0xc9, 1);
}

void PauseMenu_LoadFixedIconKeyframe(struct SceneHudState *hud, struct SceneInputWidget *widget)
{
    Anim_LoadKeyframe(widget, hud->motionResource, 0xc8, 1);
}

void PauseMenu_LoadIndexedIconKeyframe(struct SceneHudState *hud, struct SceneInputWidget *widget,
                                       s32 icon)
{
    Anim_LoadKeyframe(widget, hud->widgetResource, (u16)(icon + 0x90), 0);
}
