#include "global.h"
#include "input.h"
struct StatGaugePalette {
    u8 unknown0[0x18];
    u16 colors[16];
};
struct StatGaugePreview {
    u8 unknown0[0x18];
    u8 backgroundData[0x20];
    u8 cursorData[0x20];
    u8 backgroundAnim[0x60];
    u8 cursorAnim[0x48];
    struct StatGaugePalette *palette;
    u8 unknown104[0x7f0];
    u32 exitScript;
    u8 unknown8f8[0xa];
    u16 selection;
    u8 unknown904[0xb];
    u8 highlightPulse;
};
s32 Anim_Step(void *, void *);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Palette_UpdateHighlightPulse(u16 *, u8 *);
void Sound_PlaySong(u32);
void StatGauge_BeginAdjustment(struct StatGaugePreview *);
void StatGauge_ConfirmPreview(struct StatGaugePreview *);
void DarkLoan_BeginClosing(struct StatGaugePreview *);
void StatGauge_UpdateConfirmation(struct StatGaugePreview *p)
{
    Anim_Step(p->backgroundAnim, p->backgroundData);
    Palette_UpdateHighlightPulse(p->palette->colors, &p->highlightPulse);
    if (gPadState[1] & 1) {
        if (p->selection) {
            Sound_PlaySong(0xdd);
            StatGauge_BeginAdjustment(p);
        } else {
            Sound_PlaySong(0xde);
            if (!p->exitScript)
                StatGauge_ConfirmPreview(p);
            else
                DarkLoan_BeginClosing(p);
        }
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xde);
        if (!p->exitScript)
            StatGauge_ConfirmPreview(p);
        else
            DarkLoan_BeginClosing(p);
    } else if (gPadState[1] & 0x30) {
        Sound_PlaySong(0xdc);
        p->selection = 1 - p->selection;
        if (p->selection)
            Anim_LoadKeyframe(p->cursorAnim, p->cursorData, 0x4b, 1);
        else
            Anim_LoadKeyframe(p->cursorAnim, p->cursorData, 0x4c, 1);
    }
}
