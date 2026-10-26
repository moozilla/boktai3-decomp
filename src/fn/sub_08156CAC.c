#include "global.h"
#include "player.h"
#include "game_state.h"
#include "contact_actor.h"
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Player_EmitPeriodicParticles(struct Player *p)
{
    struct ContactPackedVector spread, position;
    if (gGameState->playerParticleRate) {
        if (gGameState->playerParticleRate <= 2499)
            p->particleCadenceCounter++;
        else
            p->particleCadenceCounter += 2;
        if (p->particleCadenceCounter > 15) {
            if ((u8)(p->actionMode - 3) <= 1) {
                position = *(struct ContactPackedVector *)&p->colliderPosition;
                spread.x = 32;
                spread.y = 32;
                spread.z = 32;
            } else {
                position = *(struct ContactPackedVector *)&p->position;
                position.y += 360;
                spread.x = 32;
                spread.y = 32;
                spread.z = 32;
            }
            SparkParticle_Spawn(&position, 1, &spread, 0, 0, 6, 3, 2);
            p->particleCadenceCounter = 0;
        }
    }
}
