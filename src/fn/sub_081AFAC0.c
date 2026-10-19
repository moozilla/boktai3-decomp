#include "global.h"

#ifdef NONMATCHING
// fuzzy: 81.4%
// foundry draft (worker best 81.4%); what differs is mostly register allocation and instruction order
#define ActorStatus_InitializeContact ActorStatus_InitializeContact
s32 Math_Atan2(s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void sub_0821FF8C(void *);
extern u8 *gUnk_02000580;
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
void ActorStatus_InitializeContact(struct ContactActor *actor, struct ContactActor *other,
                  struct ActorStatus *status)
{
    u8 *world = gUnk_02000580;
    s32 strength, shift, scaled;
    u32 value;
    u32 direction;
    sub_0821FF8C(actor);
    strength = other->strength;
    shift = status->strengthShift;
    if (strength >= 0)
        scaled = strength >> shift;
    else
        scaled = -(-strength >> shift);
    status->contactStrength = scaled;
    value = actor->value38;
    status->contactValue = value;
    status->contactKind = actor->value44;
    if (status->mode == 1 && Player_IsSlotInActionTwelveOrThirtyEight(0) && world[0x418] == 2) {
        if (status->contactStrength < status->duration)
            status->contactStrength = status->duration;
    }
    {
        u32 mask = 0x100;
        if (actor->flags & mask)
            direction = actor->direction;
        else {
            s32 dx = other->x - actor->x;
            s32 dy = other->y - actor->y;
            direction = Math_Atan2(dx, dy);
        }
    }
    status->contactDirection = direction;
    PlayerStats_AddContactHundredths(actor, other, 5);
}
#else
INCLUDE_ASM("asm/nonmatching", ActorStatus_InitializeContact);
#endif
