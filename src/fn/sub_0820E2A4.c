#include "global.h"
#include "scene_vec.h"

struct PoisonHandSprite {
    u32 flags;
    u8 unk_04[0xc];
    u16 frame;
    u8 unk_12[0xa];
};
struct PoisonHandCollider {
    u8 unk_00[6];
    u16 flags;
    u8 unk_08[4];
    u8 damagePosition[8];
    u8 damageOffset[8];
    u8 unk_1C[0x27];
    u8 direction;
};
struct PoisonHandSequence {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unk_0A, speed, elapsed;
};
struct PoisonHand {
    u8 unk_00[0x18];
    u32 age;
    u8 unk_1C[8];
    struct PoisonHandSprite sprite;
    struct SceneVec position;
    u8 unk_48[8];
    u8 graphics[0x1c];
    struct PoisonHandCollider collider;
    u16 blinkTimer;
    u8 unk_B2[0x56];
    void *update;
    struct PoisonHandSequence sequence;
    u8 unk_11C[0xe];
    s16 flashTimer;
    u8 unk_12C[4];
    s16 hitReaction;
    s16 defeated;
    s16 damage;
    s16 health;
    s16 maxHealth;
    u8 unk_13A[4];
    s16 hudState;
    u8 unk_140[2];
    s16 hudTimer;
    u8 unk_144[8];
    void *hud;
    u32 reaction;
    u32 pendingReaction;
    u8 unk_158[0x10];
    void *animationResource;
};
struct ParticleParameters {
    u32 words[14];
};
extern u32 gUnk_030053F4, gUnk_030053F0, gUnk_02000560, gUnk_03005308;
extern u8 gUnk_0203B000[];
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
s32 PoisonHand_UpdateDefeatBurst(void *);
s32 Mod(s32, s32);
void EnemyParticle_InitParameters(void *);
void EnemyParticle_SetGraphicsParameters(void *, u32, u32, s32);
void EnemyParticle_SetMotionParameters(void *, u16, u16, u8, u32);
void EnemyParticle_SetVerticalMotion(void *, s32, s32);
void EnemyParticle_EnableShadow(void *);
void EnemyParticle_AddFlags(void *, u32);
void EnemyParticle_Emit(void *, void *);

static inline u32 TestPair(u32 mask, u32 *a, u32 *b)
{
    return (*a | *b) & mask;
}
static inline u32 TestFlag(u32 mask, u32 *p)
{
    return *p & mask;
}
static inline u32 RandomFrom(const u16 *table)
{
    table += 512;
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return table[gUnk_03005308];
}
static inline u32 RandomValue(void)
{
    return RandomFrom((const u16 *)gUnk_0203B000);
}
static inline void StepAnimation(struct PoisonHand *p, struct PoisonHandSprite *sprite,
                                 struct PoisonHandSequence *s)
{
    u32 flags, elapsed, duration, mask, savedDuration;
    const volatile u16 *c = s->commands + s->index;
    sprite->frame = *c >> 6;
    {
        u32 input = s->flags, bit = 1;
        u8 first = bit;
        first &= input;
        if (first != (((*c & 0x30) >> 4) & bit))
            p->sprite.flags |= 4;
        else
            p->sprite.flags &= ~4;
    }
    {
        u32 input = s->flags, bit = 2;
        u8 first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*c & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    elapsed = s->elapsed + 1;
    s->elapsed = elapsed;
    mask = 0xffff;
    duration = s->duration;
    elapsed &= mask;
    savedDuration = duration;
    if (elapsed >= duration) {
        s->elapsed = 0;
        flags = s->flags;
        if (flags & 4) {
            u32 i = s->index;
            if (!i) {
                u32 hold = 8;
                hold &= flags;
                if (hold)
                    s->elapsed = savedDuration;
                else
                    s->index = s->frameCount - 1;
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->frameCount;
            if ((i & mask) >= n) {
                u32 hold = 8;
                hold &= flags;
                if ((u8)hold) {
                    s->elapsed = duration;
                    s->index = n - 1;
                } else
                    s->index = 0;
            }
        }
        c = s->commands + s->index;
        s->frameWeight = *c & 15;
        {
            s32 speed = s->frameWeight;
            s32 w = s->speed;
            speed *= w;
            speed >>= 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}
#ifdef NONMATCHING
// fuzzy: 82.4%
// scope shape 37 (differences once low registers are set aside)
s32 PoisonHand_Update(struct PoisonHand *p)
{
    struct PoisonHandCollider *collider;
    if (TestPair(0x400, &gUnk_030053F4, &gUnk_030053F0) || TestFlag(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (p->defeated == 0) {
        struct PoisonHandCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->position, 0);
        collider = c;
    } else {
        struct PoisonHandCollider *c = &p->collider;
        c->flags |= 4;
        collider = c;
    }
    {
        s16 *damage = &p->damage;
        u16 amount = *damage;
        if (*damage > 0 && p->defeated == 0) {
            s16 *health = &p->health;
            *health -= amount;
            p->reaction = p->pendingReaction;
            Sound_PlaySong(0x13e);
            if (*damage > 0) {
                DamageNumberDisplay_Show(collider, *damage, 0, p->collider.damagePosition,
                                         p->collider.damageOffset, 1);
                SceneHud_SetTimedState(p->hud, *health, p->maxHealth, p->hudState,
                                       p->collider.direction == 4, p->hudTimer);
            }
            if (p->health <= 0) {
                struct ParticleParameters parameters;
                const u16 *randomTable;
                s32 i;
                p->defeated = 1;
                p->blinkTimer = 0;
                randomTable = (const u16 *)gUnk_0203B000;
                for (i = 0; i <= 7; i++) {
                    s32 frame = Mod(RandomFrom(randomTable), 2) + 0x1d;
                    u8 direction;
                    EnemyParticle_InitParameters(&parameters);
                    direction = RandomFrom(randomTable);
                    EnemyParticle_SetGraphicsParameters(&parameters, 0x7e7c, frame, 0x1c5);
                    EnemyParticle_SetMotionParameters(&parameters, 0, 60, direction,
                                                      RandomFrom(randomTable) & 15);
                    EnemyParticle_SetVerticalMotion(&parameters, 0x2000, 0x18000);
                    EnemyParticle_EnableShadow(&parameters);
                    if (i & 1)
                        EnemyParticle_AddFlags(&parameters, 0x200);
                    else
                        EnemyParticle_AddFlags(&parameters, 0x400);
                    if (RandomValue() & 1)
                        EnemyParticle_AddFlags(&parameters, 4);
                    if (RandomValue() & 1)
                        EnemyParticle_AddFlags(&parameters, 8);
                    EnemyParticle_Emit(&parameters, &p->position);
                }
                Anim_RestartSequence(&p->sequence, p->animationResource, 0x19, 0, 0);
                AnimData_SelectPaletteBank(p->graphics, 0x1c5);
                p->flashTimer = 0;
                p->update = PoisonHand_UpdateDefeatBurst;
                p->age = 0;
                Sound_PlaySong(0x52f);
            }
            {
                s16 *flash = &p->flashTimer;
                s32 zero = 0;
                *flash = 8;
                p->hitReaction = 1;
                p->damage = zero;
            }
        }
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1c5);
    if (p->blinkTimer) {
        p->blinkTimer--;
        if ((p->blinkTimer >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    StepAnimation(p, &p->sprite, &p->sequence);
    p->age++;
}
#else
INCLUDE_ASM("asm/nonmatching", PoisonHand_Update);
#endif
