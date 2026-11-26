#include "scene/collision/contact_actor.h"
#include "global.h"
#include "monsters/cannons/vulcan_cannon_worker.h"
#include "engine/ram.h"
#include "scene/collision/contact_collider.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"
#include "player/player.h"
void VulcanCannon_UpdateTargeting(void);
void VulcanCannon_UpdateDefeatEffects(void);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
u8 *Explosion_CreateAt(void *, u32, s32, s32, s32, s32, s32, u32);
u32 Anim_RestartSequence(struct DisplaySequenceState *, void *, u16, u8, u8);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, s32, s32, s32, s32, s32, s32);
static inline s32 CannonRandom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
s32 VulcanCannon_UpdateDamageState(struct VulcanCannon *p)
{
    struct ContactCollider *collider;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    p->previousVisible = p->targetVisible;
    Callback_InvokeActor(p, p->state);
    p->display.flags &= ~1;
    if (!p->dead) {
        struct ContactCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->display.position, 0);
        collider = c;
    } else {
        struct ContactCollider *c = &p->collider;
        c->flags |= 4;
        collider = c;
    }
    if (p->damage > 0 && !p->dead) {
        p->hp -= p->damage;
        p->hitFlags = p->colliderHitFlags;
        Sound_PlaySong(0x13e);
        if (p->damage > 0) {
            DamageNumberDisplay_Show(collider, p->damage, 0, &p->collider.position, &p->collider.offset,
                                     1);
            SceneHud_SetTimedState(p->owner, p->hp, p->maxHp, p->hudStyle,
                                   p->collider.costFlags == 4,
                                   p->hudDuration);
        }
        if (p->state == VulcanCannon_UpdateTargeting)
            p->aimTimer = 120;
        if (p->hp <= 0) {
            struct ContactPosition center, smoke;
            s32 explosionFrame = 0;
            struct ContactPackedVector velocity, spread;
            const u16 *table;
            p->dead = 1;
            if (p->terrainEnabled)
                TerrainModifiers_UnregisterCell(p->terrain);
            VulcanCannon_GetEffectPosition(&center, p);
            velocity.x = -4;
            table = gRandomTable;
            velocity.y = (CannonRandom(table) & 7);
            velocity.z = -4;
            spread.x = 24;
            spread.y = (CannonRandom(table) & 7) + 3;
            spread.z = 24;
            sub_08014634((CannonRandom(table) & 3) + 4, 13, &center, &velocity, &spread, 15, 15);
            VulcanCannon_GetEffectPosition(&smoke, p);
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 0);
            smoke.x += 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.x -= 128;
            smoke.z += 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.z -= 128;
            smoke.x -= 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 5);
            smoke.x += 128;
            smoke.z -= 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, explosionFrame, 5);
            {
                struct DisplaySequenceState *seq;
                void *res;
                u32 facing;
                seq = &p->sequence;
                res = p->resource;
                facing = (u8)p->facing;
                Anim_RestartSequence(seq, res, 0x13, facing, 0);
            }
            AnimData_SelectPaletteBank(&p->graphics, 0x1be);
            p->flash = 0;
            p->state = VulcanCannon_UpdateDefeatEffects;
            p->timer = 0;
            Sound_PlaySong(0x52d);
        }
        p->flash = 8;
        p->damaged = 1;
        p->damage = 0;
    }
    if (p->targetVisible != p->previousVisible) {
        if (p->targetVisible == 1) {
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, p->body, 0, 3, 0, 0, 150, 250);
            Sound_PlaySong(0x1ec);
        } else {
            p->attachment =
                ActorAttachment_UpdateFromDescriptor(p->attachment, p->body, 0, 2, 0, 0, 150, 250);
            Sound_PlaySong(0x1ed);
        }
    }
    if (p->aimTimer == 1 && !p->dead) {
        p->attachment =
            ActorAttachment_UpdateFromDescriptor(p->attachment, p->body, 0, 2, 0, 0, 150, 250);
        Sound_PlaySong(0x1ed);
    }
    if (p->aimTimer == 120 && !p->dead) {
        p->attachment =
            ActorAttachment_UpdateFromDescriptor(p->attachment, p->body, 0, 1, 0, 0, 150, 250);
        Sound_PlaySong(0x1b1);
    }
    if (p->flash > 0 && !p->dead) {
        AnimData_SelectPaletteBank(&p->graphics, 0x192);
        p->flash--;
    } else
        AnimData_SelectPaletteBank(&p->graphics, 0x1be);
    if (p->collider.responseValue) {
        u16 n = --p->collider.responseValue;
        if ((n >> 2) & 1)
            p->display.flags |= 1;
        else
            p->display.flags &= ~1;
    }
    p->timer++;
    if (p->aimTimer > 0)
        p->aimTimer--;
}
