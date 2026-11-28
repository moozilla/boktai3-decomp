#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "objects/sun_orb/sun_orb_effect.h"

void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
u8 Callback_InvokeActor(void *, void *);
s32 Ratatosk_FindPointNearestReference(void *, s32);
void RisingActorManager_BeginRaise(s32);

void Ratatosk_UpdateSunOrbSummon(struct Ratatosk *p, s32 tick)
{
    struct OrbitEffectParams params;
    struct Ratatosk *alias = p;

    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 15, 1, 4);
        p->actionCounter = 0;
        p->selectedPoint = 0xffff;
        p->selectedTarget = -1;
        Sound_PlaySong(0x570);
    }
    switch (p->actionCounter) {
        case 0:
            if (tick > 5) {
                SceneActors_StartUnmirroredAnimation(p, alias->animationData, alias->animation, 16,
                                                     1, 4);
                alias->actionCounter++;
            }
            break;
        case 1:
            if (tick > 85) {
                SceneActors_StartUnmirroredAnimation(p, alias->animationData, alias->animation, 15,
                                                     1, 4);
                alias->actionCounter++;
                Sound_PlaySong(0x55b);
            }
            break;
        case 2: {
            s32 index = Ratatosk_FindPointNearestReference(alias, 0);
            s32 slot;

            if (index < 0) {
                alias->actionCounter = 4;
                break;
            }
            RisingActorConfig_Initialize(&params);
            params.unk_07 = index;
            SunOrb_SetTargetSource(&params, 1);
            slot = SunOrb_CreateAtPosition(
                &params, (struct OrbitEffectPosition *)&alias->orbPoints[index].position);
            if (slot >= 0) {
                alias->orbPoints[index].active = 1;
                alias->targets[slot].active = 1;
                RisingActorManager_BeginRaise(slot);
                alias->selectedPoint = slot;
            }
            SceneActors_StartUnmirroredAnimation(p, alias->animationData, alias->animation, 16, 1,
                                                 4);
            alias->actionCounter++;
            p->actionFinished = 1;
            break;
        }
        case 3:
        case 4:
            p->actionFinished = 1;
            break;
    }
    Callback_InvokeActor(p, p->actionTryUpdate);
}
