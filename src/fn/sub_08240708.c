#include "global.h"
#include "player.h"
#include "engine.h"
#define Player_UpdateSlidingRecovery sub_08240708
// confidence=low: sliding motion, terrain stop, and blinking recovery phases.
struct RecoveryVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
u32 ActorVariant_GetTableValue(u32, u32);
u32 sub_08238280(void *, u32, u32);
void Player_PlayLocalSong(u32, u32);
void Player_SetTetherMode(void *, u32, u32);
void Player_SetSecondaryActionFlags(void *, u32);
void sub_0823C280(void *);
void sub_08109040(void *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 Player_IsFlaggedWithGlobalModifier10(u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u32 properties(struct RecoveryVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8, cell;
    u8 *data;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + (cell * 4 + 12);
    return *(u16 *)(data + 2);
}
static inline void slideMotion(struct Player *p, u16 *magnitude)
{
    s32 angle = ((p->facing + 1) & 7) << 5;
    p->velocityX = scale(gUnk_086149C4[(angle + 64) & 255] * *magnitude);
    p->velocityZ = scale(gUnk_086149C4[angle] * p->directionFrame);
}
// Remaining differences: speed-address lifetime, scratch registers, and saved-register frame.
#ifdef NONMATCHING
// fuzzy: 92.2%
void Player_UpdateSlidingRecovery(struct Player *p)
{
    if (p->tetherMode != 0x1F) {
        sub_08238280(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40);
        p->interactionResetCounter = 0;
        if (p->actionMode == 6)
            Player_PlayLocalSong(p->slot, 0x2EB);
        else
            Player_PlayLocalSong(p->slot, 0xE7);
        Player_PlayLocalSong(p->slot, 0xE6);
        Player_SetTetherMode(p, 0x1F, 0);
    }
    switch (p->tetherPhase) {
        case 0: {
            struct RecoveryVec pos;
            slideMotion(p, &p->directionFrame);
            p->directionFrame = (p->directionFrame * 3) >> 2;
            pos = *(struct RecoveryVec *)&p->position;
            pos.x += p->velocityX;
            pos.z += p->velocityZ;
            if (Player_IsFlaggedWithGlobalModifier10(properties(&pos))) {
                p->velocityX = 0;
                p->velocityZ = 0;
            }
            if (!p->directionFrame)
                Player_SetTetherMode(p, 0x1F, 1);
            break;
        }
        case 1:
            if (p->actionCueFlags == 3 && !p->actionCueCounter) {
                Player_PlayLocalSong(p->slot, 0xD1);
                sub_08109040(&p->position, p->slot);
            }
            if (sub_08238280(p, ActorVariant_GetTableValue(p->actionMode, 0x2E), 0x40))
                Player_SetTetherMode(p, 0x1F, 2);
            break;
        case 2:
            if (++p->tetherFrame > 15)
                Player_SetTetherMode(p, 0x1F, 3);
            break;
        case 3:
            if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 3))
                Player_SetSecondaryActionFlags(p, 5);
            if (++p->tetherFrame > 55)
                sub_0823C280(p);
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08240708);
#endif
