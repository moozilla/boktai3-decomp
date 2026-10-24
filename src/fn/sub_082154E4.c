#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: priority-loop allocation and invariant address hoisting.
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
extern s32 gUnk_0300479C, gUnk_03004798, gUnk_03004BA0;
extern u16 gUnk_03004790[];
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
    register s32 count asm("r5");
    register struct OamEntry *buffer asm("r9");
    work.fill = -1;
    CpuFastSet(&work.fill, work.heads, 0x1000040);
    count = gUnk_03004798;
    {
        register struct OamEntry *entry asm("r5") = gUnk_030047A0;
        register s32 i asm("r4");
        for (i = 0; i < gUnk_0300479C; i++) {
            u32 depth = entry->depth;
            u8 *head = &work.heads[depth >> 8];
            entry->depth = depth | (*head);
            *head = i;
            entry++;
        }
    }
    {
        s32 sum;
        sum = gUnk_03004798;
        buffer = gUnk_03004390;
        for (i = 0; i < 4; i++) {
            priorities[i] = sum;
            sum += gUnk_03004790[i];
        }
    }
    for (i = 255; (i >= 0) && (count < 128); i--) {
        s32 index = work.heads[i];
        if (index != 255) {
            do {
                s32 *next;
                entry = &gUnk_030047A0[index];
                next = &priorities[(entry->tile >> 10) & 3];
                if ((*next) < 128) {
                    gUnk_03004390[*next] = *entry;
                    count++;
                    (*next)++;
                }
                index = (u8)entry->depth;
            } while (index != 255);
        }
    }

    i = count;
    entry = buffer + i;
    for (; i < 128; i++, entry++) {
        entry->attributes = 0x200;
    }

    entry = buffer;
    {
        register struct Matrix *matrix asm("r6") = gUnk_03004310;
        register struct OamEntry *entry asm("r5") = buffer;
        register s32 i asm("r4");
        if (gUnk_03004BA0 > 31)
            gUnk_03004BA0 = 32;
        for (i = 0; i < gUnk_03004BA0; i++) {
            s32 angle = matrix->angle;
            s32 sine = gUnk_086149C4[angle] * 4;
            s32 cosine = gUnk_086149C4[(angle + 64) & 255] * 4;
            s32 value;
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

#else
INCLUDE_ASM("asm/nonmatching", SpriteOam_FinalizeQueue);
#endif
