#define gArmorFacingAnimations gUnk_0824EB84
#include "global.h"
struct ArmorCollider {
    u16 unknown0[3];
    u16 flags;
};
struct ArmorCollisionGate {
    u32 unknown0;
    u32 flags;
};
/* Observed component offsets; collision interpretations are tentative. */
struct ArmorActor {
    u8 unknown0[0x180];
    struct ArmorCollisionGate collisionGate;
    u8 unknown188[8];
    struct ArmorCollider collider190;
    u8 unknown198[0x4c];
    struct ArmorCollider collider1E4;
    u8 unknown1EC[0x4c];
    struct ArmorCollider collider238;
    u8 unknown240[0x4c];
    struct ArmorCollider collider28C;
    u8 unknown294[0x89e];
    u16 flags;
    u8 unknownB34[5];
    u8 facing;
    u8 unknownB3A[6];
    u8 state;
    u8 phase;
};
extern const u32 gUnk_0824EB84[];
void Armor_SetAnimationFromStart(u32, u32);
void Armor_EnterFacingAnimation(struct ArmorActor *actor)
{
    u8 *p = (u8 *)actor;
    u8 *state = p + 0xb40;
    if (*state == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EB84[((p[0xb39] + 0x60) >> 6) & 3]);
        *state = 0xff;
        p[0xb41] = 1;
        ((struct ArmorCollider *)(p + 0x238))->flags &= ~four;
        ((struct ArmorCollider *)(p + 0x1e4))->flags &= ~four;
        ((struct ArmorCollider *)(p + 0x28c))->flags &= ~four;
        ((struct ArmorCollider *)(p + 0x190))->flags |= four;
        {
            u32 one = 1;
            ((struct ArmorCollisionGate *)(p + 0x180))->flags &= ~one;
        }
        *(u16 *)(p + 0xb32) &= ~0x20;
    }
}
