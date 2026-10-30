#include "global.h"
#include "display_sequence_inline.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
extern u32 gUnk_02000560;
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void sub_082086B8(void);
void CannonTurret_FireAimedVolley(void);
void Anim_SetDurationScale(void *, u16);
static inline void CopyPosition(struct SceneVec *out, const struct SceneVec *in)
{
    *out = *in;
}
static inline u32 RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}
static inline u32 Paused(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
struct ColliderView {
    u8 pad[6];
    u16 flags;
};
union CannonTurretSprite {
    struct DisplaySequenceSprite display;
    struct {
        u8 unknown0[0x1c];
        struct SceneVec position;
        u8 unknown24[8];
    } coordinates;
};
struct CannonTurretCombat {
    u8 unknown0[0x18];
    u32 age;
    u8 unknown1C[0x8];
    union CannonTurretSprite sprite;
    u8 graphics[0x1c];
    struct ColliderView collider;
    u8 unknown74[0x4];
    struct SceneVec damagePosition;
    struct SceneVec damageOffset;
    u8 unknown88[0x27];
    u8 direction;
    u16 invulnerability;
    u8 unknownB2[0x56];
    void (*update)(void);
    struct DisplaySequenceState sequence;
    u8 unknown11C[0xC];
    s16 flashTimer;
    u8 unknown12A[0x2];
    s16 damage;
    s16 hitReaction;
    s16 defeated;
    s16 health;
    s16 activeAnimation;
    s16 maxHealth;
    u8 unknown138[0x4];
    s16 hudStyle;
    u8 unknown13E[0x6];
    s16 hudTime;
    u8 unknown146[0x2];
    void *hud;
    u32 reactionFlags;
    u32 pendingReaction;
    u8 unknown154[0x10];
    void *resource;
};
struct EffectVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline void CannonTurret_FinishDamage(struct CannonTurretCombat *p)
{
    s16 *flash = &p->flashTimer;
    u32 zero = 0;
    *flash = 8;
    p->hitReaction = 1;
    p->damage = zero;
}
static inline void CannonTurret_ClearBlink(u16 *timer)
{
    u32 zero = 0;
    *timer = zero;
}
static inline u32 CannonTurret_StepSequence(struct DisplaySequenceSprite *owner,
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

s32 CannonTurret_UpdateCombat(struct CannonTurretCombat *p)
{
    struct SceneVec position;
    struct EffectVector velocity;
    struct EffectVector size;
    struct ColliderView *collider;
    u32 ended;
    if (Paused(0x400))
        goto paused;
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask) {
        paused:
            return 0;
        }
    }
    Callback_InvokeActor(p, p->update);
    p->sprite.display.flags &= ~1;
    {
        struct ColliderView *c;
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
            DamageNumberDisplay_Show(collider, p->damage, 0, &p->damagePosition, &p->damageOffset,
                                     1);
            SceneHud_SetTimedState(p->hud, p->health, p->maxHealth, p->hudStyle, p->direction == 4,
                                   p->hudTime);
        }
        if (p->health <= 0) {
            const u16 *table;
            s32 count;
            p->defeated = 1;
            CannonTurret_ClearBlink(&p->invulnerability);
            CopyPosition(&position, &p->sprite.coordinates.position);
            {
                struct SceneVec *v = &position;
                v->y += 200;
            }
            velocity.x = -4;
            table = gRandomTable;
            velocity.y = RandomFrom(table) & 7;
            velocity.z = -4;
            size.x = 24;
            size.y = (RandomFrom(table) & 7) + 3;
            size.z = 24;
            count = (RandomFrom(table) & 3) + 4;
            sub_08014634(count, 2, &position, &velocity, &size, 15, 15);
            Anim_RestartSequence(&p->sequence, p->resource, 10, 0, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1bc);
            p->flashTimer = 0;
            p->update = sub_082086B8;
            p->age = 0;
            Sound_PlaySong(0x52c);
        }
        CannonTurret_FinishDamage(p);
    }
    if (((Mod(p->age, 240) == 0) && (p->defeated == 0)) &&
        (p->update != CannonTurret_FireAimedVolley)) {
        Anim_SetSequence(&p->sequence, p->resource, 9, 0, 8);
        Anim_SetDurationScale(&p->sequence, 32);
        p->activeAnimation = 1;
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1bc);
    if (p->invulnerability) {
        p->invulnerability--;
        if ((p->invulnerability >> 2) & 1)
            p->sprite.display.flags |= 1;
        else
            p->sprite.display.flags &= ~1;
    }

    ended = CannonTurret_StepSequence(&p->sprite.display, &p->sequence);

    if ((ended && (p->activeAnimation != 0)) && (p->defeated == 0)) {
        Anim_SetSequence(&p->sequence, p->resource, 8, 0, 0);
        p->activeAnimation = 0;
    }
    p->age++;
    /* The task callback has no explicit return assignment on the active path. */
}
