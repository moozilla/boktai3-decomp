#include "global.h"
#include "display_sequence_inline.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "render.h"
#include "engine.h"
#include "ram.h"
// confidence=low: handles collision damage, defeat effects, palette flashing and animation.
extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
void OrangeSpine_UpdateDefeatEffect(u8 *);
void Explosion_CreateAt(void *, s32, s32, s32, s32, s32, s32, s32);
struct OrangeSpineCollider {
    u8 pad[6];
    u16 flags;
};
union OrangeSpineSprite {
    struct DisplaySequenceSprite display;
    struct {
        u8 unknown0[0x1C];
        struct SceneVec position;
        u8 unknown24[8];
    } coordinates;
};
struct OrangeSpineActor {
    u8 unknown0[0x18];
    u32 age;
    struct SceneVec initialPosition;
    union OrangeSpineSprite sprite;
    u8 graphics[0x1c];
    struct OrangeSpineCollider collider;
    u8 unknown74[0x4];
    struct SceneVec damagePosition;
    struct SceneVec damageOffset;
    u8 unknown88[0x27];
    u8 direction;
    u16 invulnerability;
    u8 unknownB2[0x56];
    void *update;
    struct DisplaySequenceState sequence;
    u8 unknown11C[0xA];
    s16 health;
    s16 damage;
    s16 hitReaction;
    u8 unknown12C[0x2];
    s16 defeated;
    s16 flashTimer;
    u8 facing;
    u8 unknown133[0x5];
    s16 maxHealth;
    u8 unknown13A[0x6];
    s16 hudStyle;
    u8 unknown142[0x6];
    s16 hudTime;
    u8 unknown14A[0x2];
    void *hud;
    u32 reactionFlags;
    u32 pendingReaction;
    u8 unknown158[0x18];
    void *resource;
};
struct OrangeSpineEffectPair {
    s32 lo : 16;
    u32 hi : 16;
};
struct OrangeSpineEffectVector {
    struct OrangeSpineEffectPair a, b;
};
static inline u32 OrangeSpine_TestEitherFlags(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 OrangeSpine_TestFlag(u32 mask, u32 *p)
{
    return *p & mask;
}
static inline u32 OrangeSpine_RandomSample(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}
static inline u16 OrangeSpine_RandomBurstCount(const u16 *table)
{
    return (OrangeSpine_RandomSample(table) & 3) + 4;
}
static inline u32 OrangeSpine_StepSequence(struct DisplaySequenceSprite *owner,
                                           struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

static inline void OrangeSpine_BeginDefeat(s16 *state, u16 *blink)
{
    u32 zero = 0;
    *state = 1;
    *blink = zero;
}
s32 OrangeSpine_UpdateDamageState(struct OrangeSpineActor *p)
{
    struct OrangeSpineCollider *collider;
    if (OrangeSpine_TestEitherFlags(0x400, &gEngineFlags, &gScenePauseFlags) ||
        OrangeSpine_TestFlag(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.display.flags &= ~1;
    if (p->defeated == 0) {
        struct OrangeSpineCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->sprite.coordinates.position, 0);
        collider = c;
    } else {
        struct OrangeSpineCollider *c = &p->collider;
        c->flags |= 4;
        collider = c;
    }
    if (p->damage > 0 && p->defeated == 0) {
        p->health -= p->damage;
        p->reactionFlags = p->pendingReaction;
        Sound_PlaySong(0x13e);
        if (p->damage > 0) {
            DamageNumberDisplay_Show(collider, p->damage, 0, &p->damagePosition, &p->damageOffset,
                                     1);
            SceneHud_SetTimedState(p->hud, p->health, p->maxHealth, p->hudStyle, p->direction == 4,
                                   p->hudTime);
        }
        if (p->health <= 0) {
            struct SceneVec center, smoke;
            struct OrangeSpineEffectVector velocity, spread;
            const u16 *table;
            OrangeSpine_BeginDefeat(&p->defeated, &p->invulnerability);
            {
                struct SceneVec *centerPosition = &center;
                *centerPosition = p->sprite.coordinates.position;
                centerPosition->y += 128;
            }
            velocity.a.lo = -4;
            table = gRandomTable;
            velocity.a.hi = OrangeSpine_RandomSample(table) & 7;
            velocity.b.lo = -4;
            spread.a.lo = 24;
            spread.a.hi = (OrangeSpine_RandomSample(table) & 7) + 3;
            spread.b.lo = 24;
            sub_08014634(OrangeSpine_RandomBurstCount(table), 13, &center, &velocity, &spread, 15,
                         15);
            {
                struct SceneVec *smokePosition = &smoke;
                *smokePosition = p->sprite.coordinates.position;
                smokePosition->y += 128;
            }
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
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 5);
            Anim_RestartSequence(&p->sequence, p->resource, 16, p->facing, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1c1);
            p->flashTimer = 0;
            p->update = OrangeSpine_UpdateDefeatEffect;
            p->age = 0;
            Sound_PlaySong(0x52d);
        }
        {
            s16 *flash = &p->flashTimer;
            s32 zero = 0;
            *flash = 8;
            p->hitReaction = 1;
            p->damage = zero;
        }
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1c1);
    if (p->invulnerability) {
        p->invulnerability--;
        if ((p->invulnerability >> 2) & 1)
            p->sprite.display.flags |= 1;
        else
            p->sprite.display.flags &= ~1;
    }
    OrangeSpine_StepSequence(&p->sprite.display, &p->sequence);
    p->age++;
}
