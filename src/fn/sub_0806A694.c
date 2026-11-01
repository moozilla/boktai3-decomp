// MATCHING_CONCESSION: register-pinning
#include "sprite_effect_controller.h"
void SpriteEffectController_RefreshCollider(struct SpriteEffectController *controller,
                                            struct ContactPosition *position)
{
    u32 flags = controller->flags;
    u32 enabled = flags & 4;
    if (enabled) {
        u32 refresh = flags & 0x100;
        DEBUG_NOP();
        if (refresh) {
            struct ContactCollider *collider = &controller->collider;
            if (position) {
                Collider_SetPosition(collider, position, 0);
            }
            Collider_Refresh(collider);
        }
    }
}
