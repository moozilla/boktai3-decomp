#include "global.h"
#include "system/game_state/game_state_flags.h"
#include "engine/ram.h"
#include "effects/screen/background_controller.h"
#include "objects/scene_object.h"
#include "system/game_state/game_state.h"
#include "scene/actor/actor_stats.h"
#include "scene/collision/contact_actor.h"
#include "scene/actor/modifier_list.h"
#include "effects/particles/scene_debris_types.h"

void Actor_ClampMovementSpeed(struct ActorStats *actor)
{
    if (actor->movementOffset > 0xc8000)
        actor->movementOffset = 0xc8000;
    if (actor->movementOffset <= 0x1ffff)
        actor->movementOffset = 0x20000;
}

extern s8 gUnk_086106C0[];

void Actor_SetSpriteDepthOffsets(struct CharacterPreviewModel *model, s32 d)
{
    s32 i;
    for (i = 0; i < 3; i++)
        model->view.parts[i].z = gUnk_086106C0[i] + d;
    model->view.parts[3].z = gUnk_086106C0[3] + d;
    model->view.parts[4].z = gUnk_086106C0[3] + d;
}

/* tentative: positions sprite parts, easing two toward terrain clearance */

extern const u16 gUnk_086141BC[];

s32 Terrain_GetCellClearance(s16 *);

void Anim_SetFrameDurationScale(void *, u32);

