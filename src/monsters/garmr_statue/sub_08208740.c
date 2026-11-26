#include "engine/video/display_sequence_sprite.h"
#include "monsters/garmr_statue.h"
#include "engine/video/display_sequence_inline.h"
#include "engine/video/display_animation.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/engine_pause.h"
#include "engine/math/monster_particle_random.h"
#include "player/player.h"
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void sub_082086B8(void);
void GarmrStatue_FireAimedVolley(void);
void Anim_SetDurationScale(void *, u16);
s32 GarmrStatue_UpdateCombat(struct GarmrStatue *p)
{
    struct SceneVec position;
    struct ContactPackedVector velocity;
    struct ContactPackedVector size;
    struct ContactCollider *collider;
    u32 ended;
    if (Engine_PauseBits(0x400))
        goto paused;
    {
        u32 mask = 4;
        if (gPlayerSlotStatus[0] & mask) {
        paused:
            return 0;
        }
    }
    Callback_InvokeActor(p, p->update);
    p->sprite.display.flags &= ~1;
    {
        struct ContactCollider *c;
        if (p->defeated == 0) {
            c = &p->collider;
            c->flags &= ~4;
            Collider_SetPosition(c, &p->sprite.coordinates.position, 0);
        } else {
            c = &p->collider;
            c->flags |= 4;
        }
        collider = &p->collider;
    }
    if ((p->damage > 0) && (p->defeated == 0)) {
        p->health -= p->damage;
        p->reactionFlags = p->pendingReaction;
        Sound_PlaySong(0x13e);
        if (p->damage > 0) {
            DamageNumberDisplay_Show(collider, p->damage, 0, &p->collider.position, &p->collider.offset,
                                     1);
            SceneHud_SetTimedState(p->hud, p->health, p->maxHealth, p->hudStyle, p->collider.costFlags == 4,
                                   p->hudTime);
        }
        if (p->health <= 0) {
            const u16 *table;
            s32 count;
            p->defeated = 1;
            p->collider.responseValue = 0;
            SceneVec_Copy(&position, &p->sprite.coordinates.position);
            {
                struct SceneVec *v = &position;
                v->y += 200;
            }
            velocity.x = -4;
            table = gRandomTable;
            velocity.y = ParticleRandom_ReadDirectSample(table) & 7;
            velocity.z = -4;
            size.x = 24;
            size.y = (ParticleRandom_ReadDirectSample(table) & 7) + 3;
            size.z = 24;
            count = (ParticleRandom_ReadDirectSample(table) & 3) + 4;
            sub_08014634(count, 2, &position, &velocity, &size, 15, 15);
            Anim_RestartSequence(&p->sequence, p->resource, 10, 0, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1bc);
            p->flashTimer = 0;
            p->update = sub_082086B8;
            p->age = 0;
            Sound_PlaySong(0x52c);
        }
        p->flashTimer = 8;
        p->hitReaction = 1;
        p->damage = 0;
    }
    if (((Mod(p->age, 240) == 0) && (p->defeated == 0)) &&
        (p->update != GarmrStatue_FireAimedVolley)) {
        Anim_SetSequence(&p->sequence, p->resource, 9, 0, 8);
        Anim_SetDurationScale(&p->sequence, 32);
        p->activeAnimation = 1;
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1bc);
    if (p->collider.responseValue) {
        p->collider.responseValue--;
        if ((p->collider.responseValue >> 2) & 1)
            p->sprite.display.flags |= 1;
        else
            p->sprite.display.flags &= ~1;
    }

    ended = DisplaySequence_StepSprite(&p->sprite.display, &p->sequence);

    if ((ended && (p->activeAnimation != 0)) && (p->defeated == 0)) {
        Anim_SetSequence(&p->sequence, p->resource, 8, 0, 0);
        p->activeAnimation = 0;
    }
    p->age++;
    /* The task callback has no explicit return assignment on the active path. */
}
