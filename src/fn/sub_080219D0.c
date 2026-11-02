#include "game_state.h"
#include "state_actor.h"
void Camera_RequestShake(int);
void SceneActors_SpawnAmbientParticle(void *);
void SceneActors_UpdateEffects(struct StateActor *p)
{
    struct StateActorDamageEffect *damage;
    if (p->health > 0)
        gGameState->unk618 = p->health;
    else
        gGameState->unk618 = 0;
    if (p->shakeFrames) {
        Camera_RequestShake(p->shakeStrength);
        p->shakeFrames--;
    }
    if (p->ambientParticle && (p->effectTimer & 3) == 0)
        SceneActors_SpawnAmbientParticle(p);
    if (p->intermittentEffect && (p->effectTimer & 3) <= 1) {
        u32 effectFlag = 0x10;
        p->actionFlags = effectFlag | p->actionFlags;
    }
    if (p->continuousEffect) {
        u32 effectFlag = 0x10;
        p->actionFlags = effectFlag | p->actionFlags;
    }
    {
        u32 disableDamage = 0x40;
        if (disableDamage & p->actionFlags) {
            damage = &p->damageEffect;
            {
                u32 clear = ~1u;
                damage->flags = clear & damage->flags;
            }
        } else {
            damage = &p->damageEffect;
            {
                u32 set = 1;
                damage->flags = set | damage->flags;
            }
        }
    }
}
