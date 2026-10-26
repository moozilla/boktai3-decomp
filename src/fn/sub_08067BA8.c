// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control.h"
#include "ram.h"
/* SceneActorControl_RequestAllDepartures: Requests departure and sets collider flags for every occupied slot not already departing */
#define SceneActorControl_RequestAllDepartures sub_08067BA8
#define SceneActorControl_RequestAllDepartures sub_08067BA8
static inline u8 IsUsed(struct SceneActorControlManager *p, u32 index)
{
    u32 bit = 1U << index;
    asm("" : "+r"(bit) : : "memory", "r6");
    if (p->used & bit)
        return 1;
    return 0;
}
void SceneActorControl_RequestAllDepartures(void)
{
    struct SceneActorControlManager *p = gSceneActorControlManager;
    if (p) {
        s32 i;
        for (i = 0; i < 8; i++) {
            if (IsUsed(p, i)) {
                struct SceneActorControlSlot *slot = (void *)((u8 *)p + (i * 0xAC + 0x3C));
                if (slot->index >= 0 && slot->kind != 2) {
                    u16 *flags = &slot->flags;
                    *flags |= 2;
                    flags = (void *)((u8 *)flags + 0x16);
                    *flags |= 4;
                }
            }
        }
    }
}
