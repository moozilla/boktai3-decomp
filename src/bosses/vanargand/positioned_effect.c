#include "bosses/vanargand/positioned_effect.h"
#include "objects/scene_object.h"
#include "engine/input.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "bosses/vanargand/vanargand_parts.h"

/* Empty callback registered by the adjacent effect constructors. */

void PositionedEffect_OnAttackContact(void)
{
}

void Collision_CalculateDamageResponse(void *, void *);

void PlayerStats_AddContactHundredths(void *, void *, u32);

void PositionedEffect_ReceiveContact(struct ContactCollider *source,
                                     struct ContactCollider *contact,
                                     struct VanargandPairAttack *actor)
{
    u32 flags;
    Collision_CalculateDamageResponse(source, contact);
    actor->damage = contact->cost;
    flags = source->reactionFlags;
    actor->hitFlags = flags;
    actor->hitDuration = source->responseValue;
    PlayerStats_AddContactHundredths(source, contact, 10);
}

void Collider_SetBodyProperties(void *, s32, s32, s32);

void PositionedEffect_InitContactCollider(struct VanargandPairAttack *actor)
{
    struct PositionedPackedVector size, offset;
    struct ContactCollider *collider = &actor->damageSource;

    size.x = 256;
    size.y = 256;
    size.z = 256;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &actor->animation.position, 0);
    Collider_SetCallback(collider, PositionedEffect_ReceiveContact, actor);
    Collider_SetBodyProperties(collider, 10, 2048, 0);
    {
        u8 direction = 32;
        actor->damageSource.direction = direction;
    }
    Collider_Refresh(collider);
}

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);

void PositionedEffect_InitAttackCollider(struct VanargandPairAttack *actor)
{
    struct PositionedPackedVector size, offset;
    struct ContactCollider *collider = &actor->attackCollider;

    size.x = 128;
    size.y = 256;
    size.z = 128;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &actor->position, 0);
    Collider_SetCallback(collider, PositionedEffect_OnAttackContact, actor);
    Collider_SetAttackProperties(collider, actor->parameter584, 80, 0, 0, 80);
}

extern const s16 gUnk_086149C4[];

void PositionedEffect_InitRadialAttackColliders(struct VanargandPairAttack *actor)
{
    struct PositionedPackedVector size;
    union PositionedColliderOffset offset;
    struct ContactCollider *collider;
    s32 i;

    size.x = 128;
    size.y = 128;
    size.z = 128;
    offset.bits.y = 0;
    collider = actor->radialColliders;
    for (i = 0; i < 2; i++) {
        Vanargand_MakeRadialVector(&offset.vector, (240 + i * 96) & 255, 416);
        Collider_Init(collider, 0, 0x2001, 0, 16, &size, &offset);
        Collider_SetCallback(collider, PositionedEffect_OnAttackContact, actor);
        Collider_SetAttackProperties(collider, actor->parameter584, 80, 0, 0, 30);
        collider++;
    }
}

s32 SceneParticle_CreateParticles(const void *anchor, const struct PositionedParticleOffset *offset,
                                  const void *range, const void *jitter, s32 count, u32 priority);

/* Emit four particles around the actor with random horizontal offsets. */

static inline void ClearWords(const void *src, void *dst, u32 control)
{
    CpuSet(src, dst, control);
}

void PositionedEffect_EmitParticles(struct VanargandPairAttack *actor)
{
    struct SceneBody *body = (struct SceneBody *)actor->bodyHeader;
    struct PositionedParticleOffset center;
    struct PositionedParticleOffset offset;
    u32 fill;
    s32 i;
    u32 *src = &fill;
    struct PositionedParticleOffset *dst = &center;
    fill = 0;
    ClearWords(src, dst, 0x05000002);
    for (i = 0; i < 4; i++) {
        const u16 *table = gRandomTable;
        u16 index = (gRandomIndex + 1) & 0x3FF;
        u16 random = table[index];
        offset.x = ((random & 0xFF) + center.x) - 0x80;
        offset.y = center.y;
        index = (index + 1) & 0x3FF;
        gRandomIndex = index;
        {
            u16 random = table[index];
            offset.z = ((random & 0xFF) + center.z) - 0x80;
        }
        table = 0;
        SceneParticle_CreateParticles(&body->position, &offset, table, table, 1, 1);
    }
}

/* Vanargand_RunUnusedHook: Empty routine near Vanargand effect helpers with no known callers or observable behavior */

void Vanargand_RunUnusedHook(void)
{
}

