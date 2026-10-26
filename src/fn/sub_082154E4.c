#include "global.h"
#include "render.h"
struct OamEntry {
    u32 attributes;
    u16 tile, depth;
};
struct Matrix {
    u8 angle;
    s8 scaleX, scaleY;
    u8 pad;
};
extern struct OamEntry gUnk_03004390[], gUnk_030047A0[];
extern struct Matrix gUnk_03004310[];
extern s16 gUnk_086149C4[];
extern u32 gUnk_03003A04;
void CpuFastSet(const void *, void *, u32);
s32 Div(s32, s32);
void SpriteOam_FinalizeQueue(void)
{
    struct {
        u8 heads[256];
        s32 fill;
    } work;
    s32 priorities[4];
    struct OamEntry *entry;
    s32 i;
    s32 count;
    /* Reused for the OAM buffer and then the affine lookup table. */
    void *base;
    work.fill = -1;
    CpuFastSet(&work.fill, work.heads, 0x1000040);
    {
        entry = gUnk_030047A0;
        for (i = 0; i < gPendingOamCount; i++) {
            u32 depth = entry->depth;
            u8 *head = &work.heads[depth >> 8];
            entry->depth = depth | (*head);
            *head = i;
            entry++;
        }
    }
    {
        s32 sum;
        sum = gOamCount;
        for (i = 0; i < 4; i++) {
            base = gUnk_03004390;
            priorities[i] = sum;
            sum += gOamPriorityCounts[i];
        }
    }
    count = gOamCount;
    for (i = 255; (i >= 0) && (count < 128); i--) {
        s32 index = work.heads[i];
        if (index != 255) {
            do {
                s32 *next;
                struct OamEntry *queued;
                queued = &gUnk_030047A0[index];
                next = &priorities[(queued->tile >> 10) & 3];
                if ((*next) < 128) {
                    gUnk_03004390[*next] = *queued;
                    count++;
                    (*next)++;
                }
                index = (u8)queued->depth;
            } while (index != 255);
        }
    }

    i = count;
    entry = (struct OamEntry *)(i * sizeof(*entry) + (u32)base);
    for (; i < 128; i++, entry++) {
        entry->attributes = 0x200;
    }

    {
        struct Matrix *matrix;
        entry = base;
        matrix = gUnk_03004310;
        if (gAffineRequestCount > 31)
            gAffineRequestCount = 32;
        for (i = 0; i < gAffineRequestCount; i++) {
            s32 angle;
            s32 sine, cosine, value;
            base = gUnk_086149C4;
            angle = matrix->angle;
            sine = ((s16 *)base)[angle] * 4;
            cosine = ((s16 *)base)[(angle + 64) & 255] * 4;
            if (!matrix->scaleX) {
                value = matrix->scaleX;
                entry[0].depth = value;
            } else {
                entry[0].depth = Div(cosine, matrix->scaleX);
                value = Div(-sine, matrix->scaleX);
            }
            entry[1].depth = value;
            if (!matrix->scaleY) {
                value = matrix->scaleY;
                entry[2].depth = value;
            } else {
                entry[2].depth = Div(sine, matrix->scaleY);
                value = Div(cosine, matrix->scaleY);
            }
            entry[3].depth = value;
            entry += 4;
            matrix++;
        }
    }
    gUnk_03003A04 |= 1;
}
