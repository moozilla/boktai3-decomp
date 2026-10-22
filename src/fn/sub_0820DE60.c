#include "global.h"
struct Pos {
    s16 x, y, z, pad;
};
struct Pos *Player_GetEffectivePosition(u32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void PoisonHand_UpdateEmergence(void);
u32 PoisonHand_WaitForPlayerApproach(u8 *p)
{
    struct Pos *pos = Player_GetEffectivePosition(0);
    struct Pos *origin = (struct Pos *)(p + 0x40);
    s32 x = pos->x - origin->x;
    s32 z = pos->z - origin->z;
    s32 xx = x * x, zz = z * z;
    if (xx + zz < *(s32 *)(p + 0x11c)) {
        *(void **)(p + 0x108) = PoisonHand_UpdateEmergence;
        Anim_RestartSequence(p + 0x10c, *(void **)(p + 0x168), 24, 0, 8);
        *(u16 *)(p + 0x128) = 180;
        Sound_PlaySong(0x51c);
    }
    return 0;
}
