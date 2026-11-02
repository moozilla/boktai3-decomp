#include "game_start_menu.h"
#include "display_sprite_playback.h"
#include "game_state.h"
#include "ram.h"
#include "render.h"
#include "menu_parameters.h"

struct GameStartPos {
    u32 x : 16;
    u32 y : 16;
    u32 z;
};
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void *ScriptProc_Start(u32, u32);
s32 ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void Anim_SetSequence(void *, void *, u16, u32, u32);
void Menu_StartFadeIn(u32);
s32 GameStart_LoadAvailableSave(void *);
void Sound_PlaySong(s32);
void GameStart_RunInitHook(void *);

// Tentative: sets up the Game Start screen (backgrounds, widgets, cursor sprite, save state).
s32 GameStart_Init(struct GameStart *p)
{
    u32 layers;
    struct GameStartPos pos;
    void *res;
    s32 i;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        void *bg = ScriptProc_Start(0xc091, 0x3536);
        layers = 4;
        Bg_SetupLayers(2, 0, bg, 0, 0, 1, &layers);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((u8 *)ScriptProc_Start(0x92b3, 0x204)) + 0x14, gBgPaletteBuffer, 0x100);
    res = ScriptProc_Start(0xcb05, 0xe2ab);
    ResourceHeader_Relocate((u8 *)&p->backgroundResource, res);
    p->itemResource = p->backgroundResource;
    Widget_InitLinked((u8 *)&p->background, (u8 *)&p->backgroundResource, 0x36, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate((u8 *)&p->decorationResource, res);
    Widget_InitLinked((u8 *)&p->decoration, (u8 *)&p->decorationResource, 0x28, 0x10, 0, 0, 0, 0);
    p->decoration.flags |= 1;
    p->decoration.position.x = 0;
    {
        s16 *targetY = &p->decoration.position.y;
        s32 k1 = 0xffe0;
        *targetY = k1;
    }
    {
        u32 group = 0xcb05;
        res = ScriptProc_Start(group, 0x530d);
    }
    ResourceHeader_Relocate((u8 *)&p->titleResource, res);
    Widget_InitLinked((u8 *)&p->title, (u8 *)&p->titleResource, 0x8a, 0x10, 0, 0, 0, 0);
    p->title.position.x = 0;
    {
        s16 *targetY = &p->title.position.y;
        s32 valueY = 0xfff8;
        *targetY = valueY;
    }
    AnimData_Load(p->cursorGraphics, 0x593a);
    Sprite_Init((u8 *)&p->cursor, p->cursorGraphics, 0x11);
    p->cursor.position.x = 0x18;
    p->cursor.position.y = 0x2c;
    p->cursorResource = ScriptProc_Start(0x922e, 0x4bbf);
    Anim_SetSequence((u8 *)&p->cursorSequence, p->cursorResource, 0, 0, 0);
    PlayerCue_StepSequenceEnded(&p->cursor, &p->cursorSequence);
    pos.x = 0xa8;
    pos.y = 0x28;
    for (i = 0; i <= 4; i++) {
        Widget_InitLinked((u8 *)&p->timeDigits[i], (u8 *)&p->itemResource, (i == 2) ? (0x14) : (0),
                          0x30, 1, 0, 0x3c, &pos);
        p->timeDigits[i].position.x += i * 8;
        p->timeDigits[i].flags |= 1;
    }

    pos.x = 0xc0;
    pos.y = 0x38;
    for (i = 0; i <= 1; i++) {
        Widget_InitLinked((u8 *)&p->levelDigits[i], (u8 *)&p->itemResource, 0, 0x30, 1, 0, 0x3c,
                          &pos);
        p->levelDigits[i].position.x += i * 8;
        p->levelDigits[i].flags |= 1;
    }

    p->timer = 0;
    if (gListMenuParameters.b == 1)
        Menu_StartFadeIn(0);
    else
        Menu_StartFadeIn(3);
    if (gListMenuParameters.b == 0) {
        u8 *name = gAnimPanelSavedSelections;
        u8 *slot = gNameEntrySavedState;
        u8 *end = name;
        s32 blank = 0x41;
        u8 *q = name + 0x17;
        do {
            *q = blank;
            q--;
        } while (((s32)q) >= ((s32)end));
        name[0x18] = 0;
        slot[0] = 4;
        slot[1] = 0;
        if (gListMenuParameters.b == 0) {
            p->saveIndex = GameStart_LoadAvailableSave(p);
            Sound_PlaySong(0xb3);
            goto check;
        }
    }
    if ((s16)gGameState->flag86E != 0)
        GameState_RestoreActiveRegions();
check:
    if ((s16)gGameState->flag86E == 0)
        p->active = 1;

    GameStart_DrawSaveInfo(p);
    GameStart_RunInitHook(p);
    return 0;
}
