#include "global.h"

#define SceneFade_BeginSoundFadeFromScript sub_081C4EF0

union ScriptPair16 {
    u16 h[2];
    u32 w;
};

struct SceneFade {
    u8 pad18[0x18];
    u32 f18;
    union ScriptPair16 g1w0;
    union ScriptPair16 g1w1;
    union ScriptPair16 g2w0;
    union ScriptPair16 g2w1;
    u32 f2c;
    void (*callback)(void);
    u16 timer;
    u16 f36;
    u16 f38;
    u16 f3a;
    u16 f3c;
};

void SceneFade_BeginSoundFade(struct SceneFade *p);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
u32 sub_081C3FBC(s32, s32);

void SceneFade_BeginSoundFadeFromScript(struct SceneFade *p)
{
    struct {
        union ScriptPair16 w0;
        union ScriptPair16 w1;
        union ScriptPair16 w2;
        union ScriptPair16 w3;
    } buf;
    s32 w, e;

    if (Script_SeekToKeyword(0x73)) {
        {
            u16 v = Script_GetValue();
            buf.w0.w = (buf.w0.w & 0xFFFF0000) | v;
        }
        {
            u32 v = Script_GetValue() << 16;
            buf.w0.w = (buf.w0.w & 0x0000FFFF) | v;
        }
        {
            u16 v = Script_GetValue();
            buf.w1.w = (buf.w1.w & 0xFFFF0000) | v;
        }
    } else {
        u16 *q = buf.w0.h;
        buf.w0.h[0] = 0x880;
        q[1] = 0x200;
        q[2] = 0x868;
    }

    if (Script_SeekToKeyword(0x67)) {
        {
            u16 v = Script_GetValue();
            buf.w2.w = (buf.w2.w & 0xFFFF0000) | v;
        }
        {
            u32 v = Script_GetValue() << 16;
            buf.w2.w = (buf.w2.w & 0x0000FFFF) | v;
        }
        {
            u16 v = Script_GetValue();
            u32 *q2 = (u32 *)((u16 *)&buf.w2 + 2);
            *q2 = (*q2 & 0xFFFF0000) | v;
        }
    } else {
        u16 *q2 = buf.w2.h;
        q2[0] = 0x580;
        q2[1] = 0x200;
        q2[2] = 0x780;
    }

    w = Script_SeekToKeyword(0x77);
    if (w)
        w = Script_GetValue();
    p->f36 = w;

    e = Script_SeekToKeyword(0x65);
    if (e)
        e = Script_GetValue();
    p->f2c = e;

    p->f18 = sub_081C3FBC(3, 1);
    p->g1w0 = buf.w0;
    p->g1w1 = buf.w1;
    p->g2w0 = buf.w2;
    p->g2w1 = buf.w3;
    p->f38 = 0x80;
    p->f3a = 0x80;
    p->f3c = 0x80;
    SceneFade_BeginSoundFade(p);
}
