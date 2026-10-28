// MATCHING_CONCESSION: register-pinning (register pin and empty asm operand hint)
#include "global.h"
#include "ram.h"
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
void FloorSwitch_Release(struct FloorSwitch *p)
{
    u16 *flags = &p->flags88;
    u32 f = *flags;
    if (2 & f) {
        u32 one;
        u32 zero;
        u32 v;
        u32 mask;
        one = 1;
        zero = 0;
        v = one;
        v = v | f;
        mask = 0xFFFD;
        do {
            *flags = v & mask;
            FloorSwitch_RunStateScript(p);
            p->field10 = zero;
            AnimData_SelectPaletteBank(p->anim, 0x16C);
        } while (0);
        if (!(gEngineFlags & 0x800)) {
            if (*flags & 4)
                Sound_PlaySong(0x389);
            else
                Sound_PlaySong(0x155);
        }
        p->field5A = 0;
    }
}
