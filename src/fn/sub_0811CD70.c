#include "global.h"

struct CharModel {
    u32 flags;
    u8 pad04[8];
    u32 heldFlags;
    u8 pad10[0x28];
    s32 f38;
    u8 pad3C[0x48];
    s32 f84;
    s32 f88;
    u8 pad8C[0x20];
    u8 fAC;
    u8 padAD[0x28F];
    s32 f33C;
    u8 pad340[0x1E4];
    u16 f524;
    u8 pad526[0x86];
    s32 f5AC;
    s32 f5B0;
    s32 f5B4;
    u8 pad5B8[0x24];
    s32 f5DC;
    u8 pad5E0[0xC];
    s8 f5EC;
};

void Modifier_StartDrainIfAboveThreshold(struct CharModel *m);
void Sound_PlaySong(u32 song);
s32 sub_0811E8B0(struct CharModel *m, s32 arg);

s32 sub_0811CD70(struct CharModel *m, u32 input)
{
    s32 ret = 0;
    s32 slow;

    m->f524 = 0x88;
    if (m->f5EC > 0)
        return 0;
    if (m->f5B0 >= 0 && (m->f38 >> 12) == 0)
        m->fAC = 0;
    else
        m->fAC = 1;
    if (m->fAC)
        return ret;
    if (input & 0x200)
        Modifier_StartDrainIfAboveThreshold(m);
    if ((m->heldFlags & 0x200) && m->f33C != 2) {
        if (!(m->flags & 0x200))
            Sound_PlaySong(0x8E);
        return ret;
    }
    if ((m->heldFlags & 0x400) && m->f33C != 1) {
        if (!(m->flags & 0x400))
            Sound_PlaySong(0x8F);
        slow = 1;
        m->f5AC = (s32)(m->f5B4 << 3) >> 4;
        m->f5B4 = (s32)(m->f5B4 << 3) >> 4;
    } else {
        slow = 0;
    }
    if (input & 0x100) {
        if (!slow) {
            m->f5AC += sub_0811E8B0(m, m->f84);
            ret = 1;
        }
        m->f524 = 0x89;
    }
    if (input & 0x40)
        m->f5B4 -= m->f88 >> slow;
    else if (input & 0x80)
        m->f5B4 += m->f88 >> slow;
    if (slow) {
        if (input & 0x10)
            m->f5DC += 0x400;
        else if (input & 0x20)
            m->f5DC -= 0x400;
    } else {
        if (input & 0x10)
            m->f5DC += 0x1000;
        else if (input & 0x20)
            m->f5DC -= 0x1000;
    }
    return ret;
}
