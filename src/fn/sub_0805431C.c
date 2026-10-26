#include "hit_switch.h"
void HitSwitch_ReceiveCollision(struct SceneCollider *other, void *unused,
                                struct HitSwitchEntry *entry)
{
    u8 *cooldown = &entry->hitCooldown;
    if (*cooldown == 0) {
        u32 mask = 0x20;
        if (other->contactFlags2 & mask) {
            entry->hitCount++;
            *cooldown = mask;
            gHitSwitchManager->hitPending = 1;
        }
    }
}
