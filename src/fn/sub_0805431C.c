#include "hit_switch.h"
#define HitSwitch_ReceiveCollision sub_0805431C
void HitSwitch_ReceiveCollision(struct SceneCollider *other, void *unused,
                                struct HitSwitchEntry *entry)
{
    u8 *cooldown = &entry->hitCooldown;
    if (*cooldown == 0) {
        u32 mask = 0x20;
        if (other->contactFlags2 & mask) {
            entry->hitCount++;
            *cooldown = mask;
            gUnk_02000490->hitPending = 1;
        }
    }
}
