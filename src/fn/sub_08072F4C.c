#include "scene_actor_init_actor.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_adjacent_cell.h"
#include "scene_actor_control_helpers.h"
#include "scene_actor_chase_shared.h"
struct SceneActorContactSample {
    u8 unknown0[8];
    u8 playerSlot, unknown9[3];
    struct ContactPosition position;
    u8 unknown14[0x48 - 0x14];
};
u8 SceneActorContacts_TestPlayerCondition(struct SceneActorContactSample *, u32, u32);
u32 SceneActorTerrain_IsFacingPosition(struct SceneActorTerrainView *,
                                       const struct ContactPosition *);
u32 TetherActor_CopySampledPosition(struct ContactPosition *);
s32 sub_08076CDC(struct ChaseActor *, struct ContactPosition *, struct ContactPosition *);
s32 sub_0821D4F0(struct ContactPosition *, struct ContactPosition *, s32, s32);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
void *memcpy(void *, const void *, unsigned long);
extern u32 gUnk_02000560[];
static inline s32 HalfDistance(s32 a)
{
    if (a >= 0)
        a = a >> 1;
    else
        a = -((-a) >> 1);
    return a;
}
static inline s32 SquareDistance(struct ContactPosition *p)
{
    s32 x = p->x;
    u32 squaredX = x * x;
    s32 z = p->z;
    u32 squaredZ = z * z;
    return squaredX + squaredZ;
}
static inline u32 PlayerMask(s32 index, u32 mask)
{
    return gUnk_02000560[index] & mask;
}
static inline s32 Absolute(s32 angle)
{
    return angle < 0 ? -angle : angle;
}
static inline struct SceneActorChaseTracking *TrackingOf(struct ChaseActor *self)
{
    return &self->base.tracking;
}
static inline s32 AngleSeparation(s32 angle)
{
    if (angle > 128)
        angle -= 256;
    else if (angle < -128)
        angle += 256;
    return Absolute(angle);
}

static inline s32 PlayerSlot(struct SceneActorContactSample *p)
{
    if (p->playerSlot <= 3)
        return p->playerSlot;
    return -1;
}

static inline u8 EvaluateSample(struct ChaseActor *base, struct SceneActorContactSample *sample,
                                u32 kind, s32 *bestDistance)
{
    struct ChaseActor *self = base;
    struct SceneActorChaseGroup *group = &base->group;
    struct ContactPosition difference;
    struct ContactPosition *delta = &difference;
    struct ContactPosition *own = &base->base.body.position;
    struct ContactPosition *position = &sample->position;
    s32 dist, angle, limit, threshold, transition, cell;
    u32 facing;
    difference.x = position->x - own->x;
    difference.y = position->y - own->y;
    difference.z = position->z - own->z;
    dist = SquareDistance(&difference);

    if ((u8)SceneActorTerrain_IsFacingPosition((void *)base, position)) {
        *bestDistance = dist;
        SceneActorContacts_SetDistanceGate((void *)base, (void *)&base->base.tracking, dist, 0, 0,
                                           kind);
        return 1;
    }
    facing = self->base.body.direction;
    angle = AngleSeparation(Math_Atan2(delta->x, delta->z) - facing);
    if (angle > self->base.tracking.fieldEA)
        return 0;

    if (kind == 2) {
        if (SceneActorControl_TestFlags(gEngineFlags, 0x4800)) {
            if (!SceneActorContacts_TestPlayerCondition(sample, 1, 0x10000))
                goto plain;
            limit = HalfDistance((s32)self->base.tracking.limit);
            if (SceneActorChase_TrackingFlags(TrackingOf(self), 4))
                threshold = limit;
            else
                threshold = Div(limit * 7, 10);
        } else {
            if (SceneActorContacts_TestPlayerCondition(sample, 1, 0x10000) ||
                !(MonsterInit_TestWorldFlags(0x20))) {
                limit = HalfDistance((s32)self->base.tracking.limit);
                if (SceneActorChase_TrackingFlags(TrackingOf(self), 4))
                    threshold = limit;
                else
                    threshold = Div(limit * 7, 10);
            } else {
                goto plain;
            }
        }
    } else {
        if (SceneActorControl_TestFlags(gEngineFlags, 0x4800))
            return 0;
        if (!MonsterInit_TestWorldFlags(0x20)) {
            limit = HalfDistance((s32)self->base.tracking.limit);
            if (SceneActorChase_TrackingFlags(TrackingOf(self), 4))
                threshold = limit;
            else
                threshold = Div(limit * 7, 10);
        } else {
            goto plain;
        }
    }
    goto gate;
plain:
    limit = (s32)self->base.tracking.limit;
    threshold = (s32)self->base.tracking.range;
gate:
    if (dist > limit)
        return 0;
    transition = sub_08076CDC(self, &self->base.body.position, &sample->position);
    if (transition < 0)
        return 0;
    cell = SceneActorTerrain_GetCellIndex(&sample->position);
    if (SceneActor_IsAdjacentCellOpen(self, cell)) {
        if (!SceneActorChase_TrackingFlags(TrackingOf(self), 4)) {
            if (!Nav_IsPositionInMesh(group->route, &self->base.body.position, &sample->position))
                return 0;
        }
        *bestDistance = dist;
        SceneActorContacts_SetDistanceGate((void *)self, (void *)&self->base.tracking, dist, 0, 0,
                                           kind);
        return 1;
    }
    if (transition != 2) {
        if (SceneActorChase_TrackingFlags(TrackingOf(self), 0x20)) {
            if (!(u8)sub_0821D4F0(&self->base.tracking.destination, &sample->position, 10, 0x100))
                return 0;
        } else if (!(u8)sub_0821D4F0(&self->base.body.position, &sample->position, 10, 0x100))
            return 0;
    }
    if (dist < *bestDistance) {
        *bestDistance = dist;
        SceneActorContacts_SetDistanceGate((void *)self, (void *)&self->base.tracking, dist,
                                           threshold, transition, kind);
        return 1;
    }
    return 0;
}

u8 SceneActorContacts_EvaluateCandidate(struct SceneActorContactNode *node, struct ChaseActor *base,
                                        s32 *distance)
{

    struct SceneActorContactSample copy;
    int tries;
    switch (node->kind) {
        case 1:
        case 4:
            memcpy(&copy, node->object, sizeof(copy));
            for (tries = 0; tries < 2; tries++) {
                if (EvaluateSample((void *)base, &copy, node->kind, distance) &&
                    Nav_IsPositionInMesh(base->group.route, &base->base.body.position,
                                         &copy.position))
                    return 1;
                TetherActor_CopySampledPosition(&copy.position);
            }
            break;
        case 2:
            if (!SceneActorChase_TrackingFlags(&base->base.tracking, 2))
                break;
            {
                u32 mode = SceneActorControl_TestFlags(gEngineFlags, 0x4800);
                struct SceneActorContactSample *candidate = node->object;
                if (mode) {
                    if (!candidate)
                        break;
                    {
                        s32 index = PlayerSlot(candidate);
                        if (index < 0)
                            break;
                        if (PlayerMask(index, 8))
                            break;
                    }
                }
                if (SceneActorContacts_TestPlayerCondition(candidate, 1, 0x21000))
                    break;
            }

            if (EvaluateSample((void *)base, node->object, node->kind, distance) &&
                Nav_IsPositionInMesh(base->group.route, &base->base.body.position,
                                     &((struct SceneActorContactSample *)node->object)->position))
                return 1;
            break;
    }
    return 0;
}
