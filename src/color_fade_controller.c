#include "global.h"
#include "color_fade_controller.h"

void sub_080036F8(struct ColorFadeChannel *p)
{
    p->level = 0x40 - ((p->time << 6) >> p->shift);
    if (p->time > (1 << p->shift)) {
        p->level = 0;
        if (p->mode == 2)
            p->mode = 0;
        else
            p->mode = 5;
    }
    p->time++;
}

void sub_08003734(struct ColorFadeChannel *p)
{
    p->level = (p->time << 6) >> p->shift;
    if (p->time > (1 << p->shift)) {
        p->level = 0x40;
        p->mode = 0;
    }
    p->time++;
}

void sub_08003760(struct ColorFadeChannel *p)
{
    p->mode = 0;
    p->color = 0x1084;
    p->start = 0;
    p->level = 0x40;
    p->target = 0;
    p->time = 0;
    p->shift = 0;
}

void sub_0800377C(struct ColorFadeChannel *p)
{
    p->mode = 0;
}

void sub_08003734(struct ColorFadeChannel *);

void sub_080036F8(struct ColorFadeChannel *);

void sub_08003784(struct ColorFadeChannel *p)
{
    switch (p->mode) {
        case 1:
        case 3:
            sub_08003734(p);
            break;
        case 2:
        case 4:
            sub_080036F8(p);
            break;
    }
}

extern struct ColorFadeController *gUnk_02000020;

extern u32 gUnk_03004DA0;

extern u16 gUnk_030051C4;

extern u16 gUnk_03005214;

extern u32 gUnk_030051D4;

extern u16 gUnk_030051DC;

extern u32 gUnk_030051C8;

void sub_0800377C(struct ColorFadeChannel *);

void sub_080037B0(u32 a, u32 b, u32 c)
{
    struct ColorFadeController *s = gUnk_02000020;
    if (s != 0) {
        gUnk_03004DA0 = a;
        gUnk_030051C4 = b;
        gUnk_03005214 = c;
        gUnk_030051D4 = a;
        gUnk_030051DC = c;
        gUnk_030051C8 = 0;
        sub_0800377C(&s->channels[0]);
        sub_0800377C(&s->channels[1]);
    }
}

extern u16 gUnk_03005210;

void sub_08003808(struct ColorFadeController *s)
{
    struct ColorFadeChannel *p = &s->channels[0];
    if (p->mode != 0) {
        gUnk_03004DA0 = p->level;
        gUnk_030051C4 = p->start;
        gUnk_03005214 = p->color;
    }
    p = &s->channels[1];
    if (p->mode != 0) {
        gUnk_030051D4 = p->level;
        gUnk_03005210 = p->target;
        gUnk_030051DC = p->color;
    }
}

s32 sub_08003860(struct ColorFadeChannel *c, s32 x)
{
    u32 *p = &c->mode;
    switch (x) {
        case 1:
            if (*p > 2)
                return -1;
            *p = 1;
            break;
        case 2:
            if (*p > 2)
                return -1;
            *p = 2;
            break;
        case 3:
            *p = 3;
            break;
        case 4:
        case 5:
            *p = 4;
            break;
    }
    return 0;
}

s32 sub_08003860(struct ColorFadeChannel *, s32);

void ColorFade_ApplyCommand(struct ColorFadeController *p, s32 type, s32 *args)
{
    u32 flags = args[0];
    s32 shift = args[1];
    u32 color = args[2] | (args[3] << 5) | (args[4] << 10);
    s32 start = args[5], target = args[6];
    if (flags & 1) {
        struct ColorFadeChannel *c = &p->channels[0];
        if (sub_08003860(c, type) >= 0) {
            if (type == 5) {
                c->time = 1;
                c->shift = 0;
            } else {
                c->time = 0;
                c->shift = shift;
            }
            c->color = color;
            c->start = start;
        }
    }
    if (flags & 2) {
        struct ColorFadeChannel *c = &p->channels[1];
        if (sub_08003860(c, type) >= 0) {
            if (type == 5) {
                c->time = 1;
                c->shift = 0;
            } else {
                c->time = 0;
                c->shift = shift;
            }
            c->color = color;
            c->target = target;
        }
    }
}

void ColorFade_ApplyCommand(struct ColorFadeController *, s32, s32 *);

void ColorFade_ApplyCommands(struct ColorFadeController *p)
{
    u32 i = 0;
    for (; i < p->count; i++) {
        s32 *args = p->args[i];
        s32 type = p->types[i];
        switch (type) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                ColorFade_ApplyCommand(p, type, args);
                break;
            case 6:
                sub_0800377C(&p->channels[0]);
                break;
            case 7:
                sub_0800377C(&p->channels[1]);
                break;
        }
    }
    p->count = 0;
}

void ColorFade_ApplyCommands(struct ColorFadeController *);

void sub_08003784(struct ColorFadeChannel *);

void sub_08003808(struct ColorFadeController *);

u32 sub_080039BC(struct ColorFadeController *p)
{
    ColorFade_ApplyCommands(p);
    sub_08003784(&p->channels[0]);
    sub_08003784(&p->channels[1]);
    sub_08003808(p);
    return 0;
}

void sub_080039E4(void)
{
    gUnk_02000020 = 0;
}

extern u32 gUnk_03004DA8;

extern u32 gUnk_03004DAC;

void sub_08003760(struct ColorFadeChannel *);

s32 sub_080039F0(struct ColorFadeController *s)
{
    gUnk_02000020 = s;
    sub_08003760(&s->channels[0]);
    sub_08003760(&s->channels[1]);
    gUnk_03004DA0 = 0x40;
    gUnk_03004DA8 = 0x40;
    gUnk_03005214 = 0x1084;
    gUnk_030051C4 = 0;
    gUnk_030051D4 = 0x40;
    gUnk_03004DAC = 0x40;
    gUnk_030051DC = 0x1084;
    gUnk_03005210 = 0;
    return gUnk_030051C8 = 0;
}

void *sub_08003A68(void)
{
    void *p = sub_08219FBC(0xb, 0x15c);
    if (p != 0) {
        sub_0821A04C(p, sub_080039BC, sub_080039E4);
        if (sub_080039F0(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}

s32 sub_08003AA8(u32 id, s32 cnt, u32 *src)
{
    struct ColorFadeController *s = gUnk_02000020;
    s32 i;
    if (s == 0 || s->count > 7)
        return -1;
    s->types[s->count] = id;
    for (i = 0; i < cnt; i++)
        s->args[s->count][i] = *src++;
    s->count++;
    return 0;
}

s32 sub_08003AA8(u32, s32, u32 *);

u32 sub_08003B00(void)
{
    u32 buf[8];
    u32 a = sub_0821ABA8(0x72, 0);
    s32 n = 0;
    if (Script_SeekToKeyword(0x70)) {
        u32 *q = buf;
        while (Script_GetPc() != 0 && n <= 7) {
            *q++ = Script_GetValue();
            n++;
        }
    }
    return sub_08003AA8(a, n, buf);
}
