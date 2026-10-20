#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.3%
// Remaining differences: branch sharing, address lifetimes and register allocation.
// MATCHING_CONCESSION: register-pinning
#include "vulcan_cannon_worker.h"
#define VulcanCannon_UpdateDamageState sub_082074F0
extern u32 gUnk_030053F0, gUnk_02000560;
void sub_08206C00(void);
void sub_082071D0(void);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void sub_081DB390(void *, s32, s32, s32, s32, s32, s32, s32);
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
static inline s32 CannonRandom(const u16 *table, s32 mask)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308] & mask;
}
static inline void CannonEffectOffset(struct SceneVec *v)
{
    v->y += 128;
}
s32 VulcanCannon_UpdateDamageState(struct VulcanCannon *p)
{
    struct CannonCollider *collider;
    if (TestPair(0x400, &gUnk_030053F4, &gUnk_030053F0) || TestFlag(4, &gUnk_02000560))
        return 0;
    p->previousVisible = p->targetVisible;
    Callback_InvokeActor(p, p->state);
    p->display.sprite.flags &= ~1;
    if (!p->dead) {
        collider = (void *)p->collider;
        collider->flags &= ~4;
        Collider_SetPosition(collider, &p->display.spatial.position, 0);
    } else {
        collider = (void *)p->collider;
        collider->flags |= 4;
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
        if (p->state == sub_08206C00)
            p->aimTimer = 120;
        if (p->hp <= 0) {
            struct SceneVec center, smoke;
            struct CannonEffectVector velocity, spread;
            const u16 *table;
            p->dead = 1;
            if (p->terrainEnabled)
                TerrainModifiers_UnregisterCell(p->terrain);
            center = p->display.spatial.position;
            CannonEffectOffset(&center);
            velocity.a.lo = -4;
            table = (const u16 *)RANDOM_TABLE_ADDR;
            velocity.a.hi = CannonRandom(table, 7);
            velocity.b.lo = -4;
            spread.a.lo = 24;
            spread.a.hi = CannonRandom(table, 7) + 3;
            spread.b.lo = 24;
            sub_08014634(CannonRandom(table, 3) + 4, 13, &center, &velocity, &spread, 15, 15);
            smoke = p->display.spatial.position;
            CannonEffectOffset(&smoke);
            sub_081DB390(&smoke, 16, 0, 0, 0, 60, 0, 0);
            smoke.x += 128;
            sub_081DB390(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.x -= 128;
            smoke.z += 128;
            sub_081DB390(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.z -= 128;
            smoke.x -= 128;
            sub_081DB390(&smoke, 16, 0, 0, 0, 60, 0, 5);
            smoke.x += 128;
            smoke.z -= 128;
            sub_081DB390(&smoke, 16, 0, 0, 0, 60, 0, 5);
            Anim_RestartSequence(&p->sequence, p->resource, 0x13, (u8)p->facing, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1be);
            p->flash = 0;
            p->state = sub_082071D0;
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
    {
        register s32 result asm("r0") = 0;
        asm("" : "+r"(result));
        return result;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_082074F0);
#endif
