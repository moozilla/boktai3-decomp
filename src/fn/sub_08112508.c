#include "global.h"
#include "game_state.h"

struct StageResults {
    u8 pad0[0x18];
    void *layerMap;
    u8 pad1C[0x944 - 0x1c];
    u8 palette[0xf60 - 0x944];
    u8 models[0x11b8 - 0xf60];
    void (*update)(struct StageResults *);
    const u8 *script;
    u8 pad11C0[0x11c8 - 0x11c0];
    u8 state;
    u8 pad11C9;
    u8 redraw;
    u8 pad11CB[0x11d0 - 0x11cb];
    s32 windows[2];
    s32 times[12];
    u8 record;
    u8 blink;
    u16 timer;
};
extern u16 gUnk_03005260[];
void Bg_SetLayerMap(u32, void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
s32 TextWindow_SetScript(s32 id, const u8 *script);
s32 TextWindow_Refresh(s32 id, s32 index);
void TextWindow_StartPlayback(s32);
s32 GameState_GetResultTimer(void);
s32 GameState_GetResultIndex(void);
s32 GameState_GetActionCount894(void);
s32 Div(s32, s32);
void SceneGauge_SelectThresholdFrame(struct StageResults *, s32);
u8 StageResults_RecordStagePerformance(void);
void StageResults_ShowTimeDigits(struct StageResults *, s32, s32);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
void StageResults_UpdateRecordBlink(struct StageResults *);
void Sound_PlaySong(u32);
void MessagePanel_UpdateExitDelay(struct StageResults *);

#define SCORE (*(s16 *)((u8 *)gGameState + 0x7b2))

#define CLAMP(x, max)                                                                              \
    {                                                                                              \
        s32 r_ = (x);                                                                              \
        if ((x) > (max))                                                                           \
            r_ = (max);                                                                            \
        else if ((x) < 0)                                                                          \
            r_ = 0;                                                                                \
        (x) = r_;                                                                                  \
    }

static inline void SetUpdate(struct StageResults *p, void (*fp)(struct StageResults *), u8 st)
{
    p->update = fp;
    p->redraw = 1;
    p->state = st;
}

void sub_08112508(struct StageResults *p)
{
    u16 *timer;
    if (p->redraw) {
        s32 elapsed, timeScore, actionScore, total;
        p->record = 0;
        Bg_SetLayerMap(3, p->layerMap, 4);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        TextWindow_SetScript(p->windows[0], p->script);
        TextWindow_SetScript(p->windows[1], p->script);
        TextWindow_Refresh(p->windows[0], 0);
        TextWindow_Refresh(p->windows[1], 1);
        TextWindow_StartPlayback(p->windows[0]);
        TextWindow_StartPlayback(p->windows[1]);
        elapsed = GameState_GetResultTimer();
        timeScore = Div(p->times[GameState_GetResultIndex()] - elapsed, 60) + 25;
        actionScore = Div(20, GameState_GetActionCount894() + 1);
        CLAMP(timeScore, 80);
        CLAMP(actionScore, 20);
        total = timeScore + actionScore;
        CLAMP(total, 999);
        SceneGauge_SelectThresholdFrame(p, total);
        p->record = StageResults_RecordStagePerformance();
        {
            s32 sum = SCORE + total;
            if (sum > 9999)
                sum = 9999;
            else if (sum < 0)
                sum = 0;
            SCORE = sum;
        }
        StageResults_ShowTimeDigits(p, 8, elapsed);
        StageResults_ShowTimeDigits(p, 14, timeScore);
        StageResults_ShowTimeDigits(p, 17, actionScore);
        StageResults_ShowTimeDigits(p, 20, total);
        p->blink = 0;
        p->timer = 0;
        p->redraw = 0;
    }
    timer = &p->timer;
    if (*timer > 9) {
        CharacterModel_UpdatePairedAnimations(p->models, 0);
        PaletteFade_CycleMenuColors(p->palette);
        StageResults_UpdateRecordBlink(p);
        if (gUnk_03005260[1] & 0x30f) {
            Sound_PlaySong(0xdd);
            SetUpdate(p, MessagePanel_UpdateExitDelay, 1);
        } else {
            (*timer)++;
        }
    } else {
        (*timer)++;
    }
}
