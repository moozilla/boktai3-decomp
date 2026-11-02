#include "global.h"
#include "scene_facing.h"
#include "player.h"
#include "engine.h"
#include "state_actor.h"

s32 Math_Atan2(s32, s32);
void Actor_SetAttachmentEffect(void *, u32, s32, s32);

struct ContactPosition *TetherActor_GetPosition(void);
s32 sub_081C8FB0(struct StateActorTargetContext *, struct StateActor *, s32);
static inline u32 Flag(u32 *p, u32 mask)
{
    return *p & mask;
}

static inline s32 PlayerAllowsFacing(struct Player *p)
{
    if ((u8)(p->actionMode - 3) <= 2)
        return 0;
    if (Flag(&p->statusFlags, 0x100000))
        return 0;
    if (p->active && (u16)(p->sceneMode - 5) <= 1)
        return 0;
    {
        u8 mode = p->tetherMode;
        u8 allow = 1;
        if (mode > 1 && mode != 7)
            allow = 0;
        return allow;
    }
}

static inline void SetHeading(struct StateActor *p, s32 heading)
{
    p->angle = heading;
    p->facing8 = SceneFacing_EightWay(heading);
    p->facing4 = SceneFacing_FourWay(heading);
}

static inline u32 PlayerIsTethered(struct StateActorTargetContext *ctx)
{
    if (ctx->targetPlayer->tetherMode == 6) {
        void *position = TetherActor_GetPosition();
        if (position)
            return 1;
    }
    return 0;
}

static inline u32 HasPlayerEvent(u32 mask)
{
    return gPlayerSlotStatus[0] & mask;
}

static inline u32 TurnAnimation(u8 kind)
{
    u32 r;
    switch (kind) {
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
            r = 0;
            break;
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
            r = 0;
            break;
        case 29:
            r = 0xb;
            break;
        default:
            r = 0;
            break;
    }
    return r;
}

static inline void SetDisplayFlag(struct StateActor *p, u32 mask)
{
    p->displayFlags = mask | p->displayFlags;
}

/* Turn toward the tracked player and select the attachment on state entry. */
#define StateActor_UpdateTargetTurn sub_081CC1B4
void StateActor_UpdateTargetTurn(struct StateActorTargetContext *ctx, struct StateActor *p)
{
    u32 r;
    struct StateActorPos *target = ctx->target;
    if (target) {
        struct StateActorPos *position = &p->position;
        s32 desired = Math_Atan2(target->x - position->x, target->z - position->z);
        if (*p->animationTable) {
            s32 diff = (s8)(desired - p->angle);
            if (diff > 8)
                SetHeading(p, p->angle + 8);
            else if (diff < -8)
                SetHeading(p, p->angle - 8);
            else
                SetHeading(p, p->angle + diff);
        }
    }
    if (p->enter) {
        p->enter = 0;
        p->done = 0;
        r = TurnAnimation(p->kind);
        Actor_PlayAnimationEntry(p, r, 1, 0, 4);
        p->overlayDrawable[0].animationStarted = 1;
        p->overlayDrawable[0].savedAngle = p->angle;
        if (ctx->targetPlayer && !PlayerAllowsFacing(ctx->targetPlayer) &&
            !(gEngineFlags & 0x200) && p->attachmentEnabled) {
            if (PlayerIsTethered(ctx) || HasPlayerEvent(4))
                Actor_SetAttachmentEffect(p, 0, 0, 0);
            else
                Actor_SetAttachmentEffect(p, 3, 0, 0);
        }
    } else {
        r = TurnAnimation(p->kind);
        Actor_PlayAnimationEntry(p, r, 1, 0, 1);
    }
    SetDisplayFlag(p, 0x1000);
    if ((u8)Callback_InvokeActor(p, p->animationCallback) != 1 && sub_081C8FB0(ctx, p, 0)) {
        SetDisplayFlag(p, 0x800);
    }
}
