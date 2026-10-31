#include "global.h"
#include "engine.h"
#include "ram.h"
#include "scene_vec.h"

extern u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];

struct CloneParams {
    u16 unknown0, lifetime, unknown4;
    s8 direction;
    u8 unknown7;
    s32 speed;
    void *callback;
    u8 unknown10[0x10];
};

struct CloneCollider {
    u8 unknown0[6];
    s16 flags : 16;
    u8 unknown8[0x3C];
    u16 timer;
    u8 unknown46[0xE];
};
struct CloneBlinkDrawable {
    u32 flags;
    u8 unknown4[2];
    u16 resetValue;
    u8 unknown8[0x10];
    struct SceneVec position;
};
struct CloneAnim {
    u8 unknown0[8];
    struct CloneBlinkDrawable drawable;
    u8 unknown28[0x12];
    u16 paletteFlags : 16;
    u8 unknown3C[0xC];
    void *palette;
    u8 unknown4C[0x74];
};
struct CloneState {
    u32 flags, phase;
    u8 unknown30C[0x10];
    s32 animation;
    u8 foldedFacing, facing, mirrored, particleCount;
    s32 speed;
    u32 index;
    u8 unknown32C[4];
    s16 paletteTimer, attackTimer;
    s16 attackPending, attack, element, power;
    u32 debris;
    u16 blinkTimer, unknown342;
};
struct PlayerClone {
    u32 frame;
    u8 unknown4[8];
    struct SceneVec velocity;
    struct SceneVec particlePosition;
    struct SceneVec origin;
    struct SceneVec initialPosition; /* entrance vector origin, tentative */
    struct CloneCollider contact, body, area, playerAttack;
    u8 unknown17C[0x1C];
    struct CloneParams params;
    struct CloneAnim anim;
    u8 resource[0x20];
    u8 shadow[0x6C];
    struct CloneState state;
};
struct PlayerClonePool {
    u8 task[0x18];
    u32 activeMask;
    struct PlayerClone entries[3];
    s32 activeCount;
    s16 palette1[16], palette2[16];
};

static inline struct CloneBlinkDrawable *CloneBlinkDrawable_FromHeight(s16 *height)
{
    return (void *)((u8 *)height - (u32) & ((struct CloneBlinkDrawable *)0)->position.y);
}

void ScaledShadow_Enable(void *);
void ScaledShadow_Disable(void *);
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_SelectPaletteBank(void *, u32);
void ActorDebris_Show(u32);
void PlayerEffect_SetPaletteSource(u32, u32, u32);
struct SceneVec *Player_GetEffectivePosition(s32);
struct SceneVec *Ratatosk_GetPosition(void);
void SlotMotion_Initialize(u8 *);
void Ratatosk_ConfigureParticle(u8 *, u32, u32, u32, u32, u32);
int SceneParticles_SpawnAtPosition(u8 *, void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);

