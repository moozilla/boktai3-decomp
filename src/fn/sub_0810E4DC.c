#include "global.h"
#include "engine.h"

struct ConfirmPanel {
    u8 f0[0x18];
    void *f18;
    u8 f1C[0x468 - 0x1C];
    u16 pulse[4];
    u8 fade[0xA8C - 0x470];
    u8 models[0xCE4 - 0xA8C];
    u32 callback;
    u8 fCE8[0xD00 - 0xCE8];
    u32 fD00;
    u8 fD04;
    u8 fD05;
    u8 fD06;
    u8 fD07;
    u8 fD08[0xD2C - 0xD08];
    u8 fD2C;
    u8 fD2D;
    u16 fD2E;
};
extern u32 gUnk_030053F0;
extern u16 gUnk_03005260[];
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void RecordPanel_DrawTime(struct ConfirmPanel *);
void Video_DrawClockAndValueOnBackground(u32);
void CharacterModel_UpdatePairedAnimations(void *, u32);
void PaletteFade_CycleMenuColors(void *);
void Palette_UpdateHighlightPulse(u16 *, u8 *);
void sub_0810DE88(struct ConfirmPanel *);
void SelectionPanel_UpdateDescription(struct ConfirmPanel *);
u32 SelectionPanel_GetSelection(struct ConfirmPanel *);
u8 StateFlags_TestIndexedFlag(u32);
void Sound_PlaySong(u32);
void Registry3C9F_OnCloseRequested(void);
void SelectionPanel_UpdateConfirmTransition(void);
void SelectionPanel_UpdateCancelTransition(void);

static inline u8 TestSel(struct ConfirmPanel *p)
{
    if (StateFlags_TestIndexedFlag(SelectionPanel_GetSelection(p)))
        return 1;
    return 0;
}

static inline u32 LinkActive(void)
{
    u32 mask = 0x800;
    return (gEngineFlags | gUnk_030053F0) & mask;
}

static inline void Refresh(struct ConfirmPanel *p)
{
    if (p->fD2C) {
        Bg_SetLayerMap(3, p->f18, 1);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        RecordPanel_DrawTime(p);
        Video_DrawClockAndValueOnBackground(3);
        p->fD2E = 0;
        p->fD07 = 0;
        p->fD2C = 0;
    }
}

// confidence=low: per-frame update of the Registry3C9F record panel; A confirms, B cancels.
void sub_0810E4DC(struct ConfirmPanel *p)
{
    u32 cb;
    u8 one;
    Refresh(p);
    CharacterModel_UpdatePairedAnimations(p->models, 0);
    PaletteFade_CycleMenuColors(p->fade);
    Palette_UpdateHighlightPulse(p->pulse, &p->fD07);
    if (p->fD2E < 30) {
        p->fD2E++;
        return;
    }
    sub_0810DE88(p);
    SelectionPanel_UpdateDescription(p);
    p->fD06 = p->fD00;
    if (gUnk_03005260[1] & 1) {
        if (TestSel(p)) {
            Sound_PlaySong(0xdd);
            if (LinkActive()) {
                cb = (u32)Registry3C9F_OnCloseRequested;
                one = 1;
                p->callback = cb;
                p->fD2C = one;
                p->fD04 = one;
            } else {
                cb = (u32)SelectionPanel_UpdateConfirmTransition;
                one = 1;
                p->callback = cb;
                p->fD2C = one;
                p->fD04 = one;
            }
        } else {
            Sound_PlaySong(0x192);
        }
    } else if (gUnk_03005260[1] & 2) {
        if (!LinkActive()) {
            cb = (u32)SelectionPanel_UpdateCancelTransition;
            p->callback = cb;
            p->fD2C = 1;
            p->fD04 = 2;
            Sound_PlaySong(0xde);
        }
    }
}
