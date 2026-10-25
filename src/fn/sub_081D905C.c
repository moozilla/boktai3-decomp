// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct HomingPosition {
    s16 x, y, z, unk_06;
};
struct HomingConfig {
    u16 flags, duration;
    u8 unk_04[2];
    s8 direction;
    u32 turnSpeed : 8;
    s32 speed;
};
struct HomingDisplay {
    u8 unk_00[0x1C];
    struct HomingPosition position;
};
struct HomingEffect {
    u32 timer, flags;
    u8 unk_08[0x18 - 8];
    struct HomingConfig config;
    u8 unk_24[0x8C - 0x24];
    struct HomingDisplay display;
    u8 unk_B0[0x160 - 0xB0];
    struct HomingPosition *target;
};
struct HomingCollider {
    struct HomingCollider *next;
    u8 unk_04[2];
    u16 flags;
    u8 unk_08[4];
    struct HomingPosition position;
};
struct HomingManager {
    u8 unk_00[0x18];
    struct HomingCollider *list;
    u8 unk_1C[0xD0 - 0x1C];
    u16 count;
};
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
struct HomingManager *Collider_GetManager(void);

static inline s32 HomingEffect_Square(s32 value)
{
    return value * value;
}
static inline s32 HomingEffect_Distance(struct HomingPosition *a, struct HomingPosition *b)
{
    s32 x = a->x - b->x;
    s32 z = a->z - b->z;
    return HomingEffect_Square(x) + HomingEffect_Square(z);
}

u32 TimedEffect_UpdateHoming(struct HomingEffect *effect)
{
    struct HomingDisplay *display = &effect->display;
    struct HomingConfig *config = &effect->config;
    s32 nearestDistance = 0x7FFFFFFF;
    if (Mod(effect->timer, 30) <= 3) {
        struct HomingManager *manager = Collider_GetManager();
        struct HomingCollider *collider;
        s32 count;
        if (manager == NULL) {
            u32 two = 2;
            effect->flags |= two;
            if (effect->timer >= config->duration)
                goto expired;
            goto active;
        }
        count = manager->count;
        collider = manager->list;
        collider = collider->next;
        if (collider == NULL) {
            u32 two = 2;
            effect->flags |= two;
            if (effect->timer >= config->duration)
                goto expired;
            goto active;
        }
        if (count > 0) {
            struct HomingPosition *nearest = NULL;
            s32 index;
            for (index = 0; index < count; ++index) {
                struct HomingPosition *origin = &effect->display.position;
                if ((collider->flags & 12) == 8) {
                    register struct HomingPosition *position asm("r4") = &collider->position;
                    s32 x = collider->position.x - display->position.x;
                    s32 z = position->z - origin->z;
                    s32 distance = x * x + z * z;
                    s32 angle = (u8)Math_Atan2(x, z);
                    s32 delta = angle - config->direction;
                    if (delta > 128)
                        delta -= 256;
                    else if (delta <= -128)
                        delta += 256;
                    if (distance < nearestDistance) {
                        s32 absolute = delta;
                        if (absolute < 0)
                            absolute = -absolute;
                        if (absolute < 32) {
                            nearestDistance = distance;
                            nearest = position;
                        }
                    }
                }
                collider = collider->next;
            }
            if (nearest != NULL) {
                struct HomingPosition *target = effect->target;
                if (target == NULL)
                    effect->target = nearest;
                else {
                    if (HomingEffect_Distance(nearest, &display->position) <
                        HomingEffect_Distance(target, &display->position))
                        effect->target = nearest;
                }
            }
        }
    }
    if (effect->target != NULL && (effect->timer & 1)) {
        u8 angle = Math_Atan2(effect->target->x - display->position.x,
                              effect->target->z - display->position.z);
        u32 current = (u8)config->direction;
        s32 signedAngle = (s8)angle;
        s32 delta = (s8)(signedAngle - current);
        s32 limit = (s8)config->turnSpeed;
        if (delta > limit) {
            u32 direction = current + config->turnSpeed;
            config->direction = direction;
        } else if (delta < limit)
            config->direction = current - config->turnSpeed;
        else
            config->direction = angle;
        if (Mod(effect->timer, 10) == 1)
            config->speed += 2;
        {
            u32 one = 1;
            effect->flags |= one;
        }
    }
    {
        u32 two = 2;
        effect->flags |= two;
        if (effect->timer >= config->duration)
            goto expired;
        goto active;
    }
active:
    return 0;
expired:
    return 2;
}
