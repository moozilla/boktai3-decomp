#include "global.h"
#define ScriptEffect_Initialize sub_08163700
#define gScriptEffect           gUnk_02000210
struct ScriptEffect {
    u8 unknown0[0x338];
    u32 resource;
    u8 unknown33C[4];
    u8 phase, state, mode;
    u8 unknown343[3];
    u16 timer, duration, interval;
    u8 active, initialized;
    u8 unknown34E[2];
    s32 parameter;
};
void sub_08163144(struct ScriptEffect *);
void sub_081631F8(struct ScriptEffect *);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 sub_08227E90(void);
extern struct ScriptEffect *gScriptEffect;
s32 sub_08163700(struct ScriptEffect *p)
{
    sub_08163144(p);
    sub_081631F8(p);
    p->phase = 0;
    p->state = 0;
    p->timer = 0;
    p->mode = 0;
    if (Script_SeekToKeyword('r'))
        p->resource = sub_08227E90();
    if (Script_SeekToKeyword('c')) {
        p->duration = Script_GetValue();
        p->interval = Script_GetValue();
    } else {
        p->duration = 500;
        p->interval = 250;
    }
    p->active = 0;
    if (Script_SeekToKeyword('p'))
        p->parameter = Script_GetValue();
    else
        p->parameter = 0;
    gScriptEffect = p;
    p->initialized = 0;
}
