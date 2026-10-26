#include "global.h"
#include "color_fade_controller.h"
#include "render.h"
#include "ram.h"

void ColorFade_StepFadeOut(struct ColorFadeChannel *p)
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

void ColorFade_StepFadeIn(struct ColorFadeChannel *p)
{
    p->level = (p->time << 6) >> p->shift;
    if (p->time > (1 << p->shift)) {
        p->level = 0x40;
        p->mode = 0;
    }
    p->time++;
}

void ColorFade_InitChannel(struct ColorFadeChannel *p)
{
    p->mode = 0;
    p->color = 0x1084;
    p->start = 0;
    p->level = 0x40;
    p->target = 0;
    p->time = 0;
    p->shift = 0;
}

void ColorFade_ClearMode(struct ColorFadeChannel *p)
{
    p->mode = 0;
}

void ColorFade_StepFadeIn(struct ColorFadeChannel *);

void ColorFade_StepFadeOut(struct ColorFadeChannel *);

void ColorFade_UpdateChannel(struct ColorFadeChannel *p)
{
    switch (p->mode) {
        case 1:
        case 3:
            ColorFade_StepFadeIn(p);
            break;
        case 2:
        case 4:
            ColorFade_StepFadeOut(p);
            break;
    }
}

void ColorFade_ClearMode(struct ColorFadeChannel *);

void ColorFade_SetImmediate(u32 a, u32 b, u32 c)
{
    struct ColorFadeController *s = gColorFadeController;
    if (s != 0) {
        gBgFadeLevel = a;
        gBgFadeBankMask = b;
        gBgFadeColor = c;
        gObjFadeLevel = a;
        gObjFadeColor = c;
        gObjPaletteEffect = 0;
        ColorFade_ClearMode(&s->channels[0]);
        ColorFade_ClearMode(&s->channels[1]);
    }
}

void ColorFade_PublishOutputs(struct ColorFadeController *s)
{
    struct ColorFadeChannel *p = &s->channels[0];
    if (p->mode != 0) {
        gBgFadeLevel = p->level;
        gBgFadeBankMask = p->start;
        gBgFadeColor = p->color;
    }
    p = &s->channels[1];
    if (p->mode != 0) {
        gObjFadeLevel = p->level;
        gObjFadeFlags = p->target;
        gObjFadeColor = p->color;
    }
}

s32 ColorFade_SetMode(struct ColorFadeChannel *c, s32 x)
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

s32 ColorFade_SetMode(struct ColorFadeChannel *, s32);

void ColorFade_ApplyCommand(struct ColorFadeController *p, s32 type, s32 *args)
{
    u32 flags = args[0];
    s32 shift = args[1];
    u32 color = args[2] | (args[3] << 5) | (args[4] << 10);
    s32 start = args[5], target = args[6];
    if (flags & 1) {
        struct ColorFadeChannel *c = &p->channels[0];
        if (ColorFade_SetMode(c, type) >= 0) {
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
        if (ColorFade_SetMode(c, type) >= 0) {
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
                ColorFade_ClearMode(&p->channels[0]);
                break;
            case 7:
                ColorFade_ClearMode(&p->channels[1]);
                break;
        }
    }
    p->count = 0;
}

void ColorFade_ApplyCommands(struct ColorFadeController *);

void ColorFade_UpdateChannel(struct ColorFadeChannel *);

void ColorFade_PublishOutputs(struct ColorFadeController *);

u32 ColorFade_OnUpdate(struct ColorFadeController *p)
{
    ColorFade_ApplyCommands(p);
    ColorFade_UpdateChannel(&p->channels[0]);
    ColorFade_UpdateChannel(&p->channels[1]);
    ColorFade_PublishOutputs(p);
    return 0;
}

void ColorFade_OnDestroy(void)
{
    gColorFadeController = 0;
}

void ColorFade_InitChannel(struct ColorFadeChannel *);

s32 ColorFade_Init(struct ColorFadeController *s)
{
    gColorFadeController = s;
    ColorFade_InitChannel(&s->channels[0]);
    ColorFade_InitChannel(&s->channels[1]);
    gBgFadeLevel = 0x40;
    gBgFadeApplied = 0x40;
    gBgFadeColor = 0x1084;
    gBgFadeBankMask = 0;
    gObjFadeLevel = 0x40;
    gObjFadeApplied = 0x40;
    gObjFadeColor = 0x1084;
    gObjFadeFlags = 0;
    return gObjPaletteEffect = 0;
}

void *ColorFade_Create(void)
{
    void *p = Task_Create(0xb, 0x15c);
    if (p != 0) {
        Task_SetCallbacks(p, ColorFade_OnUpdate, ColorFade_OnDestroy);
        if (ColorFade_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 ColorFade_QueueCommand(u32 id, s32 cnt, u32 *src)
{
    struct ColorFadeController *s = gColorFadeController;
    s32 i;
    if (s == 0 || s->count > 7)
        return -1;
    s->types[s->count] = id;
    for (i = 0; i < cnt; i++)
        s->args[s->count][i] = *src++;
    s->count++;
    return 0;
}

s32 ColorFade_QueueCommand(u32, s32, u32 *);

u32 ColorFade_FromScript(void)
{
    u32 buf[8];
    u32 a = Script_GetKeywordValue(0x72, 0);
    s32 n = 0;
    if (Script_SeekToKeyword(0x70)) {
        u32 *q = buf;
        while (Script_GetPc() != 0 && n <= 7) {
            *q++ = Script_GetValue();
            n++;
        }
    }
    return ColorFade_QueueCommand(a, n, buf);
}
