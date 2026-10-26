// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"

void Anim_Remove(void *);
void Collider_Remove(void *);
void Sprite_Remove(void *);
void ScaledShadow_Destroy(void *);
void SceneLink_RemoveNode(void *);
// confidence=low: removes the controller animation and both embedded part records.
#define Vanargand_DestroyPartController sub_081B5E38
void Vanargand_DestroyPartController(u8 *controller)
{
    u8 *node, *shadow, *collider, *sprite;
    s32 i;
    Anim_Remove(controller + 0x18);
    node = controller + 0x2a4;
    shadow = controller + 0x238;
    collider = controller + 0x1a0;
    sprite = controller + 0x98;
    i = 1;
    do {
        Collider_Remove(collider);
        if (sprite[4])
            Sprite_Remove(sprite);
        ScaledShadow_Destroy(shadow);
        SceneLink_RemoveNode(node);
        {
            register u32 stride asm("r0") = 0x21c;
            node += stride;
            shadow += stride;
            collider += stride;
            sprite += stride;
        }
    } while (--i >= 0);
    {
        u32 zero = 0;
        gVanargandPartController = (void *)zero;
    }
}