static inline void Unproject(struct ContactPosition *out, s16 *in, s32 height)
{
    s32 x = in[0];
    s32 y = in[1];
    out->x = MonsterInit_Half(((x + y) << 5) / 3 + height);
    out->y = height;
    out->z = MonsterInit_Half(((y - x) << 5) / 3 + height);
}

void Debug_TestIsometricProjection(void)
{
    s16 in[3];
    struct ContactPosition out;
    if (gPadState[1] & 0x100) {
        s32 height = 0;
        s16 *v = in;
        s32 screenY = 1;
        in[0] = height;
        v[1] = screenY;
        v[2] = height;
        Unproject(&out, in, height);
    }
}

void BgGfx_LoadTilemapAtPosition(u32, s32, s32);

static inline s32 Shift5(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 5;
    else
        r = -((-x) >> 5);
    return r;
}

static inline void Project(struct ContactPosition *out, struct ContactPosition *v)
{
    s32 s, h;
    s32 x, z;
    x = v->x;
    z = v->z;
    out->x = Shift5((x - z) * 3);
    s = Shift5((x + z) * 3);
    h = Shift5(v->y * 3);
    out->y = s - h;
    out->z = s + h;
}

void ScreenPalette_ProjectPosition(u32 layer, struct ContactPosition *pos, s32 xOff, s32 yOff)
{
    struct ContactPosition d;
    Project(&d, pos);
    d.x = gCameraOrigin[0] - d.x;
    d.y = gCameraOrigin[1] - d.y;
    d.z = d.z - gCameraOrigin[2];
    BgGfx_LoadTilemapAtPosition(layer, d.x + xOff, d.y + yOff);
}

void ScreenPalette_ProjectPositionAt88(struct PositionedEffectCoordinates *obj)
{
    struct ContactPosition d;
    Project(&d, &obj->position);
    d.x = gCameraOrigin[0] - d.x;
    d.y = gCameraOrigin[1] - d.y;
    d.z = d.z - gCameraOrigin[2];
    BgGfx_LoadTilemapAtPosition(2, d.x + 0x88, d.y + 0x6c);
}

static inline void AddVector(struct ContactPosition *p, struct ContactPosition *a,
                             struct ContactPosition *b)
{
    p->x = a->x + b->x;
    p->y = a->y + b->y;
    p->z = a->z + b->z;
}

static inline void UpdateAnimation(struct PositionedEffectAnimation *a,
                                   struct PositionedVectorEffect *p)
{
    AddVector(&a->sprite.position, a->offset, &p->base);
}

void PositionedEffect_UpdateVectors(struct PositionedVectorEffect *p)
{
    p->position = p->source;
    Vanargand_SetVector(&p->shifted, p->source.x - 160, p->source.y, p->source.z - 160);
    UpdateAnimation(&p->animation, p);
}

s32 __divsi3(s32, s32);

void PositionedEffect_InterpolateArcNodes(struct PositionedArc *effect)
{
    struct ContactPosition diff;
    struct ContactSwarmSprite *node;
    s32 i;

    Vanargand_Difference(&diff, effect->end, effect->start);
    node = effect->nodes;
    for (i = 0; i <= 5; i++) {
        Vanargand_SetVector(&node->position, effect->start->x + diff.x * i / 5,
                            effect->start->y +
                                ((diff.y * gUnk_086149C4[((i << 6) / 5) & 255]) >> 12),
                            effect->start->z + diff.z * i / 5);
        node->flags &= ~1;
        node++;
    }
}

void PositionedEffect_SetHalfScaleCoordinates(struct PositionedEffectCoordinates *out,
                                              struct ContactPosition *source)
{
    out->position.x = MonsterInit_Half(source->x - 640) + 512;
    out->position.z = MonsterInit_Half(source->z - 640) + 512;
}

void PositionedEffect_UpdateOscillatingCoordinates(struct VanargandPairAttack *effect, s32 phase)
{
    struct PositionedEffectCoordinates *coordinates = &effect->coordinates;
    PositionedEffect_SetHalfScaleCoordinates(coordinates, &effect->effectOrigin);
    coordinates->position.y =
        Vanargand_TruncateScale(gUnk_086149C4[(effect->attackParameter5B8 * phase) & 255]
                                << effect->attackParameter5BA) +
        256;
    Vanargand_SetVector(&coordinates->shifted, coordinates->position.x - 256,
                        coordinates->position.y, coordinates->position.z - 256);
}
