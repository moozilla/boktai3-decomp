#include "engine/shared/global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
#include "engine/shared/scene_facing.h"

/* tentative: rises, then dives until it lands and spawns a ground burst */

void Hresvelgr_SetAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void sub_0819C8F8(void *, u32, u32);
void EffectPool_CreateEntry(void *, u32, u32, u32, u32);
static inline void SetVector(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
void Hresvelgr_UpdateGroundSlam(struct HresvelgrTravelActor *p)
{
    struct BossActionState *action = &p->action;
    struct SceneVec pos;
    Hresvelgr_SetFlags(p, 0x600040);
    if (p->changed) {
        p->changed = 0;
        ActionState_Reset(action);
        p->facing = p->targetHeading;
        p->facing4 = SceneFacing_FourWay(p->facing);
        p->facing8 = SceneFacing_EightWay(p->facing);
    }
    switch (action->state) {
        case 0:
            if (action->frame == 0) {
                Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 0, 1, 0);
                (action->frame)++;
            } else if (action->frame > 0x40)
                ActionState_SetState(action, 1);
            else
                (action->frame)++;
            break;
        case 1:
            if (action->frame == 0)
                Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 0x13, 2, 4);
            else
                Hresvelgr_SetFlags(p, 0x800000);
            if (++(action->frame) > 0x10)
                ActionState_SetState(action, 2);
            break;
        case 2:
            if (action->frame == 0) {
                Sound_PlaySong(0x3E2);
                (action->frame)++;
            }
            sub_0819C8F8(p, 0x10, action->frame << 2);
            if (p->pulse) {
                s32 x = p->position.x;
                s32 z = p->position.z;
                SetVector(&pos, x, 0x200, z);
                EffectPool_CreateEntry(&pos, p->side << 6, 10, 4, 0x20);
                Sound_PlaySong(0x3DA);
                ActionState_SetState(action, 3);
            } else
                (action->frame)++;
            break;
        case 3:
            p->finished = 1;
            break;
    }
    Callback_InvokeActor(p, p->callback);
}
