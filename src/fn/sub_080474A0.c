#include "transient_sprite_effects.h"
#define TransientSpriteEffects_Create     sub_080474A0
#define TransientSpriteEffects_Update     sub_08047338
#define TransientSpriteEffects_Destroy    sub_080473F0
#define TransientSpriteEffects_Initialize sub_08047424
s32 TransientSpriteEffects_Update(struct TransientSpriteEffects *);
s32 TransientSpriteEffects_Destroy(struct TransientSpriteEffects *);
s32 TransientSpriteEffects_Initialize(struct TransientSpriteEffects *, void *);
struct TransientSpriteEffects *TransientSpriteEffects_Create(void *context, u32 unused)
{
    struct TransientSpriteEffects *pool = Task_Create(10, sizeof(*pool));
    if (pool) {
        Task_SetCallbacks(pool, (void (*)(void *))TransientSpriteEffects_Update,
                          (void (*)(void *))TransientSpriteEffects_Destroy);
        if (TransientSpriteEffects_Initialize(pool, context) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
