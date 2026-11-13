#include "orbit_contact_actor.h"

void OrbitContactActor_UpdateInactive(struct OrbitContactActor *actor)
{
    if (actor->stateFlag) {
        actor->sprite.flags |= 1;
        actor->radius = 0;
        actor->angle = 192;
    }
    ++actor->stateFrame;
}

void OrbitContactActor_UpdateExpanding(struct OrbitContactActor *actor)
{
    if (actor->stateFlag)
        actor->sprite.flags &= ~1;
    actor->angle += 6;
    actor->radius += 7;
    if (Mod(actor->stateFrame, 20) == 8)
        Sound_PlaySongChecked(0x3BC);
    if (actor->stateFrame > 59) {
        actor->state = 2;
        actor->stateFlag = 1;
        actor->stateFrame = 0;
    }
    ++actor->stateFrame;
}

void OrbitContactActor_UpdateContracting(struct OrbitContactActor *actor)
{
    actor->angle += 6;
    actor->radius -= 7;
    if (actor->radius < 0)
        actor->radius = 0;
    if (Mod(actor->stateFrame, 20) == 8)
        Sound_PlaySongChecked(0x3BC);
    if (actor->stateFrame > 59) {
        actor->state = 0;
        actor->stateFlag = 1;
        actor->stateFrame = 0;
    }
    ++actor->stateFrame;
}

extern void (*const gUnk_086059B4[])(struct OrbitContactActor *);

static inline s32 scaleOrbit(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

s32 OrbitContactActor_Update(struct OrbitContactActor *actor)
{
    gUnk_086059B4[actor->state](actor);
    if (actor->state) {
        struct ContactSwarmSprite *sprite = &actor->sprite;
        struct ContactPosition *position = &actor->sprite.position;
        u32 angle = actor->angle;
        s32 radius = actor->radius;
        position->x = scaleOrbit(gUnk_086149C4[(angle + 64) & 255] * radius);
        position->y = 0;
        position->z = scaleOrbit(gUnk_086149C4[angle] * radius);
        {
            struct ContactPosition *destination = &sprite->position;
            const struct ContactPosition *center = &actor->center;
            sprite->position.x += actor->center.x;
            destination->y += center->y;
            destination->z += center->z;
            Collider_SetPosition(&actor->collider, destination, 0);
            Collider_Refresh(&actor->collider);
        }
    }
    return 0;
}

s32 OrbitContactActor_Destroy(struct OrbitContactActor *actor)
{
    Sprite_Remove(&actor->sprite);
    return 0;
}

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

s32 OrbitContactActor_Initialize(struct OrbitContactActor *actor,
                                 const struct ContactPosition *center, u32 resourceValue)
{
    struct ContactSwarmGraphics *graphics;
    struct ContactSwarmSprite *sprite;
    struct ContactPackedVector bounds, offset;
    struct ContactCollider *collider;
    s32 result;
    actor->center = *center;
    graphics = &actor->graphics;
    if (AnimData_Load(graphics, 0x4148)) {
        sprite = &actor->sprite;
        Sprite_Init(sprite, graphics, 1);
        sprite->priority = 2;
        AnimData_SelectPaletteBank(graphics, 0x15F);
        bounds.x = 128;
        bounds.y = 128;
        bounds.z = 128;
        offset.x = 0;
        offset.y = 128;
        offset.z = 0;
        collider = &actor->collider;
        Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
        Collider_SetCallback(collider, NULL, actor);
        Collider_SetAttackProperties(collider, resourceValue, 50, 0, 0x60000, 30);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}

s32 OrbitContactActor_Initialize(struct OrbitContactActor *, const struct ContactPosition *, u32);

s32 OrbitContactActor_Update(struct OrbitContactActor *);

s32 OrbitContactActor_Destroy(struct OrbitContactActor *);

struct OrbitContactActor *OrbitContactActor_Create(const struct ContactPosition *center,
                                                   u32 resourceValue)
{
    struct OrbitContactActor *actor = Task_Create(8, sizeof(*actor));
    if (actor) {
        Task_SetCallbacks(actor, (void (*)(void *))OrbitContactActor_Update,
                          (void (*)(void *))OrbitContactActor_Destroy);
        if (OrbitContactActor_Initialize(actor, center, resourceValue) < 0) {
            Task_Kill(actor);
            return NULL;
        }
    }
    return actor;
}

void OrbitContactActor_BeginExpansion(struct OrbitContactActor *actor)
{
    u32 one = 1;
    u32 zero = 0;
    actor->state = one;
    actor->stateFlag = one;
    actor->stateFrame = zero;
    actor->angle = 192;
    actor->radius = zero;
}