static inline int Rand(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void Actor_UpdateShadowSpriteParts(struct ActorStats *actor, struct CharacterPreviewModel *model)
{
    struct ContactPosition pos;
    struct ContactPackedVector tmp;
    s32 i;
    s32 shadowHeight;
    s32 groundHeight;
    s32 delta;
    s32 v;
    s16 *coordinates = &pos.x;
    pos.x = (actor->position.x >> 12) - gSceneCamera->x;
    coordinates[1] = ((actor->position.z + (actor->position.y + actor->height)) >> 12) + 0x5A;
    coordinates[2] = actor->position.z >> 12;
    for (i = 0; i < 3; i++) {
        model->view.parts[i].x = pos.x;
        model->view.parts[i].y = pos.y;
    }
    for (i = 3; i <= 4; i++) {
        s32 ax = actor->position.x >> 12;
        pos.x = ax - gSceneCamera->x;
        tmp.y = actor->position.y >> 12;
        tmp.z = actor->position.z >> 12;
        switch (i) {
            case 3:
                pos.x += 15;
                pos.y -= 2;
                tmp.x = ax + 15;
                break;
            case 4:
                pos.x -= 19;
                pos.y -= 2;
                tmp.x = ax - 19;
                break;
        }
        groundHeight = Terrain_GetCellClearance((s16 *)&tmp) - 6;
        delta = pos.y - groundHeight;
        if (delta > 8)
            shadowHeight = pos.y - 8;
        else if (delta < -8)
            shadowHeight = pos.y + 8;
        else
            shadowHeight = groundHeight;
        model->view.parts[i].x = pos.x;
        if (model->view.parts[i].y + 3 < shadowHeight)
            model->view.parts[i].y += 3;
        else if (model->view.parts[i].y - 3 > shadowHeight)
            model->view.parts[i].y -= 3;
        else
            model->view.parts[i].y = shadowHeight;
        model->view.parts[i].z = (actor->position.z >> 12) - 6;
        if ((Rand() & 15) > 9) {
            model->view.parts[i].y += (Rand() & 3) - 1;
        }
    }
    v = (actor->motion.forward >> 12) + 1;
    if (v != 1) {
        s32 sc = 0x40 - v * 2;
        sc = sc < 0x20 ? 0x20 : sc;
        Anim_SetFrameDurationScale(&model->view.parts[3], (u16)sc);
        Anim_SetFrameDurationScale(&model->view.parts[4], (u16)sc);
    }
    Actor_SetSpriteDepthOffsets(model, actor->position.z >> 12);
}

u32 ActorEffect_SpawnAtActorPos();

static inline u32 ActorParticles_NextRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

/* Tentative: emit paired particles when the actor reports an impact. */

void ActorParticles_SpawnImpactBurst(void *actor)
{
    struct SceneExtent offset, velocity;
    s32 i;
    u8 modifier;
    Sound_PlaySong(0x90);
    Memory_Clear(&offset, 8);
    modifier = GameState_TestWorldFlags(8);
    for (i = 0; i < 4; i++) {
        offset.x = (ActorParticles_NextRandom() & 31) - 7;
        velocity.x = (ActorParticles_NextRandom() & 3) - 2;
        velocity.y = -(ActorParticles_NextRandom() & 1);
        velocity.z = 0;
        ActorEffect_SpawnAtActorPos(actor, &offset, &velocity, 0x409C, 0x1C22, 4, 0, modifier, 0,
                                    0xE3, 30, 0, 512, 1, 0);
        ActorEffect_SpawnAtActorPos(actor, &offset, &velocity, 0x409C, 0x1C22, 3, 0, modifier, 0,
                                    0xE3, 12, 0, 1024, 1, 0);
    }
}

u32 ActorParticles_CreateAtOffset();

static inline u16 NextOffsetRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void ActorParticles_EmitOffsetBurst(void *actor)
{
    s32 i;
    struct SceneExtent offset;
    for (i = 0; i < 4; i++) {
        offset.x = (NextOffsetRandom() & 31) - 7;
        offset.y = -(NextOffsetRandom() & 31);
        offset.z = 0;
        ActorParticles_CreateAtOffset(actor, &offset, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0,
                                      0, 1, 0);
    }
}

/* tentative: spawns a jittered pair of particles moving left and right */

struct SideBurstActor {
    u8 pad0[0x4a];
    u8 field4A;
    u8 field4B;
};

u32 ActorEffect_SpawnAtActorPos(struct SideBurstActor *, struct ContactPackedVector *,
                                struct ContactPackedVector *, u32, u32, u32, u32, u32, u32, u32,
                                u32, u32, u32, u32, u32);

/* Spawns two 0x3D99 particles from the actor: a shared jittered base offset
   (y = -32) and two velocity offsets, one leftward then one rightward (tentative). */

void ActorParticles_SpawnSideBurst(struct SideBurstActor *p)
{
    struct ContactPackedVector a, b;
    u32 anim = p->field4B + 0x378;
    u8 layer;
    a.x = (ActorParticles_NextRandom() & 31) - 7;
    a.y = -32;
    a.z = 0;
    b.x = -(ActorParticles_NextRandom() & 3) - 3;
    b.y = -(ActorParticles_NextRandom() & 7) - 3;
    b.z = (ActorParticles_NextRandom() & 7) - 3;
    layer = p->field4A;
    ActorEffect_SpawnAtActorPos(p, &a, &b, 0x3d99, 0, layer, 1, 0, 0, anim, 0x30, 1, 0, 2, 6);
    b.x = (ActorParticles_NextRandom() & 3) + 2;
    b.y = -(ActorParticles_NextRandom() & 7) - 2;
    b.z = (ActorParticles_NextRandom() & 7) - 3;
    layer = p->field4A;
    ActorEffect_SpawnAtActorPos(p, &a, &b, 0x3d99, 0, layer, 1, 0, 0, anim, 0x30, 1, 0, 2, 6);
}

struct ActorParticleEmitter {
    u8 interval, countdown, count, enabled;
};

u32 ActorParticles_CreateAtOffset(void *, struct SceneExtent *, struct SceneExtent *, u32, u32, u32,
                                  u32, u32, u32, u32, u32, u32, u32, u32, u32);

void ActorParticles_UpdateEmitter(void *actor, struct ActorParticleEmitter *emitter)
{
    struct SceneExtent offset, velocity;
    s32 i;
    if (!emitter->enabled)
        return;
    if (emitter->countdown) {
        emitter->countdown--;
        return;
    }
    emitter->countdown = emitter->interval;
    for (i = 0; i < emitter->count; i++) {
        offset.x = (ActorParticles_NextRandom() & 31) - 7;
        offset.y = 0;
        offset.z = 0;
        velocity.x = (ActorParticles_NextRandom() & 1) + 1;
        velocity.y = -(ActorParticles_NextRandom() & 1);
        velocity.z = 0;
        ActorParticles_CreateAtOffset(actor, &offset, &velocity, 0x4384, 0xf62b, 0, 0, 0, 0, 0xb8,
                                      30, 0, !(i & 1) ? 512 : 1024, 1, 0);
    }
}

struct State {
    u8 pad0[0x14];
    s32 value, limit;
};

s32 Div(s32, s32);

void Character_SetLowResourceColorEffect(u8 *p, struct State *s)
{
    s32 six = Div(s->limit * 6, 10);
    s32 four = Div(s->limit * 4, 10);
    s32 two = Div(s->limit * 2, 10);
    if (s->value <= two) {
        p[0] = 20;
        p[2] = 2;
        p[3] = 1;
        return;
    } else if (s->value <= four) {
        p[0] = 40;
        p[2] = 2;
        p[3] = 1;
        return;
    } else if (s->value <= six) {
        p[0] = 60;
        p[2] = 1;
        p[3] = 1;
        return;
    } else
        p[3] = 0;
}

/* tentative: counts a held charge, spawning particles, and fires the callback on release */

struct Size {
    s16 w, h, a, b;
};

struct Input {
    u16 held;
    u16 pad;
    u16 pressed;
};

struct Owner {
    u8 filler0[0x20];
    struct ContactPosition pos;
    u8 filler28[0x251 - 0x28];
    s8 markerSlot;
};

struct Self {
    u8 filler0[0x58];
    s16 value;
    u8 filler5A[0x2B0 - 0x5A];
    void (*callback)(void *, s32);
    u8 filler2B4[0x2B8 - 0x2B4];
    u16 threshold;
    u8 filler2BA[0x2BE - 0x2BA];
    u16 timer;
};

void Sound_PlaySong(u32);

s32 Mod(s32, s32);

void ActorMarker_ShowSlot(s32);

void ActorMarker_HideSlot(s32);

s32 SpriteParticle_SpawnSingleFrameBatch(struct ContactPosition *, struct ContactPosition *,
                                         struct Size *, u32, u32, u32, u32);

void ChargeMarker_TickHold(struct Self *self, struct Owner *owner, struct Input *input)
{
    if (input->held & 2) {
        if (self->value >= self->threshold) {
            self->timer++;
            if (self->timer == 10)
                Sound_PlaySong(0x1F8);
            else if (self->timer == 50)
                Sound_PlaySong(0x1F8);
            if (self->timer > 60) {
                ActorMarker_ShowSlot(owner->markerSlot);
            } else if (Mod(self->timer, 12) == 0) {
                struct ContactPosition pos;
                struct Size size;
                pos = owner->pos;
                pos.x += 32;
                pos.y -= 16;
                size.w = 0x16;
                size.h = 0x14;
                SpriteParticle_SpawnSingleFrameBatch(
                    &pos, &owner->pos, &size, (ActorParticles_NextRandom() & 3) + 1, 1, 0, 0);
            }
        } else {
            self->timer = 0;
            ActorMarker_HideSlot(owner->markerSlot);
        }
    } else if ((input->pressed & 2) && self->timer) {
        self->callback(&self->callback, 0);
        self->timer = 0;
        ActorMarker_HideSlot(owner->markerSlot);
    }
}