static inline s32 Clone_ScaleCoordinate(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

static inline void Clone_CreateAngleVector(struct SceneVec *v, s32 angle, s32 speed)
{
    v->x = Clone_ScaleCoordinate(gUnk_086149C4[(angle + 0x40) & 0xff] * speed);
    v->y = 0;
    v->z = Clone_ScaleCoordinate(gUnk_086149C4[angle & 0xff] * speed);
}

static inline void Clone_AddVectors(struct SceneVec *d, struct SceneVec *a, struct SceneVec *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}

static inline void Clone_SubtractVectors(struct SceneVec *d, struct SceneVec *a, struct SceneVec *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

static inline s32 Clone_NextRandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return *(u8 *)(gRandomIndex * 2 + RANDOM_TABLE_ADDR);
}

static inline u16 Clone_NextRandomAngle(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

static inline void Clone_EnableHiddenFlag(struct CloneAnim *animation)
{
    animation->drawable.flags |= 1;
}
static inline void Clone_DisableHiddenFlag(struct CloneAnim *animation)
{
    animation->drawable.flags &= ~1;
}
static inline void Clone_EnableRiseFlag(struct CloneAnim *animation)
{
    animation->drawable.flags |= 0x400;
}
static inline void Clone_DisableRiseFlag(struct CloneAnim *animation)
{
    animation->drawable.flags &= ~0x400;
}

static inline void Clone_SetFlags(u32 mask, u32 *flags)
{
    *flags |= mask;
}

static inline struct CloneAnim *Clone_GetAnimation(struct PlayerClone *clone)
{
    return &clone->anim;
}

static inline void Clone_SetRiseHeight(struct PlayerClone *clone, s16 height)
{
    clone->anim.drawable.position.y = height;
}
static inline void Clone_ClearVector(const void *value, void *destination, u32 control)
{
    CpuSet(value, destination, control);
}
static inline u32 Clone_UpdateBlink(struct CloneBlinkDrawable *animation, struct PlayerClone *s)
{
    u16 *timer = &s->state.blinkTimer;
    if (Sprite_BlinkUntilExpired(animation, (s16)*timer, 0x2d, 0x5a))
        return 1;
    (*timer)++;
    return 0;
}
#define CLONE_DISABLE_COLLIDER(collider)                                                           \
    do {                                                                                           \
        (collider)->flags |= 4;                                                                    \
    } while (0)

static inline void Clone_MaskColliderFlags(u32 mask, struct CloneCollider *collider)
{
    collider->flags &= mask;
}
s32 PlayerClone_UpdateBehavior(struct PlayerClone *clone, s32 index)
{
    struct CloneParams *params = &clone->params;
    vu32 zero;
    u8 particleMotion[0x18];
    struct SceneVec offset;
    struct SceneVec delta;
    struct SceneVec particlePosition;
    switch (clone->state.phase) {
        case 0: {
            struct CloneCollider *body = &clone->body;
            s32 direction;
            struct SceneVec *initialPosition;
            CLONE_DISABLE_COLLIDER(body);
            ScaledShadow_Enable(clone->shadow);
            Clone_EnableHiddenFlag(&clone->anim);
            direction = params->direction;
            Clone_CreateAngleVector(&offset, direction + 0x80, Div(clone->frame << 9, 30));
            {
                struct SceneVec *destination = &clone->anim.drawable.position;
                initialPosition = &clone->initialPosition;
                Clone_AddVectors(destination, &offset, initialPosition);
            }
            if (clone->frame > 30) {
                Clone_DisableHiddenFlag(&clone->anim);
                Clone_MaskColliderFlags(~4, body);
                clone->state.phase = 1;
                clone->state.flags &= ~1;
                {
                    void *destination = &clone->velocity;
                    zero = 0;
                    Clone_ClearVector((void *)(&zero), destination, 0x05000002);
                }
            }
            break;
        }

        case 1: {
            struct CloneAnim *display;
            if (clone->frame & 1)
                SpriteParticle_CreateDefaultVariant4384(&clone->particlePosition, 0x28, 0x20, 0xc,
                                                        4, 0x18, Clone_NextRandomByte(), 0x20, 0x20,
                                                        3, 2, 0x200);
            display = Clone_GetAnimation(clone);
            Clone_EnableRiseFlag(display);
            {
                s16 height = clone->origin.y - ((60 - clone->frame) * 2);
                Clone_SetRiseHeight(clone, height);
            }
            if (clone->frame > 60) {
                clone->state.phase = 2;
                Clone_DisableRiseFlag(display);
                ScaledShadow_Disable(clone->shadow);
            }
            break;
        }
        case 2:
            if (clone->frame > 0x120)
                clone->state.phase = 3;
            break;

        case 3:
            if (Mod(clone->frame, 120) == 0) {
                params->direction = Clone_NextRandomAngle();
                Clone_SetFlags(1, &clone->state.flags);
                clone->state.animation = 5;
                Anim_LoadKeyframe(&clone->anim, clone->resource, 5, 1);
            }
            if (clone->frame > ((index * 30) + 0x294)) {
                u32 z;
                clone->origin = clone->anim.drawable.position;
                {
                    u16 x = (u16)clone->anim.drawable.position.x;
                    z = 0;
                    clone->particlePosition.x = x;
                }
                clone->particlePosition.y = ((u16)clone->anim.drawable.position.y) + 0x80;
                clone->particlePosition.z = (u16)clone->anim.drawable.position.z;
                clone->state.phase = 5;
                params->speed = z;
                Clone_SetFlags(1, &clone->state.flags);
                {
                    struct CloneCollider *body = &clone->body;
                    CLONE_DISABLE_COLLIDER(body);
                }
                clone->state.blinkTimer = z;
                clone->state.animation = z;
                Anim_LoadKeyframe(&clone->anim, clone->resource, 0, 1);
            } else {
                struct SceneVec *playerPosition = Player_GetEffectivePosition(0);
                struct SceneVec *pos = &clone->anim.drawable.position;
                s32 dx = playerPosition->x - pos->x;
                s32 dz = playerPosition->z - pos->z;
                if (((dx * dx) + (dz * dz)) <= 0xffff) {
                    struct CloneAnim *animation;
                    u8 *resource;
                    struct PlayerClonePool *pool;
                    s16 *timer = &clone->state.attackTimer;
                    s32 z = 0;
                    *timer = z;
                    params->speed = z;
                    Clone_SetFlags(1, &clone->state.flags);
                    {
                        struct SceneVec *target = Player_GetEffectivePosition(0);
                        params->direction = Math_Atan2(target->x - pos->x, target->z - pos->z);
                    }
                    clone->state.animation = 0x54;
                    animation = &clone->anim;
                    resource = clone->resource;
                    Anim_LoadKeyframe(animation, resource, 0x54, 2);
                    Anim_SetKeyframes(animation, resource,
                                      (u16)(clone->state.animation + clone->state.foldedFacing), 2,
                                      4);
                    clone->anim.drawable.resetValue = z;
                    pool = (void *)gDirectionalSpritePool;
                    if (!pool) {
                        /* Animation setup cannot finish without the drawable pool. */
                        return;
                    }
                    Anim_SelectPaletteBank(animation, 0x1f4);
                    clone->anim.paletteFlags = 0xe000;
                    clone->anim.palette = pool->palette2;
                    ActorDebris_Show(clone->state.debris);
                    PlayerEffect_SetPaletteSource(clone->state.debris, 0xe001, 0);
                    clone->state.phase = 4;
                } else {
                    s32 r;
                    if (Clone_NextRandomByte() < 4) {
                        if (clone->state.particleCount == 0)
                            clone->state.particleCount = 10;
                    }
                    if (clone->state.particleCount == 0)
                        break;
                    r = Mod(clone->frame, 0x20);
                    Clone_SubtractVectors(&delta, Ratatosk_GetPosition(),
                                          &clone->anim.drawable.position);
                    if (r == 0) {
                        zero = r;
                        Clone_ClearVector((void *)(&zero), &delta, 0x05000002);
                    } else {
                        delta.x = Div(delta.x * r, 0x20);
                        delta.y = 0;
                        delta.z = Div(delta.z * r, 0x20);
                    }
                    Clone_AddVectors(&particlePosition, &delta, &clone->anim.drawable.position);
                    particlePosition.y += 0x60;
                    SlotMotion_Initialize(particleMotion);
                    Ratatosk_ConfigureParticle(particleMotion, 10, 0, 0, 0, 0);
                    SceneParticles_SpawnAtPosition(particleMotion, &particlePosition);
                    clone->state.particleCount--;
                }
            }
            break;

        case 4: {
            s16 *attackTimer = &clone->state.attackTimer;
            if ((*attackTimer) > 5) {
                struct CloneCollider *area = &clone->area;
                Clone_MaskColliderFlags(~4, area);
            }
            if ((*attackTimer) > 30) {
                clone->state.phase = 3;
                params->direction = Clone_NextRandomAngle();
                params->speed = clone->state.speed;
                Clone_SetFlags(1, &clone->state.flags);
                clone->state.animation = 5;
                Anim_LoadKeyframe(&clone->anim, clone->resource, 5, 1);
            }
            (*attackTimer)++;
            break;
        }

        case 5: {
            s16 *height;
            if (clone->frame & 1)
                SpriteParticle_CreateDefaultVariant4384(&clone->particlePosition, 0x28, 0x20, 0xc,
                                                        4, 0x18, Clone_NextRandomByte(), 0x20, 0x20,
                                                        3, 2, 0x200);
            {
                s32 delay = index * 30;
                s32 elapsed = clone->frame - 0x2b2;
                s32 remaining = delay - elapsed;
                s16 y = clone->origin.y - ((30 - remaining) * 2);
                height = &clone->anim.drawable.position.y;
                *height = y;
            }
            return Clone_UpdateBlink(CloneBlinkDrawable_FromHeight(height), clone);
        }
    }

    return 0;
}
