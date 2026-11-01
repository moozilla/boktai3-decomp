#include "scene_particle_burst.h"
#include "scene_particle_step.h"
s32 Div(s32, s32);
void SceneParticleBurst_UpdateSlot(struct SceneParticleBurstPool *p,
                                   struct SceneParticleBurstSlot *slot)
{
    struct SceneParticleEffect *sprite = &p->effects[slot->index];
    if (slot->delay > 0) {
        slot->delay--;
        return;
    }
    sprite->flags &= ~1;
    slot->position.x += slot->velocity.x;
    slot->position.y += slot->velocity.y;
    slot->position.z += slot->velocity.z;
    slot->velocity.x = Div(slot->velocity.x * 7, 10);
    slot->velocity.y = Div(slot->velocity.y * 7, 10);
    slot->velocity.z = Div(slot->velocity.z * 7, 10);
    sprite->position = slot->position;
    {
        s32 index = slot->index;
        const u16 *graphics = p->graphicsResource;
        if (SceneParticle_StepSprite(&p->sequences[index], sprite, graphics))
            slot->state = 2;
    }
    slot->frame++;
}
