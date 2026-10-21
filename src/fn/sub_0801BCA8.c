#include "global.h"
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void *AnimData_FindEntry(int);
void ParticleSprite_Initialize(void *, void *, int);
static inline void CpuSet_2(u32 control, void *dest, const void *src)
{
    CpuSet(src, dest, control);
}
int ResultMenu_CreateGraphics(u8 *arg)
{
    u8 *p = arg;
    struct {
        u32 zero;
        u32 position[2];
    } tmp;
    u32 *pos = tmp.position;
    u8 *out;
    int i;
    u32 *saved;
    int clear;
    tmp.zero = 0;
    {
        u32 *src = &tmp.zero;
        u32 *dst = pos;
        CpuSet_2(0x05000002, dst, src);
    }
    saved = pos;
    clear = 0;
    out = p + 0x144;
    i = 14;
    do {
        Widget_InitLinked(out, p + 0x104, 0xb9, 0x11, clear, clear, 60, saved);
        out += 0x60;
        i--;
    } while (i >= 0);
    {
        void *resource = AnimData_FindEntry(0x1c1a);
        u8 *a = p + 0xb8;
        u8 *b = p + 0xb9;
        out = p + 0x6e4;
        i = 3;
        do {
            ParticleSprite_Initialize(out, resource, 0x11);
            out += 0x28;
            i--;
        } while (i >= 0);
        {
            int zero = 0;
            *a = zero;
            *b = zero;
        }
    }
}
