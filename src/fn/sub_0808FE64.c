#include "scene_actor_chase.h"
void Slime_StartRandomWait(void);
#define Slime_OnInterrupt sub_0808FE64
void Slime_OnInterrupt(struct SceneActorRegistryResetView *actor)
{
    u8 kind = actor->field2B5;
    if (kind != 0x26 && kind != 0x22 && kind != 0x23 && kind != 0x24) {
        Chase_SetCtl(actor, Slime_StartRandomWait, 0x26);
        Chase_SetState(actor, 0, 0, 0xF);
    }
}
