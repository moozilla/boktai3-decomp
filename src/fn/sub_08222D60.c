// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern s32 gUnk_03005324, gUnk_0300531C;
extern u32 gUnk_0300172C, gUnk_03001730, gUnk_03001734;
extern u16 gUnk_0300172A, gUnk_03001728, gUnk_03005330, gUnk_03005380;
extern u16 gUnk_03005370[], gUnk_03005340[], gUnk_03005350[];
#define IO(a) (*(volatile u16 *)(a))
static inline void CopyQueued(u16 *out)
{
    u16 *q = gUnk_03005350;
    out[0] = q[gUnk_03005380 * 4];
    out[1] = *(gUnk_03005380 * 4 + &gUnk_03005350[1]);
    out[2] = *(gUnk_03005380 * 4 + &gUnk_03005350[2]);
    out[3] = *(gUnk_03005380 * 4 + &gUnk_03005350[3]);
}
static inline void QueuePacket(u16 *in, u32 pos)
{
    u16 *q = gUnk_03005350;
    q[pos * 4] = in[0];
    *(pos * 4 + &gUnk_03005350[1]) = in[1];
    *(pos * 4 + &gUnk_03005350[2]) = in[2];
    *(pos * 4 + &gUnk_03005350[3]) = in[3];
}
static inline void CopyPacket(u16 *out, u16 *in)
{
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
}
static inline void AdvanceSend(void)
{
    if (gUnk_0300172A & 0x4000) {
        gUnk_0300172A = gUnk_03001728;
        gUnk_03001728 = 0x4000;
    }
}
#ifdef NONMATCHING
// fuzzy: 86.5%
void Serial_ServiceInterrupt(void)
{
    u32 control;
    IO(0x04000120);
    gUnk_03005324 = 0;
    control = IO(0x04000128);
    if ((control & 0x40) || (control & 0xb0) == 0x80)
        gUnk_03005324 = -2;
    else {
        u32 bits = 0;
        u16 *receive = &gUnk_03005370[(1 - gUnk_0300172C) * 4];
        u16 *latest = &gUnk_03005340[(1 - gUnk_03001734) * 4];
        u32 word = IO(0x04000120);
        register u32 first asm("r8") = word;
        latest[0] = word;
        if (!(word & 0x8000))
            bits = word;
        word = IO(0x04000122);
        latest[1] = word;
        if (!(word & 0x8000))
            bits |= word;
        word = IO(0x04000124);
        latest[2] = word;
        if (!(word & 0x8000))
            bits |= word;
        word = IO(0x04000126);
        latest[3] = word;
        if (!(word & 0x8000))
            bits |= word;
        if (!(bits & 0x4000)) {
            if (gUnk_03001730) {
                u32 pos = (gUnk_03005380 + gUnk_03005330) & 3;
                gUnk_03005350[pos * 4] = first;
                *(pos * 4 + &gUnk_03005350[1]) = latest[1];
                *(pos * 4 + &gUnk_03005350[2]) = latest[2];
                *(pos * 4 + &gUnk_03005350[3]) = latest[3];
                ++gUnk_03005330;
                gUnk_0300172A = 0x4000;
            } else {
                if (gUnk_03005330) {
                    volatile u16 count;
                    CopyQueued(receive);
                    gUnk_03005380 = (gUnk_03005380 + 1) & 3;
                    count = gUnk_03005330;
                    --gUnk_03005330;
                    QueuePacket(latest, (gUnk_03005380 + gUnk_03005330) & 3);
                    gUnk_03005330 = count;
                } else {
                    receive[0] = first;
                    receive[1] = latest[1];
                    receive[2] = latest[2];
                    receive[3] = latest[3];
                }
                gUnk_0300172A = gUnk_03001728;
                gUnk_03001728 = 0x4000;
                gUnk_0300172C = 1 - gUnk_0300172C;
                gUnk_03001730 = 1;
            }
        } else {
            if (!gUnk_03001730 && gUnk_03005330) {
                CopyQueued(receive);
                gUnk_03005380 = (gUnk_03005380 + 1) & 3;
                --gUnk_03005330;
                AdvanceSend();
                gUnk_0300172C = 1 - gUnk_0300172C;
                gUnk_03001730 = 1;
            } else
                AdvanceSend();
        }
        IO(0x0400012A) = gUnk_0300172A;
        gUnk_0300531C = (IO(0x04000128) >> 4) & 3;
        gUnk_03001734 = 1 - gUnk_03001734;
    }
    IO(0x03007FF8) = 0x80;
}

#else
INCLUDE_ASM("asm/nonmatching", Serial_ServiceInterrupt);
#endif
