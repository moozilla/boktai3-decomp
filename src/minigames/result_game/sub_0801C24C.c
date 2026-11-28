#include "global.h"
#include "engine/video/render.h"
#include "minigames/result_game/result_tutorial.h"
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);
s32 Anim_Step(void *anim, void *data);
void Bg_SetTilemapEntry(u32 n, s32 x, s32 y, u32 a, u32 b, u32 c);
void Text_DrawNumber(int, int, int, int, int, int, int);
s32 Div(s32, s32);
s32 Mod(s32, s32);

s32 ResultMenu_UpdateMarkers(struct ResultMenu *menu);

static inline void Clamp(s32 *v)
{
    if (*v < 0)
        *v = 0;
    else if (*v > 0x1600)
        *v = 0x1600;
}

s32 sub_0801C24C(struct ResultMenu *menu)
{
    s32 i;
    u32 hideTime;
    s32 minutes, seconds;
    s32 brightness, red, green, blue;
    u16 *colors;
    s32 position, left;
    s16 *range;
    struct AnimSprite *sprite;
    if (menu->gaugePosition > 0x1600)
        menu->gaugePosition = 0x1600;
    else if (menu->gaugePosition < 0)
        menu->gaugePosition = 0;
    menu->lastPressScore = menu->gaugePosition >> 5;
    Clamp(&menu->playerPosition);
    Clamp(&menu->targetPosition);
    menu->lastPressScore = menu->playerPosition >> 5;
    menu->targetCenter = menu->targetPosition >> 5;
    sprite = &menu->widgets[4];
    sprite->position.x = menu->lastPressScore - 0xb0;
    range = &menu->targetLeft;
    position = (s16)menu->lastPressScore;
    left = *range;
    sprite = &menu->widgets[5];
    if (position < left) {
        if (menu->markerAnimationState != 1) {
            menu->markerAnimationState = 1;
            Anim_SetKeyframes(sprite, &menu->primaryResource, 1, 1, 0);
        }
    } else if (position < menu->targetRight) {
        if (menu->markerAnimationState != 2) {
            menu->markerAnimationState = 2;
            Anim_SetKeyframes(sprite, &menu->primaryResource, 2, 1, 0);
        }
    } else if (menu->markerAnimationState != 3) {
        menu->markerAnimationState = 3;
        Anim_SetKeyframes(sprite, &menu->primaryResource, 3, 1, 0);
    }
    for (i = 0; i < 15; i++)
        Anim_Step(&menu->widgets[i], &menu->primaryResource);
    sprite = &menu->widgets[3];
    sprite->position.x = menu->targetCenter;
    if (menu->hideLevel) {
        for (i = -1; i <= 10; i++)
            Bg_SetTilemapEntry(menu->layer, i + 4, 0x12, 1, 0, 0xf);
    } else {
        for (i = 0; i < (u32)menu->level; i++)
            Bg_SetTilemapEntry(menu->layer, i + 4, 0x12, 0x5d, 0, 0xd);
        for (; i <= 9; i++)
            Bg_SetTilemapEntry(menu->layer, i + 4, 0x12, 0x5e, 0, 0xd);
    }
    hideTime = menu->hideTimer;
    if (hideTime) {
        Bg_SetTilemapEntry(menu->layer, 0x15, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(menu->layer, 0x16, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(menu->layer, 0x17, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(menu->layer, 0x18, 0x12, 1, 0, 0xf);
        Bg_SetTilemapEntry(menu->layer, 0x19, 0x12, 1, 0, 0xf);
    } else {
        Div(Mod(menu->countdown, 60) * 100, 60);
        seconds = Div(menu->countdown, 60);
        minutes = Div(seconds, 60);
        seconds = Mod(seconds, 60);
        Text_DrawNumber(menu->layer, 0x15, 0x12, minutes, 0x11, 2, 0xd);
        Text_DrawNumber(menu->layer, 0x18, 0x12, seconds, 0x11, 2, 0xd);
        Bg_SetTilemapEntry(menu->layer, 0x17, 0x12, 0x2c, hideTime, 0xd);
    }
    sprite = &menu->widgets[12];
    if (menu->pressed) {
        if (menu->pressed <= 3)
            Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xcf, 0);
        else
            Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xd0, 0);
        if (++menu->pressed > 10)
            menu->pressed = 0;
    } else
        Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xce, 0);
    colors = (u16 *)(gPaletteBankData + 0x75a0);
    brightness = menu->gaugeBrightness;
    red = (0x20 - brightness) * 31;
    red = (red + (colors[15] & 31) * brightness) >> 5;
    green = ((0x20 - brightness) * 4 + ((colors[15] >> 5) & 31) * brightness) >> 5;
    blue = (((colors[15] >> 10) & 31) * brightness) >> 5;
    menu->meterPalette[15] = (red & 31) + ((green & 31) << 5) + ((blue & 31) << 10);
    return ResultMenu_UpdateMarkers(menu);
}
