#include "global.h"
#include "game_state.h"
#include "input.h"

struct LinkScorePanel {
    u8 pad0[0x18];
    void *layerMap;
    u8 pad1C[0xfe4 - 0x1c];
    void (*update)(struct LinkScorePanel *);
    u8 padFE8[4];
    const u8 *script;
    u8 padFF0[0x1000 - 0xff0];
    u8 state;
    u8 pad1001[7];
    s32 windows[7];
    u8 pad1024;
    u8 pad1025;
    u16 timer;
    u8 pad1028[6];
    u16 skip;
};
void Bg_SetLayerMap(u32, void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
s32 TextWindow_SetScript(s32 id, const u8 *script);
s32 TextWindow_Refresh(s32 id, s32 index);
void TextWindow_StartPlayback(s32);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
void LinkScorePanel_UpdateRollingNumber(struct LinkScorePanel *);
void LinkScorePanel_UpdateNumberReveal(struct LinkScorePanel *);
void LinkScorePanel_SetNumber(struct LinkScorePanel *, s32, s32);
void sub_0811048C(struct LinkScorePanel *);

static inline void SetUpdate(struct LinkScorePanel *p, void (*fp)(struct LinkScorePanel *), u8 st)
{
    p->update = fp;
    *((u8 *)p + 0x1024) = 1;
    p->state = st;
}

void sub_08110650(struct LinkScorePanel *p)
{
    u8 *redraw = (u8 *)p + 0x1024;
    u16 *timer;
    if (*redraw) {
        Bg_SetLayerMap(3, p->layerMap, 3);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        TextWindow_SetScript(p->windows[0], p->script);
        TextWindow_SetScript(p->windows[1], p->script);
        TextWindow_SetScript(p->windows[2], p->script);
        TextWindow_SetScript(p->windows[3], p->script);
        TextWindow_SetScript(p->windows[4], p->script);
        TextWindow_SetScript(p->windows[5], p->script);
        TextWindow_SetScript(p->windows[6], p->script);
        TextWindow_Refresh(p->windows[0], 0);
        TextWindow_Refresh(p->windows[1], 1);
        TextWindow_Refresh(p->windows[2], 2);
        TextWindow_Refresh(p->windows[3], 3);
        TextWindow_Refresh(p->windows[4], 4);
        TextWindow_Refresh(p->windows[5], 5);
        TextWindow_Refresh(p->windows[6], 6);
        TextWindow_StartPlayback(p->windows[0]);
        TextWindow_StartPlayback(p->windows[1]);
        TextWindow_StartPlayback(p->windows[2]);
        TextWindow_StartPlayback(p->windows[3]);
        TextWindow_StartPlayback(p->windows[4]);
        TextWindow_StartPlayback(p->windows[5]);
        TextWindow_StartPlayback(p->windows[6]);
        p->timer = 0;
        *redraw = 0;
    }
    timer = &p->timer;
    if (*timer > 9) {
        CharacterModel_UpdatePairedAnimations((u8 *)p + 0xd8c, 0);
        PaletteFade_CycleMenuColors((u8 *)p + 0x770);
        if (!p->skip) {
            SetUpdate(p, sub_0811048C, 1);
        } else {
            if (gPadState[1] & 0x30b)
                p->skip = 1;
            LinkScorePanel_UpdateRollingNumber(p);
            LinkScorePanel_UpdateNumberReveal(p);
            LinkScorePanel_SetNumber(p, 1, *(s16 *)((u8 *)gGameState + 0x7b2));
            (*timer)++;
        }
    } else {
        (*timer)++;
    }
}
