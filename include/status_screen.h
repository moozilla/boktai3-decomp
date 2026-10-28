#ifndef STATUS_SCREEN_H
#define STATUS_SCREEN_H
#include "global.h"
#include "engine.h"
#include "render.h"

// Tentative: a full-screen status/info page. A background map and palette are loaded from script
// resources, the page is revealed through a scanline-noise (horizontal offset) transition while
// its palette fades in, shows numeric counters, waits for confirm (A or START), then fades out
// and calls the script given by keyword 'e' with the arguments given by keyword 'a'.

// Argument block handed to Script_Call.
struct StatusScreenArgs {
    u16 count, pad;
    u32 *values;
};

struct StatusScreen;
typedef void (*StatusScreenFn)(struct StatusScreen *);

struct StatusScreen {
    u8 task[24];
    u16 id;
    u8 flags, bg;
    u16 mapId, paletteId;
    u16 *buffer;      // working palette (gBgPaletteBank13), 2 banks of 16 colors
    u16 *source;      // palette resource the fades blend towards
    u16 dirty;        // bit 0/1/2: numeric displays to redraw; bit 3: palette upload
    u16 delay;        // frames to wait before starting (keyword 'w')
    u16 rows[2][160]; // per-scanline horizontal offsets, double buffered
    u16 active;       // index of the rows buffer being displayed
    s16 amplitude;    // noise amplitude, 0..256
    u16 delta;        // amplitude change per frame (may wrap negative)
    u16 top, bottom;  // scanline range the noise applies to
    u16 times[6];     // phase durations (keyword 't'), see gUnk_08604B3C for defaults
    u16 start, end;   // palette entries (per bank) the fade affects
    u16 blend;        // color the fade blends with
    u16 fade;         // fade progress, 0..256
    u16 fadeStep;
    StatusScreenFn state;  // current phase
    StatusScreenFn effect; // palette fade in progress, or NULL
    StatusScreenFn noise;  // scanline noise updater, or NULL
    u16 timer, pad;
    struct StatusScreenArgs args;
    u32 values[8];
    u32 script; // script called when the screen closes (keyword 'e')
};

// Game state counters shown by the numeric displays (only the fields used here).
struct StatusGameState {
    u8 pad0[14];
    u8 mode; // 0, 4 or 5 select the "alternate" counter pair
    u8 pad0F;
    u8 counters[2];    // shown when mode is not 0, 4 or 5
    u8 altCounters[2]; // shown when mode is 0, 4 or 5
};
extern struct StatusGameState gSunClock;

u16 *StatusScreen_GetTilePtr(u32 bg, u32 x, s32 y); // address of a tile in the background's tilemap
void NumericDisplay_DrawPair(void *unused, int left, int right);
void NumericDisplay_DrawStatusValues(struct StatusScreen *);
void StatusScreen_DrawClock(struct StatusScreen *);
void StatusScreen_RefreshDirtyDisplays(struct StatusScreen *);
int StatusScreen_CheckConfirm(struct StatusScreen *);
void StatusScreen_Close(struct StatusScreen *);
void StatusScreen_BeginClose(struct StatusScreen *);
void StatusScreen_WaitForClose(struct StatusScreen *);
void StatusScreen_Reveal(struct StatusScreen *);
void StatusScreen_WaitForReveal(struct StatusScreen *);
void StatusScreen_FadeAndExit(struct StatusScreen *);
void StatusScreen_WaitForStart(struct StatusScreen *);
void StatusScreen_FadeInPalette(struct StatusScreen *);
void StatusScreen_FadeOutPalette(struct StatusScreen *);
void StatusScreen_ClearScanlineNoise(struct StatusScreen *);
void sub_0800E974(struct StatusScreen *);
void StatusScreen_UpdateScanlineNoise(struct StatusScreen *);

s32 Div(s32, s32);
s32 SunClock_GetBaseHour(void); // counter values
s32 SunClock_GetBaseMinute(void);
u32 SunClock_GetPackedDate(void);
void Clock_UnpackDate(s32 *, s32 *, s32 *, u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32,
                                  s32); // (guess) start a screen transition

int StatusScreen_Update(struct StatusScreen *);
int StatusScreen_Destroy(struct StatusScreen *);
int StatusScreen_Initialize(struct StatusScreen *, u16 id);

extern const u16 gUnk_08604B3C[]; // default phase durations
void Bg_SetPriority(u32, s32);
void Bg_SetupLayer(u32, u32, void *, s32, s32, s32, s32, void *);
void *Video_GetBackgroundMap(u32);
extern struct StatusScreen *gStatusScreen; // the live status screen, or NULL
extern u16 gPadState[];                    // input: [1] = newly pressed keys

#endif
