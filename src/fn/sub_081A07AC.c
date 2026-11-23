#include "global.h"
#include "hresvelgr_travel.h"

void Anim_SetFrameDurationScale(void *, u32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);

static inline void SetVector(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Hresvelgr_PlayAscentSequence(struct HresvelgrTravelActor *actor)
{
    struct BossActionState *state = &actor->action;
    Hresvelgr_SetFlags(actor, 0x200000);
    actor->flags |= 0xc;
    if (actor->changed) {
        actor->changed = 0;
        ActionState_Reset(state);
        ActionState_SetState(state, 0);
    }
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    switch (state->state) {
        case 0:
            if (state->frame == 0) {
                void *animation = &actor->animation;
                Hresvelgr_SetAnimation(actor, animation, &actor->animationData, 0, 1, 0);
                Anim_SetFrameDurationScale(animation, 0x80);
                state->frame = 1;
            }
            if (actor->position.y > 0x1fff) {
                Anim_SetFrameDurationScale(&actor->animation, 0x40);
                ActionState_AdvanceState(state);
            } else {
                s32 speed = 0x20;
                SetVector(&actor->velocity, 0, speed, 0);
            }
            break;
        case 1:
            actor->finished = 1;
            break;
    }
}
