#include "global.h"
#include "input.h"

struct TimePanel {
    u8 f0[0x80];
    u8 f80[0x162 - 0x80];
    u16 f162;
    u8 f164[0x422 - 0x164];
    u16 f422;
    u8 f424[0xD24 - 0x424];
    s8 selA;
    u8 repeat;
    s8 cursor;
    u8 cursorMin;
    u8 cursorMax;
    u8 selMax;
    u8 holdState;
};
extern u8 gUnk_0860654C[];
void Anim_SelectWidgetGlyph(void *, void *, u32);
void RecordPanel_RefreshEntryLabels(struct TimePanel *);
void RecordPanel_DrawTime(struct TimePanel *);
void Sound_PlaySong(u32);

// confidence=low: handles cursor/selection input on the record time panel.
void sub_0810DE88(struct TimePanel *p)
{
    u16 *keys;
    s32 changed = 0;
    if (p->cursorMax != 0) {
        if (gPadState[1] & 0x220) {
            p->cursor--;
            if (p->cursor < p->cursorMin)
                p->cursor = p->cursorMax;
            changed = 1;
        } else if (gPadState[1] & 0x110) {
            p->cursor++;
            if (p->cursor > p->cursorMax)
                p->cursor = p->cursorMin;
            changed = 1;
        }
        if (changed) {
            if (p->cursor == 0) {
                p->f422 = 0x1f;
                p->selMax = gUnk_0860654C[0];
            } else {
                p->f422 = 0x20;
                p->selMax = gUnk_0860654C[1];
            }
            if (p->selA > p->selMax)
                p->selA = p->selMax;
            Anim_SelectWidgetGlyph((u8 *)p + 0x80, (u8 *)p + 0x400, p->f422);
            RecordPanel_RefreshEntryLabels(p);
        }
    }
    {
        if (gPadState[1] & 0x40) {
            p->selA--;
            if (p->selA < 0)
                p->selA = p->selMax;
            changed = 1;
        } else if (gPadState[1] & 0x80) {
            p->selA++;
            if (p->selA > p->selMax)
                p->selA = 0;
            changed = 1;
        }
        keys = gPadState;
        if (keys[0] & 0x40) {
            if (p->holdState & 0x80)
                p->repeat = 0;
            else
                p->repeat++;
            p->holdState = 0x40;
        } else if (keys[0] & 0x80) {
            if (p->holdState & 0x40)
                p->repeat = 0;
            else
                p->repeat++;
            p->holdState = 0x80;
        } else {
            p->repeat = 0;
        }
        if (p->repeat > 10) {
            if (keys[0] & 0x40) {
                p->selA--;
                if (p->selA < 0)
                    p->selA = p->selMax;
            } else if (keys[0] & 0x80) {
                p->selA++;
                if (p->selA > p->selMax)
                    p->selA = 0;
            }
            changed = 1;
        }
    }
    if (changed) {
        RecordPanel_DrawTime(p);
        p->repeat = 0;
        Sound_PlaySong(0xdc);
    }
    p->f162 = p->selA << 4;
}
