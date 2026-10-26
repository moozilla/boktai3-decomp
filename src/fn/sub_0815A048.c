// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
extern u32 gUnk_0200054C;
void Player_AddControlFlags(void *, u32);
void Sound_PlaySong(u32);
// confidence=low: forced variant clears movement state before starting or refreshing stun.
#define Player_TryStartStun sub_0815A048
s32 Player_TryStartStun(u8 *p, s32 duration, s32 force)
{
    if (force == 1) {
        s32 guard = *(u16 *)(p + 0x274);
        if (guard > 0)
            return 0;
        Player_AddControlFlags(p, 8);
        {
            u16 *flags = (u16 *)(p + 0x236);
            register u32 old asm("r3") = *flags;
            register u32 bits asm("r0") = 4;
            u32 zeroByte;
            register u32 zeroHalf asm("r1");
            zeroByte = 0;
            zeroHalf = 0;
            *flags = bits | old;
            *(u16 *)(p + 0x270) = zeroHalf;
            *(u8 *)(p + 0x4BA) = zeroByte;
            *(u16 *)(p + 0x4D8) = zeroHalf;
            *(u16 *)(p + 0x4DA) = zeroHalf;
            *(u8 *)(p + 0x4BE) = zeroByte;
            *(u16 *)(p + 0x442) = zeroHalf;
            gUnk_0200054C = zeroHalf;
            gPlayerNotificationPending = zeroHalf;
        }
    }
    {
        u32 kind = *(s32 *)(p + 0x1C);
        if (kind == 1 && *(u8 *)(p + 0x457) != 0x1A) {
            if (!*(u16 *)(p + 0x4D8)) {
                *(u16 *)(p + 0x4D8) = duration;
                *(u16 *)(p + 0x4D6) = kind;
                *(u16 *)(p + 0x4DA) = kind;
                Sound_PlaySong(0xE6);
                return 1;
            }
            *(u16 *)(p + 0x4D6) = kind;
        }
    }
    return 0;
}
