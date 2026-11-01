#include "scene_particle_pool.h"
#include "scene_particle_step.h"
void SceneParticle_UpdateSlot(struct SceneParticlePool *p, struct SceneParticleSlot *slot)
{
    struct SceneParticleEffect *sprite = &p->effects[slot->index];
    if (slot->delay > 0) {
        slot->delay--;
        return;
    }
    sprite->flags &= ~1;
    sprite->position.x = slot->anchor->x + slot->offset.x;
    sprite->position.y = slot->anchor->y + slot->offset.y;
    sprite->position.z = slot->anchor->z + slot->offset.z;
    {
        s32 index = slot->index;
        const u16 *graphics = p->graphicsResource;
        if (SceneParticle_StepSprite(&p->sequences[index], sprite, graphics))
            slot->state = 2;
    }
    slot->frame++;
}
