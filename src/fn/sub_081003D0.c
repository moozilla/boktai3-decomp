#define FloorSwitch_RunStateScript sub_081003D0
#include "global.h"
struct FloorSwitch {
    u32 flags;
    u8 pad4[8];
    void *anim;
    u16 field10;
    u8 pad12[0xA];
    s16 x, y, z;
    u8 pad22[0xA];
    u8 animData[0x1C];
    s32 name;
    s32 r[2];
    s32 region;
    u16 sprite, field5A;
    u8 pad5C[8];
    s32 s[4];
    s32 w[4];
    u8 pad84[2];
    u16 field86;
    u16 flags88;
    u8 field8A, field8B;
    u16 field8C, field8E;
};
struct FloorSwitchScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
void Script_Call(u32, struct FloorSwitchScriptArgs *);
static inline void fill(struct FloorSwitch *p, s32 *values)
{
    values[0] = p->name;
    values[1] = p->s[0];
    values[2] = p->s[1];
    values[3] = p->s[2];
    values[4] = p->s[3];
    values[5] = p->w[0];
    values[6] = p->w[1];
    values[7] = p->w[2];
    values[8] = p->w[3];
    values[9] = p->field5A;
}
void sub_081003D0(struct FloorSwitch *p)
{
    s32 values[10];
    struct FloorSwitchScriptArgs args;
    if (p->flags88 & 2) {
        if (p->r[0]) {
            fill(p, values);
            args.count = 10;
            args.values = values;
            Script_Call(p->r[0], &args);
        }
    } else if (p->flags88 & 1) {
        if (p->r[1]) {
            fill(p, values);
            args.count = 10;
            args.values = values;
            Script_Call(p->r[1], &args);
        }
    }
}
