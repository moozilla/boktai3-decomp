#include "scene_combat_colliders.h"
#include "game_state.h"

extern struct GameState *gGameState;

/* tentative: 4-state charge attack with fade, collider arm and camera shake */

extern u32 gUnk_0824EE80[];
extern u32 gUnk_0824EE90[];
extern const struct ContactPosition gUnk_0824EEA0;
extern const struct ContactPosition gUnk_0824EEA8;
extern u32 gUnk_0824EEB0[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void Sound_PlaySongChecked(u32 a);
void Armor_StartFastWhiteFade(void *);
void Armor_StartPaletteTransition(void *);
void ActorPalette_Reset(void *);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Camera_RequestShake(s32);
s32 Math_Atan2(s32, s32);

void ActorController_UpdateChargeAttackTransition(struct SceneCombatActor *actor)
{
    u8 *p = (void *)actor;
    u8 *s = &actor->actionPhase;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(actor, &gUnk_0824EE80[((actor->heading + 0x60) >> 6) & 3]);
            (*s)++;
            actor->actionReady = st;
            ((struct ContactCollider *)(p + 0x238))->flags |= four;
            ((struct ContactCollider *)(p + 0x1E4))->flags &= ~four;
            ((struct ContactCollider *)(p + 0x28C))->flags &= ~four;
            ((struct ContactCollider *)(p + 0x190))->flags |= four;
            {
                struct ActorControllerMotion *render = (void *)(p + 0x180);
                u32 one = 1;
                render->flags |= one;
            }
            *(u16 *)(p + 0xB32) &= ~0x20;
            Sound_PlaySongChecked(0xC1);
            break;
        }
        case 1:
            if (actor->animationComplete != 0) {
                struct ContactCollider *c;
                Armor_SetAnimationFromStart(actor,
                                            &gUnk_0824EE90[((actor->heading + 0x60) >> 6) & 3]);
                {
                    u32 next = *s + 1;
                    u32 zero = 0;
                    *s = next;
                    actor->actionReady = zero;
                    actor->actionCounter = zero;
                }
                Armor_StartFastWhiteFade(p);
                actor->shakeStrength = 20;
                *(u16 *)(p + 0x196) &= ~4;
                c = (struct ContactCollider *)(p + 0x190);
                {
                    u32 mask = 0x40000;
                    c->reactionFlags |= mask;
                }
                Collider_SetAttackPowerAndForce(c, 0, 0x50);
                c->bounds = gUnk_0824EEA8;
                c->offset = gUnk_0824EEA0;
                p[0x1D2] = Math_Atan2(gGameState->positionX - *(s16 *)(p + 0x48),
                                      gGameState->positionZ - *(s16 *)(p + 0x4C));
                Sound_PlaySongChecked(0x1BA);
            }
            break;
        case 2:
            if (actor->actionCounter > 7) {
                Armor_SetAnimationFromStart(actor,
                                            &gUnk_0824EEB0[((actor->heading + 0x60) >> 6) & 3]);
                {
                    u32 next = *s + 1;
                    u32 zero = 0;
                    *s = next;
                    actor->actionReady = zero;
                }
                Armor_StartPaletteTransition(p);
                {
                    u16 *flags = (u16 *)(p + 0x196);
                    u32 old = *flags;
                    u32 set = 4;
                    set |= old;
                    *flags = set;
                }
            }
            break;
        case 3:
            if (actor->animationComplete != 0) {
                *s = 0xFF;
                actor->actionReady = 1;
                ActorPalette_Reset(p);
            }
            break;
    }
    if (actor->shakeStrength != 0) {
        Camera_RequestShake(0x80);
        actor->shakeStrength--;
    }
}
