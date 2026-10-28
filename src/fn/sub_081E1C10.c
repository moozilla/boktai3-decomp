#include "global.h"
#include "menu_parameters.h"
// Provisional name: SoundTest_DrawTrackName
struct Sprite {
    u8 head[8];
    u32 flags;
    u8 gap[0x20 - 12];
    s16 x, y;
    u8 tail[0x60 - 0x24];
};
struct Resource {
    u32 words[8];
};
struct Viewer {
    u8 p0[0x20];
    struct Sprite background;
    struct Resource backgroundResource;
    u8 p1[8];
    struct Sprite panel;
    struct Resource panelResource;
    u8 p2[8];
    struct Sprite cursor;
    struct Resource cursorResource;
    u8 p3[8];
    struct Sprite label;
    struct Resource labelResource;
    u8 p4[8];
    struct Resource digitsResource;
    struct Sprite digitsA[3], digitsB[3], labels[3];
    u8 p5[0x5cc - 0x5c0];
    u32 state, selection, countA, countB, countC, textIndex, valueA, valueB;
};
void Anim_LoadKeyframe(void *, void *, u16, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Bg_ClearTilemap(s32);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
s32 Script_ParseStringRef(void *);
const char *Text_LookupString(s32);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(s32, s32, const char *);
// Refresh the selected sound-test entry's text label.
void SoundTest_DrawTrackName(struct Viewer *p)
{
    const char *text;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    text = Text_LookupString(Script_ParseStringRef(gSoundTestParameters.text) + p->textIndex);
    Menu_EraseRect(10, 4, 16, 2);
    Menu_DrawText(10, 4, text);
}
