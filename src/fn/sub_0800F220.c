#include "global.h"
typedef struct Screen Screen;
struct Screen {
    u8 task[24];
    u16 id;
    u8 flags, bg;
    u16 mapId, paletteId;
    u16 *buffer, *source;
    u16 dirty, delay;
    u16 rows[2][160], active;
    s16 amplitude, delta;
    u16 top, bottom;
    u16 times[6], start, end, blend, fade, fadeStep;
    void (*state)(Screen *);
    void (*effect)(Screen *);
    void (*noise)(Screen *);
    u16 timer, pad;
    u16 argc, pad2;
    u32 *argv, values[8], script;
};
extern Screen *gUnk_02000044;
extern u16 gUnk_03004BD8, gUnk_03005160[];
extern const u16 gUnk_08604B3C[];
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int Script_GetPc(void);
u8 *sub_0821A520(int, int);
void sub_08215A74(int, int);
void *Video_GetBackgroundMap(int);
void sub_082160B8(int, int, void *, int, int, int, int, void *);
void StatusScreen_WaitForStart(Screen *);
int StatusScreen_Initialize(Screen *p, u16 id)
{
    int i;
    u8 *map, *palette;
    gUnk_02000044 = p;
    {
        u16 *flags = &gUnk_03004BD8;
        {
            int mask = ~256;
            mask &= *flags;
            *flags = mask;
        }
    }
    p->id = id;
    p->flags = sub_0821ABA8('f', 0);
    p->mapId = 0x5AF3;
    p->paletteId = sub_0821ABA8('p', 0);
    p->buffer = gUnk_03005160;
    p->script = sub_0821ABA8('e', 0);
    if (Script_SeekToKeyword('a')) {
        p->argv = p->values;
        for (i = 0; i < 8 && Script_GetPc(); i++)
            p->argv[i] = Script_GetValue();
        p->argc = i;
    } else
        p->argc = 0;
    if (Script_SeekToKeyword('t')) {
        for (i = 0; i < 6 && Script_GetPc(); i++) {
            int v = Script_GetValue();
            p->times[i] = v;
            if ((u16)v < gUnk_08604B3C[i])
                p->times[i] = gUnk_08604B3C[i];
        }
        for (; i < 6; i++)
            p->times[i] = gUnk_08604B3C[i];
    } else
        for (i = 0; i < 6; i++)
            p->times[i] = gUnk_08604B3C[i];
    p->bg = 0;
    map = sub_0821A520(0xC091, p->mapId);
    if (!map)
        return -1;
    palette = sub_0821A520(0x92B3, p->paletteId);
    if (!palette)
        return -1;
    if (map[0] == 'M' && map[1] == 'P')
        sub_08215A74(p->bg, 0);
    else {
        void *dst = Video_GetBackgroundMap(0);
        sub_082160B8(p->bg, 0, map, 0, 0, 0, 0, dst);
    }
    p->source = (u16 *)(palette + 0x1B4);
    {
        u32 zero = 0;
        CpuSet(&zero, p->buffer, 0x05000018);
    }
    *(volatile u16 *)0x04000008 |= 0x40;
    p->delay = sub_0821ABA8('w', 0);
    {
        register void (*state)(Screen *) asm("r1") = StatusScreen_WaitForStart;
        p->state = state;
    }
    p->timer = 0;
    p->start = 0;
    p->end = 16;
    p->blend = 0x1084;
    p->fade = 0;
    p->fadeStep = 1;
    p->effect = 0;
    {
        u32 zero = 0;
        CpuSet(&zero, p->rows, 0x05000091);
    }
    p->active = 0;
    p->amplitude = 1024;
    p->delta = 1;
    p->top = 0;
    p->bottom = 160;
    p->noise = 0;
    p->dirty = 15;
    return 0;
}
