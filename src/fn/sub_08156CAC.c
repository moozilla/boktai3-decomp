#define Player_EmitPeriodicParticles sub_08156CAC
#define SparkParticle_SpawnBurst     sub_0806A160
#define gGameState                   gUnk_02000710
#include "global.h"
#include "player.h"
#include "game_state.h"
#include "contact_actor.h"
void sub_0806A160(void *, s32, void *, s32, s32, s32, s32, s32);
void sub_08156CAC(struct Player *p)
{
    struct ContactPackedVector spread, position;
    if (gUnk_02000710->playerParticleRate) {
        if (gUnk_02000710->playerParticleRate <= 2499)
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
            sub_0806A160(&position, 1, &spread, 0, 0, 6, 3, 2);
            p->particleCadenceCounter = 0;
        }
    }
}
