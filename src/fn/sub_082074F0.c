// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "vulcan_cannon_worker.h"
#include "ram.h"
extern u32 gUnk_02000560;
void VulcanCannon_UpdateTargeting(void);
void VulcanCannon_UpdateDefeatEffects(void);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Explosion_CreateAt(void *, s32, s32, s32, s32, s32, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, s32, s32, s32, s32, s32, s32);
struct CannonEffectPair {
    s32 lo : 16;
    u32 hi : 16;
};
struct CannonEffectVector {
    struct CannonEffectPair a, b;
};
struct CannonCollider {
    u8 pad[6];
    u16 flags;
};
static inline u32 TestPair(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 TestFlag(u32 mask, u32 *p)
{
    return *p & mask;
}
static inline s32 CannonRandom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}
static inline void CopyPosition(struct SceneVec *out, const struct SceneVec *in)
{
    *out = *in;
}
static inline void CannonEffectOffset(struct SceneVec *v)
{
    v->y += 128;
}
s32 VulcanCannon_UpdateDamageState(struct VulcanCannon *p)
{
    struct CannonCollider *collider;
    if (TestPair(0x400, &gEngineFlags, &gScenePauseFlags) || TestFlag(4, &gUnk_02000560))
        return 0;
    p->previousVisible = p->targetVisible;
    Callback_InvokeActor(p, p->state);
    p->display.sprite.flags &= ~1;
    if (!p->dead) {
        struct CannonCollider *c = (void *)p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->display.spatial.position, 0);
        collider = c;
    } else {
        struct CannonCollider *c = (void *)p->collider;
        c->flags |= 4;
        collider = c;
    }
    if (p->damage > 0 && !p->dead) {
        p->hp -= p->damage;
        p->hitFlags = p->colliderHitFlags;
        Sound_PlaySong(0x13e);
        if (p->damage > 0) {
            DamageNumberDisplay_Show(collider, p->damage, 0, p->collider + 0xc, p->collider + 0x14,
                                     1);
            SceneHud_SetTimedState(p->owner, p->hp, p->maxHp, p->hudStyle,
                                   *(u8 *)((u8 *)p + 0xf7) == 4, p->hudDuration);
        }
        if (p->state == VulcanCannon_UpdateTargeting)
            p->aimTimer = 120;
        if (p->hp <= 0) {
            struct SceneVec center, smoke;
            struct CannonEffectVector velocity, spread;
            const u16 *table;
            p->dead = 1;
            if (p->terrainEnabled)
                TerrainModifiers_UnregisterCell(p->terrain);
            CopyPosition(&center, &p->display.spatial.position);
            CannonEffectOffset(&center);
            velocity.a.lo = -4;
            table = (const u16 *)RANDOM_TABLE_ADDR;
            velocity.a.hi = (CannonRandom(table) & 7);
            velocity.b.lo = -4;
            spread.a.lo = 24;
            spread.a.hi = (CannonRandom(table) & 7) + 3;
            spread.b.lo = 24;
            sub_08014634((CannonRandom(table) & 3) + 4, 13, &center, &velocity, &spread, 15, 15);
            CopyPosition(&smoke, &p->display.spatial.position);
            CannonEffectOffset(&smoke);
            {
                s32 zero = 0;
                Explosion_CreateAt(&smoke, 16, 0, zero, zero, 60, zero, zero);
            }
            smoke.x += 128;
            {
                s32 zero = 0;
                Explosion_CreateAt(&smoke, 16, 0, 0, zero, 60, zero, 2);
            }
            smoke.x -= 128;
            smoke.z += 128;
            {
                s32 zero = 0;
                Explosion_CreateAt(&smoke, 16, 0, 0, zero, 60, zero, 2);
            }
            smoke.z -= 128;
            smoke.x -= 128;
            {
                s32 zero = 0;
                Explosion_CreateAt(&smoke, 16, zero, 0, zero, 60, zero, 5);
            }
            smoke.x += 128;
            smoke.z -= 128;
            {
                s32 zero = 0;
                Explosion_CreateAt(&smoke, 16, 0, zero, zero, 60, zero, 5);
            }
            {
                void *seq, *res;
                u32 facing;
                {
                    register u32 off asm("r4") = 0x10c;
                    asm("" : "+r"(off));
                    seq = (u8 *)p + off;
                }
                {
                    register u32 off asm("r2") = 0x17c;
                    asm("" : "+r"(off));
                    {
                        register u8 *addr asm("r1") = (u8 *)p + off;
                        res = *(void **)addr;
                    }
                }
                {
                    register u32 off asm("r3") = 0x132;
                    asm("" : "+r"(off));
                    {
                        register u8 *addr asm("r2") = (u8 *)p + off;
                        facing = *addr;
                    }
                }
                Anim_RestartSequence(seq, res, 0x13, facing, 0);
            }
            AnimData_SelectPaletteBank(p->graphics, 0x1be);
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
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flash--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1be);
    if (*(u16 *)((u8 *)p + 0xf8)) {
        u16 n = --*(u16 *)((u8 *)p + 0xf8);
        if ((n >> 2) & 1)
            p->display.sprite.flags |= 1;
        else
            p->display.sprite.flags &= ~1;
    }
    p->timer++;
    if (p->aimTimer > 0)
        p->aimTimer--;
}
