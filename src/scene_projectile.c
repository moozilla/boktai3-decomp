#include "global.h"
#include "ram.h"
#include "scene_projectile.h"
#include "player.h"
#include "engine.h"

u32 SceneProjectile_ConsumeHitEvent(void)
{
    struct SceneProjectilePool *pool = gSceneProjectilePool;
    u32 r;
    if (pool->hitPending == 0)
        r = 0;
    else {
        pool->hitPending = 0;
        r = 1;
    }
    return r;
}

void SceneProjectile_UpdateSpriteRotation(struct SceneProjectile *p)
{

    u32 t;
    p->spriteFlags = (p->spriteFlags & ~2) | 2;
    t = p->direction;
    p->direction = t + 0x10;
    p->spriteRotation = -(t + 0x70);
}

s32 Mod(s32, s32);

void sub_081FD4C0(void *);

s32 SceneProjectile_UpdatePulse(struct SceneProjectile *p)
{
    if (p->age > p->lifetime)
        return 1;
    if (Mod(p->age, 2) == 0)
        sub_081FD4C0(p);
    if (p->age == 1) {
        s32 v = p->pulse >> 2;
        p->pulse = v;
        if (v <= 0)
            p->pulse = 1;
    } else if (p->age == 30) {
        p->pulse <<= 2;
    } else if (p->age == 90 || p->age == 120) {
        s32 v = p->pulse >> 1;
        p->pulse = v;
        if (v <= 0)
            p->pulse = 1;
    }
    if (p->age < 120) {
        if (Mod(p->age, 10) == 0)
            p->phase--;
    }
    if (p->age >= 120) {
        if (Mod(p->age, 10) == 0)
            p->phase += 2;
    }
    {
        u32 mask = 2;
        p->flags = mask | p->flags;
    }
    return 0;
}

#define gRandomTableIndex gRandomIndex

extern u32 gRandomTableIndex;

void SlotMotion_Initialize(u8 *);

void SceneParticles_SpawnAtPosition(u8 *, s32 *);

void Ratatosk_ConfigureParticle(u8 *, u16, u32, u8, u32, u32);

s32 SceneProjectile_EmitPeriodicParticle(struct SceneProjectile *p)
{
    u8 particle[0x18];
    u32 scale;
    u32 offset;
    if (p->age > p->lifetime)
        return 1;
    {
        u32 mask = 2;
        p->flags = mask | p->flags;
    }
    if (Mod(p->age, 8) == 0) {
        gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
        offset = gRandomTableIndex * 2;
        scale = *(const u16 *)(offset + (u32)gRandomTable);
        scale &= 1;
        scale += 1;
        SlotMotion_Initialize(particle);
        Ratatosk_ConfigureParticle(particle, 40, 0, 0, 1, scale << 9);
        SceneParticles_SpawnAtPosition(particle, p->position);
    }
    return 0;
}

u32 SceneProjectile_UpdateHomingRotation(struct SceneProjectile *p)
{
    u32 r;
    if (p->age <= p->lifetime) {
        sub_081FD4C0(p);
        {
            u32 m = 2;
            p->flags = m | p->flags;
        }
        SceneProjectile_UpdateSpriteRotation(p);
        r = 0;
    } else
        r = 1;
    return r;
}

s32 Player_StartForcedStun(u8 *, s32);

void Sound_PlaySong(u32);

void SceneProjectile_RespondToContact(void *unused, struct SceneProjectileContact *contact,
                                      struct SceneProjectile *p)
{
    struct SceneProjectileContactState *state =
        (struct SceneProjectileContactState *)&p->stateUnknown;
    if (p->contactMode == 1 && contact->kind == 1)
        ((struct SceneProjectilePool *)gSceneProjectilePool)->hitPending = 1;
    if (p->contactMode == 0) {
        if ((*(u8 **)gPlayers)[0x457] != 26 && *(u16 *)((*(u8 **)gPlayers) + 0x4D8) == 0) {
            if (Player_StartForcedStun((*(u8 **)gPlayers), 90) == 1)
                Sound_PlaySong(1400);
        }
    }
    if (p->contactMode != 2) {
        p->age = state->lifetime;
    } else if (contact->kind == 32) {
        p->contactCount++;
        if (p->contactCount <= 1 && state->timer <= -10) {
            state->timer = 120;
            p->age = 0;
        }
    } else {
        if (p->age < state->lifetime)
            p->age = state->lifetime;
    }
}

struct SlotActorSignal {
    u8 pad0[0x34];
    u32 flags;
};

struct SlotActorState {
    u8 pad0[0xa];
    u16 state;
};

struct SlotActor {
    u32 flags;
    u8 pad4[0x28];
    u16 phase;
    s8 heading;
    u8 pad2F;
    u32 action;
    u8 pad34[0x9e];
    u16 enabled;
    u8 padD4[0xa4];
    u16 timer;
};

void SlotActor_ResetOnFlags(struct SlotActorSignal *signal, struct SlotActorState *state,
                            struct SlotActor *actor)
{
    if (signal->flags & 7) {
        Sound_PlaySong(0x170);
        actor->timer = 0;
        actor->heading -= 0x80;
        actor->flags = 0;
        actor->phase = 0;
        actor->action = 0x24;
        state->state = 1;
        actor->enabled = 1;
    }
}
