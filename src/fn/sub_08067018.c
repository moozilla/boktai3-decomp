// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control.h"
#define SceneActorControl_TickJitter sub_08067018
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
void SceneActorControl_TickJitter(void *unused, struct SceneActorControlSlot *slot)
{
    register s16 *timer asm("r5") = &slot->timer;
    s32 remaining;
    remaining = *timer;
    if (remaining <= 0) {
        u32 reset = 20;
        *timer = reset;
        {
            u16 *flags = &slot->flags;
            register u32 old asm("r1") = *flags;
            *flags = old & (reset - 25);
        }
    } else {
        if (remaining > 10) {
            const u16 *table = gUnk_0203B400;
            register u32 mask asm("r8");
            s32 jitter;
            u32 index;
            register u32 lowMask asm("r1");
            index = gUnk_03005308 + 1;
            lowMask = 1023;
            mask = lowMask;
            gUnk_03005308 = index & lowMask;
            jitter = Mod(*(const u16 *)((gUnk_03005308 << 1) + (u32)table), 40);
            {
                register u32 x asm("r1") = (u16)slot->position.x - 20;
                slot->position.x = x + jitter;
            }
            gUnk_03005308 = (gUnk_03005308 + 1) & mask;
            jitter = Mod(*(const u16 *)((gUnk_03005308 << 1) + (u32)table), 40);
            {
                register u32 z asm("r1") = (u16)slot->position.z - 20;
                slot->position.z = z + jitter;
            }
        }
        (*timer)--;
    }
}
