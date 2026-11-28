#include "global.h"
#include "engine/engine.h"
#include "ui/menu_parameters.h"
#include "engine/runtime.h"
#include "ui/title/title_display.h"

struct Params3D0Gfx {
    u8 pad[0x6220];
};

void TimeDisplay_InitBackgrounds(struct Display *);
void TimeDisplay_InitAnimatedObjects(struct Display *);
void TimeDisplay_SetSelectionKeyframe(struct Display *);
void TutorialOtenko_HideAuxiliarySprites(struct Display *);
void BgGfx_LoadTilemapAtPosition(s32, s32, s32);
void Menu_StartFadeIn(s32);

s32 TitleDisplay_Initialize(struct Display *p)
{
    s32 i;
    p->slide = -240;
    p->unused760 = 8;
    TimeDisplay_InitBackgrounds(p);
    TimeDisplay_InitAnimatedObjects(p);
    if ((s16)gTimeDisplayParameters.silent == 1) {
        p->selection = gGameState->timeDisplaySelection;
        p->widgets[0].flags &= ~1;
        p->widgets[6].flags &= ~1;
        p->widgets[7].flags &= ~1;
        p->widgets[1].flags &= ~1;
        p->widgets[2].flags &= ~1;
        p->widgets[8].flags &= ~1;
        BgGfx_LoadTilemapAtPosition(2, 0, 0);
        p->state = p->lastState = 8;
        TimeDisplay_SetSelectionKeyframe(p);
        TutorialOtenko_HideAuxiliarySprites(p);
    } else {
        p->selection = 0;
        p->state = p->lastState = 0;
    }
    if (gTimeDisplayParameters.M == 1)
        p->count = 1;
    else
        p->count = gSessionState[9] ? 4 : 3;
    p->pairFrame = 0;
    p->pairTicks = 0;
    Sound_PlaySong(0xa4);
    p->fadeShift = gTimeDisplayParameters.fadeOut;
    Menu_StartFadeIn(gTimeDisplayParameters.fadeOut);
    CpuSet(gPaletteBankData + 0x6220, p->palettes[0], 0x100);
    for (i = 0; i < 16; i++)
        p->palettes[16][i] = 0x7fff;
    return 0;
}
