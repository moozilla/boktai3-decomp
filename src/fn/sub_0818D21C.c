#include "motion_0818.h"
#include "game_state.h"
#include "engine_globals.h"
// MATCHING_CONCESSION: register-pinning
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
u32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void Armor_AcceleratePartTurn(struct Motion0818Object *);
void Armor_InitPartLunge(struct Motion0818Object *p)
{
    u8 **player = &gGameState;
    u8 *target;
    register s32 turn asm("r1");
    s32 choice;
    u32 one;
    target = *player;
    p->angle = Math_Atan2(*(s16 *)(target + 0x30) - p->x, *(s16 *)(target + 0x34) - p->z);
    p->targetRotation = 0xA0 - p->angle;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        register const u16 *table asm("r3") = gUnk_0203B400;
        choice = *(const u16 *)(offset + (u32)table);
    }
    one = 1;
    choice &= one;
    {
        register s32 negative asm("r3") = -1;
        asm("" : "+r"(negative));
        turn = negative;
    }
    if (choice)
        turn = 1;
    p->turnDirection = turn;
    p->done = one;
    p->startX = p->x;
    p->startY = p->y;
    p->startZ = p->z;
    p->endX = p->x + Div(800 * gUnk_086149C4[(p->angle + 64) & 255], 4096);
    p->endY = *(u16 *)((*player) + 0x32) + 220;
    p->endZ = p->z + Div(800 * gUnk_086149C4[p->angle], 4096);
    p->dx = p->endX - p->startX;
    p->dy = p->endY - p->startY;
    p->dz = p->endZ - p->startZ;
    p->callback = Armor_AcceleratePartTurn;
}
