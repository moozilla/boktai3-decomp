#include "global.h"
#include "engine/ram.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

/* tentative: travels to the nearest active growth hazard, lands on it and resets its cooldown */

void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
s32 GrowthHazard_FindNearestActiveEntry(void *, struct SceneVec *, struct SceneVec *);
s32 ColliderActor_SetTargetPosition(struct HresvelgrTravelActor *, struct SceneVec *);
s32 Hresvelgr_UpdateTravel(struct HresvelgrTravelActor *);
void Anim_SetFrameDurationScale(void *, u32);
void GrowthHazard_ResetEntryCooldown(void *, s32);

void Hresvelgr_UpdateHazardVisit(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *self = p;
    Hresvelgr_SetFlags(p, 0x40);
    if (p->changed) {
        p->changed = 0;
        gHresvelgrHazardIndex =
            GrowthHazard_FindNearestActiveEntry(p->hazards, &p->position, &p->target);
        if (gHresvelgrHazardIndex >= 0) {
            Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 0, 1, 4);
            ColliderActor_SetTargetPosition(p, &p->target);
            p->phase = 0;
            p->phaseFrame = 0;
        } else {
            Hresvelgr_EnterPhase(p, 2);
        }
    }
    if (p->pulse)
        Sound_PlaySong(0x3D9);
    switch (p->phase) {
        case 0:
            if (Hresvelgr_UpdateTravel(self)) {
                p->phase = 1;
                p->phaseFrame = 0;
            }
            break;
        case 1:
            if (p->phaseFrame == 0) {
                Hresvelgr_SetAnimation(self, &self->animation, &self->animationData, 0x13, 1, 0);
                Anim_SetFrameDurationScale(&self->animation, 0x20);
                p->phaseFrame++;
            }
            if (p->pulse && p->animationCompletions > 4) {
                GrowthHazard_ResetEntryCooldown(self->hazards, gHresvelgrHazardIndex);
                Anim_SetFrameDurationScale(&self->animation, 0x40);
                Hresvelgr_EnterPhase(p, 2);
            }
            break;
        case 2:
            p->finished = 1;
            break;
    }
    Callback_InvokeActor(p, p->callback);
}
