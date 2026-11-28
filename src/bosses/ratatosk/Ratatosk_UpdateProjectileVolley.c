#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "scene/collision/contact_position.h"

extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void Sound_PlaySong(u32);
u8 Callback_InvokeActor(void *, void *);
s32 Math_Atan2(s32, s32);
static inline s32 Fixed_Truncate12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void Vec_SetPolar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = Fixed_Truncate12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = Fixed_Truncate12(gUnk_086149C4[angle & 255] * radius);
}
static inline void Vec_Add(struct ContactPosition *d, struct ContactPosition *a,
                           struct ContactPosition *b)
{
    d->x = b->x + a->x;
    d->y = a->y + b->y;
    d->z = b->z + a->z;
}
struct ContactPosition *Player_GetEffectivePosition(s32);
void Projectile_InitParameters(void *);
void Projectile_SetLaunchParameters(void *, u16, u32, u8, u32);
void Projectile_SetSpriteParameters(void *, u32, u32, u32);
void Projectile_SetScriptProcedure(void *, u32, u16);
void Projectile_SetCollisionParameters(void *, s32, u32, s32, s32, s32);
void Projectile_SetBehaviorMode(void *, u16);
void Projectile_SpawnAtPosition(void *, struct ContactPosition *);
s32 SceneProjectile_ConsumeHitEvent(void);
// Advance the attack animation and fire three projectiles toward the player.
void Ratatosk_UpdateProjectileVolley(struct Ratatosk *p, s32 tick)
{
    int angleStep;
    u8 params[0x58];
    struct ContactPosition delta;
    struct ContactPosition spawn;
    struct Ratatosk *p2 = p;
    if (p->actionChanged) {
        p->actionChanged = 0;
        Ratatosk_SetFacing(p, p->requestedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 17, 1, 4, 4);
        p->actionCounter = 0;
    }
    switch (p->actionCounter) {
        case 0:
            if (tick > 9) {
                SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 18, 1, 4,
                                                      4);
                p2->actionCounter++;
            }
            break;

        case 1:
            if (tick > 33) {
                SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 19, 1, 4,
                                                      4);
                p2->actionCounter++;
            }
            break;

        case 2:
            if (tick > 36) {
                SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 20, 1, 4,
                                                      4);
                p2->actionCounter++;
            }
            break;

        case 3:
            if (tick > 42) {
                struct ContactPosition *target = Player_GetEffectivePosition(0);
                struct ContactPosition *anchor = &p2->anchor;
                u8 angle = Math_Atan2(target->x - anchor->x, target->z - anchor->z);
                u8 facingAngle = (p->facing << 6) - 64;
                s32 difference = facingAngle - angle;
                s32 i;
                angleStep = 16;
                if (difference > 128)
                    difference = 255 - difference;
                else if (difference < (-128))
                    difference += 255;
                if (difference > 32)
                    angle = facingAngle - 32;
                else if (difference < (-32)) {
                    angle = 32;
                    angle |= facingAngle;
                }
                Vec_SetPolar(&delta, (u8)((p->facing << 6) - 64), 180);
                Vec_Add(&spawn, &delta, &p2->anchor);
                spawn.y += 200;
                angle -= angleStep;
                Sound_PlaySong(0x568);
                for (i = 0; i < 3; i++) {
                    Projectile_InitParameters(params);
                    Projectile_SetLaunchParameters(params, 0, 150, angle, 24);
                    Projectile_SetSpriteParameters(params, 0x8e5a, 0, 498);
                    Projectile_SetScriptProcedure(params, 0x4363, 0);
                    Projectile_SetCollisionParameters(params, 0, 0, 30, 15, 30);
                    Projectile_SetBehaviorMode(params, 1);
                    Projectile_SpawnAtPosition(params, &spawn);
                    angle += angleStep;
                }
                p2->actionCounter++;
            }
            break;

        case 4:
            if (SceneProjectile_ConsumeHitEvent() && (p2->hitTimer == 0)) {
                p2->hitTimer = 120;
                p->actionFinished = 1;
            }
            break;
    }

    if (tick > 119) {
        p2->actionCounter = 0;
        p->actionFinished = 1;
    }
    Callback_InvokeActor(p, p->actionTryUpdate);
}
