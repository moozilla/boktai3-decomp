#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.1%
// fix.py best (sweep after the inlined-insn fix)
// Parameter-save order and the first signed-load scratch still differ.
// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"

s32 Math_Atan2(s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Collision_CalculateDamageResponse(void *);
struct ContactActor {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x1C];
    s16 x;
    u8 pad26[2];
    s16 y;
    u8 pad2A[0xE];
    u32 value38;
    u8 pad3C[2];
    u16 strength;
    u8 pad40[2];
    u8 direction;
    u8 pad43;
    u16 value44;
};
struct ActorStatus {
    u8 pad0[0x15C];
    u32 mode;
    u8 pad160[0x14];
    s16 duration;
    u8 pad176[6];
    u32 contactValue;
    u16 contactStrength;
    u8 pad182[4];
    u8 contactKind;
    u8 contactDirection;
    u16 strengthShift;
};
void ActorStatus_InitializeContact(struct ContactActor *input, struct ContactActor *other,
                                   struct ActorStatus *status)
{
    u8 *world = (*(u8 **)gPlayers);
    s32 strength;
    s32 shift;
    s32 scaled;
    u32 value;
    u32 direction;
    Collision_CalculateDamageResponse(input);
    strength = other->strength;
    shift = status->strengthShift;
    if (strength >= 0)
        scaled = strength >> shift;
    else
        scaled = -((-strength) >> shift);
    status->contactStrength = scaled;
    value = input->value38;
    do {
        status->contactValue = value;
    } while (0);
    status->contactKind = input->value44;
    if (((status->mode == 1) && Player_IsSlotInActionTwelveOrThirtyEight(0)) &&
        (world[0x418] == 2)) {
        if (status->contactStrength < status->duration)
            status->contactStrength = status->duration;
    }
    {
        u32 mask = 0x100;
        if (input->flags & mask)
            direction = input->direction;
        else {
            s32 dx = other->x - input->x;
            s32 dy = other->y - input->y;
            direction = Math_Atan2(dx, dy);
        }
    }
    status->contactDirection = direction;
    PlayerStats_AddContactHundredths(input, other, 5);
}

#else
INCLUDE_ASM("asm/nonmatching", ActorStatus_InitializeContact);
#endif
