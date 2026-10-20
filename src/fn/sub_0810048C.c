// MATCHING_CONCESSION: register-pinning (empty asm operand hints)
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
void FloorSwitch_RunStateScript(struct FloorSwitch *);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void FloorSwitch_SetOccupant(struct FloorSwitch *p, u32 occupant)
{
    u16 *flags;
    u32 f;
    u32 one;
    u32 mask;
    p->field5A = occupant;
    flags = &p->flags88;
    f = *flags;
    one = 1;
    asm("" : "+r"(one));
    if (one & f) {
        u32 set = 2;
        u32 zero;
        u32 v;
        v = set | f;
        zero = 0;
        asm("" : "+r"(zero));
        v = v | zero;
        mask = 0xFFFE;
        *flags = v & mask;
        p->field8C = 0;
        FloorSwitch_RunStateScript(p);
        p->field10 = one;
        AnimData_SelectPaletteBank(p->anim, 0x16D);
        if (*flags & 4)
            Sound_PlaySong(0x389);
        else
            Sound_PlaySong(0x155);
    }
}
