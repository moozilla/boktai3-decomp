#include "actor_stats.h"
#include "render.h"
#include "modifier_list.h"

// Tentative names: PalFade_Start sets up a timed fade between two 16-color palette rows.

s32 Div(s32, s32);

void PalFade_Start(void *self, s32 p1, s32 p2, s32 p3, s32 p4, s32 duration, u32 flags, s32 mode);

static inline s32 Trunc5(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}

void PalFade_Start(void *self, s32 p1, s32 p2, s32 p3, s32 p4, s32 duration, u32 flags, s32 mode)
{
    struct ActorPaletteFade *f = &((struct ActorStats *)self)->fade;
    u16 *src2;
    s32 j;
    s32 k;
    s16 *accrBase;
    s32 idx;
    s32 next;
    struct CharacterPreviewModel **slot;
    f->targetFirst = p1;
    f->targetSecond = p2;
    f->sourceFirst = p3;
    f->sourceSecond = p4;
    for (j = 0; j < 2; j = next) {
        u16 *src1;
        u16 *out;
        s16 *accr;
        s16 *accg;
        s16 *accb;
        s16 *greenBase, *blueBase;
        accrBase = f->channels[0];
        if (j == 0) {
            src1 = (u16 *)(gPaletteBankData + (p1 * 32));
            src2 = (u16 *)(gPaletteBankData + (p3 * 32));
            slot = &((struct ActorStats *)self)->renderers;
            if (mode == 1)
                src2 = f->output;
        } else {
            src1 = (u16 *)(gPaletteBankData + (p2 * 32));
            src2 = (u16 *)(gPaletteBankData + (p4 * 32));
            slot = &((struct ActorStats *)self)->renderers;
            if (mode == 1)
                src2 = f->output + 16;
        }
        accr = (s16 *)(j * 32 + (u32)accrBase);
        greenBase = f->channels[1];
        accg = (s16 *)(j * 32 + (u32)greenBase);
        blueBase = f->channels[2];
        accb = (s16 *)(j * 32 + (u32)blueBase);
        out = f->output;
        next = j + 1;
        for (k = 0; k < 16; k++) {
            s32 n = (j * 16) + k;
            s32 c2 = src2[k];
            f->increments[0][n] = ((src1[k] & 0x1F) - (c2 & 0x1F)) << 5;
            f->increments[1][n] = (((src1[k] >> 5) & 0x1F) - ((c2 >> 5) & 0x1F)) << 5;
            f->increments[2][n] = (((src1[k] >> 10) & 0x1F) - ((c2 >> 10) & 0x1F)) << 5;
            f->increments[0][n] = Div(f->increments[0][n], duration);
            f->increments[1][n] = Div(f->increments[1][n], duration);
            f->increments[2][n] = Div(f->increments[2][n], duration);
            accr[k] = (c2 & 0x1F) << 5;
            accg[k] = ((c2 >> 5) & 0x1F) << 5;
            accb[k] = ((c2 >> 10) & 0x1F) << 5;
            idx = k;
            out[n] = (Trunc5(accr[idx]) | (Trunc5(accg[idx]) << 5)) | (Trunc5(accb[idx]) << 10);
        }
    }

    {
        struct ModifierDisplayObject *q = (*slot)->view.parts;
        k = 4;
        q += 4;
        for (; k >= 0; k--) {
            q->palette = f->output;
            q--;
        }
    }
    f->timer = duration;
    f->duration = duration;
    f->flags = flags;
}

// PalFade_Update steps an active fade by one frame; returns 1 when the fade finishes or restarts.

u32 PalFade_Update(void *self)
{
    struct ActorPaletteFade *f = &((struct ActorStats *)self)->fade;
    s32 j, k;
    u16 *out;

    if (!(f->flags & 7))
        return 0;
    if (f->timer == 0) {
        if (f->flags & 2) {
            if ((f->flags & 8) && f->sourceSecond == ((struct ActorStats *)self)->recoveryValue)
                f->flags = 1;
            PalFade_Start(self, f->sourceFirst, f->sourceSecond, f->targetFirst, f->targetSecond,
                          f->duration, f->flags, 0);
        } else {
            if (!(f->flags & 4)) {
                struct CharacterPreviewModel *model =
                    (void *)((struct ActorStats *)self)->renderers;
                struct ModifierDisplayObject *part = model->view.parts;
                u16 *palette = model->activePalette;
                for (k = 4; k >= 0; k--) {
                    part->palette = palette;
                    part++;
                }
            }
            f->flags = 0;
        }
        return 1;
    }
    for (j = 0; j < 2; j++) {
        // Row base formed as offset + struct address, as in the ROM.
        out = (u16 *)(j * 32 + (u32)f);
        for (k = 0; k < 16; k++) {
            f->channels[0][j * 16 + k] += f->increments[0][j * 16 + k];
            f->channels[1][j * 16 + k] += f->increments[1][j * 16 + k];
            f->channels[2][j * 16 + k] += f->increments[2][j * 16 + k];
            out[k] = Trunc5(f->channels[0][j * 16 + k]) | Trunc5(f->channels[1][j * 16 + k]) << 5 |
                     Trunc5(f->channels[2][j * 16 + k]) << 10;
        }
    }
    if (--f->timer == 0)
        return 1;
    return 0;
}
