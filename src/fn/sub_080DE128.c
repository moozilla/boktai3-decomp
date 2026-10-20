#include "global.h"

#ifdef NONMATCHING
// fuzzy: 72.2%
// Remaining differences: mask lifetime, branch layout and state-reset register allocation.
#include "scene_actor_chase.h"
#include "scene_actor_restoration.h"
extern const u8 gUnk_0824E2A8[];
void sub_080DF400(void *);
s32 Math_Atan2(s32, s32);
struct DogAttackContext {
    u8 unknown0[0x678];
    u32 flags;
    u32 nearDistance;
    u32 closeDistance;
    u8 unknown684[0x14];
    u16 nearTimer;
    u16 closeTimer;
};
static inline u8 Busy(struct DogAttackContext *ctx)
{
    if (ctx->flags & 8)
        return 1;
    return 0;
}
#define Dog_SelectTargetAction sub_080DE128
// confidence=low: selects response packets from target range and facing angle.
void Dog_SelectTargetAction(struct SceneActorRegistryResetView *actor)
{
    struct DogAttackContext *ctx = actor->context;
    u8 *group = (u8 *)&actor->group;
    if (actor->flags2D4 & 0x100)
        return;
    if (*(u32 *)((u8 *)actor + 0xF0) < *(u32 *)((u8 *)actor + 0xF4)) {
        struct ContactPosition vector;
        struct ContactPosition *delta = &vector;
        const struct ContactPosition *from = &actor->position;
        const struct ContactPosition *to = &((struct ChaseTarget *)actor->fieldDC)->position;
        s32 angle, difference, magnitude;
        vector.x = to->x - from->x;
        delta->y = to->y - from->y;
        delta->z = to->z - from->z;
        angle = Math_Atan2(vector.x, delta->z);
        difference = angle - actor->direction;
        if (difference > 128)
            difference -= 256;
        else if (difference < -128)
            difference += 256;
        magnitude = difference;
        if (difference < 0)
            magnitude = -difference;
        if (magnitude <= 23) {
            u32 kind = *(u16 *)((u8 *)actor + 0x1E6);
            if (kind == 0 || kind == 2) {
                if (*(u32 *)((u8 *)actor + 0xF0) < ctx->nearDistance && ctx->nearTimer == 0) {
                    Chase_SetState(actor, 0, 2, 0);
                    Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
                    SceneActorReaction_ResetResponseState(group + 0x8C, gUnk_0824E2A8, 0x75);
                    return;
                }
            } else {
                if (*(u32 *)((u8 *)actor + 0xF0) < ctx->nearDistance && ctx->nearTimer == 0) {
                    Chase_SetState(actor, 0, 2, 0);
                    Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
                    SceneActorReaction_ResetResponseState(group + 0x8C, gUnk_0824E2A8, 0x82);
                    return;
                }
                if (*(u32 *)((u8 *)actor + 0xF0) < ctx->closeDistance && magnitude <= 5 &&
                    ctx->closeTimer == 0) {
                    Chase_SetState(actor, 0, 2, 1);
                    Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
                    SceneActorReaction_ResetResponseState(group + 0x8C, gUnk_0824E2A8, 0x87);
                    return;
                }
            }
        }
        if (!Busy(ctx))
            sub_080DF400(actor);
    } else {
        if (!Busy(ctx))
            sub_080DF400(actor);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080DE128);
#endif
