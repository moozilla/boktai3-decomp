#include "contact_collider_manager.h"
#include "task4e01_projectile.h"

s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);

static inline s32 HomingEffect_Square(s32 value)
{
    return value * value;
}
static inline s32 HomingEffect_Distance(struct ContactPosition *a, struct ContactPosition *b)
{
    s32 x = a->x - b->x;
    s32 z = a->z - b->z;
    return HomingEffect_Square(x) + HomingEffect_Square(z);
}

u32 TimedEffect_UpdateHoming(struct Task4E01Projectile *effect)
{
    struct ContactSwarmSprite *display = &effect->sprite;
    struct Task4E01Params *config = &effect->params;
    union {
        struct ContactColliderManager *manager;
        struct ContactPosition *position;
    } scan;
    s32 nearestDistance = 0x7FFFFFFF;
    if (Mod(effect->frame, 30) <= 3) {
        struct ContactCollider *collider;
        s32 count;
        scan.manager = Collider_GetManager();
        if (scan.manager == NULL) {
            u32 two = 2;
            effect->flags |= two;
            if (effect->frame >= config->lifetime)
                goto expired;
            goto active;
        }
        count = scan.manager->sourceCount;
        collider = scan.manager->source;
        collider = collider->next;
        if (collider == NULL) {
            u32 two = 2;
            effect->flags |= two;
            if (effect->frame >= config->lifetime)
                goto expired;
            goto active;
        }
        if (count > 0) {
            struct ContactPosition *nearest = NULL;
            s32 index;
            for (index = 0; index < count; ++index) {
                struct ContactPosition *origin = &display->position;
                if ((collider->flags & 12) == 8) {
                    s32 x, z;
                    s32 distance;
                    s32 angle;
                    s32 delta;
                    scan.position = &collider->position;
                    x = collider->position.x - display->position.x;
                    z = scan.position->z - origin->z;
                    distance = x * x + z * z;
                    angle = (u8)Math_Atan2(x, z);
                    delta = angle - config->direction;
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
                            nearest = scan.position;
                        }
                    }
                }
                collider = collider->next;
            }
            if (nearest != NULL) {
                struct ContactPosition *target = effect->target;
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
    if (effect->target != NULL && (effect->frame & 1)) {
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
        if (Mod(effect->frame, 10) == 1)
            config->speed += 2;
        {
            u32 one = 1;
            effect->flags |= one;
        }
    }
    {
        u32 two = 2;
        effect->flags |= two;
        if (effect->frame >= config->lifetime)
            goto expired;
        goto active;
    }
active:
    return 0;
expired:
    return 2;
}
