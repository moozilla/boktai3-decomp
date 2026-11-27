#include "global.h"
#include "bosses/hresvelgr/offset_actor_layout.h"
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
s32 DirectionGate_IsAngleInModeRange(struct OffsetActor *);
static inline u8 Enter(struct OffsetActor *p)
{
    if (p->pending) {
        p->pending = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}
static inline void SetAnimation(struct OffsetActor *p, u32 id)
{
    p->animation = id;
}
void OffsetActor_UpdateFacingWait(struct OffsetActor *p)
{
    struct OffsetActor *obj = p;
    struct OffsetActorAction *s = &p->action;
    p->active = 1;
    if (Enter(p))
        ActionState_Reset(s);
    switch (s->state) {
        case 0:
            if (!s->frame)
                SetAnimation(obj, 1);
            if (DirectionGate_IsAngleInModeRange(obj)) {
                s->state = 1;
                SetAnimation(obj, 2);
            } else
                s->frame++;
            if (s->frame > 0x100)
                ActionState_SetState(s, 3);
            break;
        case 1:
            if (obj->finished)
                ActionState_SetState(s, 0);
            break;
        case 2:
            if (obj->finished)
                ActionState_SetState(s, 0);
            break;
        case 3:
            p->ready = 1;
            break;
    }
}
