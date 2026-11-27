#include "objects/sliding_doors/scene_object_controller.h"
#include "engine/video/sprite.h"
#include "engine/input.h"
#include "scene/collision/contact_position.h"

s32 MessageQueue_GetMessagesById(u16, void *);
void SlidingDoors_BeginActivation(struct SceneObjPool *, u16);
void SlidingDoors_BeginDeactivation(struct SceneObjPool *, u16);
void SlidingDoors_SetActive(struct SceneObjPool *, u16);
void SlidingDoors_SetInactive(struct SceneObjPool *, u16);
void SlidingDoors_Lock(u8 *, u16);
void SlidingDoors_Unlock(u8 *, u16);
int SlidingDoors_HasPrimaryId(u8 *, u16);
int SlidingDoors_HasSecondaryId(u8 *, u16);
void SlidingDoors_AddPrimaryId(u8 *, u8 *);
void SlidingDoors_UpdatePoseA(u8 *);
void SlidingDoors_UpdatePoseB(u8 *);
void SlidingDoors_UpdatePoseC(u8 *);
u32 TetherActor_CopyPosition(void *);
int SlidingDoors_CheckActorCompatibility(u8 *, u8 *);

struct Obj1FScriptArgs {
    u16 kind, unknown2;
    u8 *data;
};
static inline s32 Obj1F_GetCellDistanceFromOrigin(struct ContactPosition *p)
{
    s32 x = p->x >> 8;
    s32 z = p->z >> 8;
    if (x < 0)
        x = -x;
    if (z < 0)
        z = -z;
    return x + z;
}
static inline s32 Obj1F_Abs(s32 n)
{
    return n < 0 ? -n : n;
}
static inline u8 Obj1F_SceneMatches(u16 *scene)
{
    if ((u32)gGameState->region == *scene)
        return 1;
    return 0;
}
// confidence=low: SceneObjects per-frame update; handles queued activation messages, then classifies each object by its pose kind into the activate list, then diffs the two id lists.
s32 SlidingDoors_UpdateController(struct SceneObjPool *self)
{
    s32 n;
    s32 i;
    u8 *msg;
    struct ContactPosition camera;
    if (self->count == 0)
        return 0;
    for (i = 0; i < self->primaryCount; i++)
        self->secondary[i] = self->primary[i];
    self->secondaryCount = self->primaryCount;
    self->primaryCount = 0;
    n = MessageQueue_GetMessagesById(self->messageId, &self->messages);
    for (msg = self->messages; n-- > 0; msg += 8) {
        switch ((*(u16 **)(msg + 4))[0]) {
            case 0:
                SlidingDoors_BeginActivation(self, (*(u16 **)(msg + 4))[1]);
                break;
            case 1:
                SlidingDoors_BeginDeactivation(self, (*(u16 **)(msg + 4))[1]);
                break;
            case 2:
                SlidingDoors_SetActive(self, (*(u16 **)(msg + 4))[1]);
                break;
            case 3:
                SlidingDoors_SetInactive(self, (*(u16 **)(msg + 4))[1]);
                break;
            case 4:
                SlidingDoors_Lock((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
            case 5:
                SlidingDoors_Unlock((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
        }
    }
    camera.x = gGameState->positionX;
    camera.y = 0;
    camera.z = gGameState->positionZ;
    Obj1F_GetCellDistanceFromOrigin(&camera);
    for (i = 0; i < self->count; i++) {
        struct SceneObjSlot *o = &self->slots[i];
        o->sprite.frame = o->pose;
        if (o->hideOutsideRegion) {
            if (Obj1F_SceneMatches(&o->region))
                o->sprite.flags &= ~1;
            else
                o->sprite.flags |= 1;
        }
        switch (o->type) {
            case 0: {
                struct ContactPosition delta;
                struct ContactPosition tether;
                if (o->activationFlags & 1)
                    break;
                SlidingDoors_UpdatePoseA((u8 *)o);
                delta = *(struct ContactPosition *)&o->pos;
                delta.x -= gGameState->positionX;
                delta.y -= gGameState->positionY;
                delta.z -= gGameState->positionZ;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SlidingDoors_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct ContactPosition delta;
                    delta = *(struct ContactPosition *)&o->pos;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 1:
                SlidingDoors_UpdatePoseB((u8 *)o);
                break;
            case 2:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14: {
                struct ContactPosition delta;
                struct Obj1FScriptArgs args;
                struct ContactPosition tether;
                if (o->activationFlags & 1)
                    break;
                SlidingDoors_UpdatePoseC((u8 *)o);
                delta = *(struct ContactPosition *)&o->pos;
                delta.x -= gGameState->positionX;
                delta.y -= gGameState->positionY;
                delta.z -= gGameState->positionZ;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                    if (!o->autoActivate) {
                        if ((gPadState[1] & 1) &&
                            !SlidingDoors_CheckActorCompatibility((u8 *)self, (u8 *)o) &&
                            o->interactionScript) {
                            args.kind = 4;
                            args.data = (u8 *)&o->args;
                            Script_Call(o->interactionScript, &args);
                        }
                    } else {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                if (TetherActor_CopyPosition(&tether)) {
                    struct ContactPosition delta;
                    delta = *(struct ContactPosition *)&o->pos;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SlidingDoors_HasPrimaryId((u8 *)self, o->id) && o->autoActivate) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 4: {
                struct ContactPosition delta;
                struct ContactPosition tether;
                if (o->activationFlags & 1)
                    break;
                SlidingDoors_UpdatePoseA((u8 *)o);
                delta = *(struct ContactPosition *)&o->pos;
                delta.x -= gGameState->positionX;
                delta.y -= gGameState->positionY;
                delta.z -= gGameState->positionZ;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SlidingDoors_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct ContactPosition delta;
                    delta = *(struct ContactPosition *)&o->pos;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 5: {
                struct ContactPosition delta;
                struct ContactPosition tether;
                if (o->activationFlags & 1)
                    break;
                SlidingDoors_UpdatePoseA((u8 *)o);
                delta = *(struct ContactPosition *)&o->pos;
                delta.x -= gGameState->positionX;
                delta.y -= gGameState->positionY;
                delta.z -= gGameState->positionZ;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SlidingDoors_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct ContactPosition delta;
                    delta = *(struct ContactPosition *)&o->pos;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 6: {
                struct ContactPosition delta;
                struct ContactPosition tether;
                if (o->activationFlags & 1)
                    break;
                SlidingDoors_UpdatePoseA((u8 *)o);
                delta = *(struct ContactPosition *)&o->pos;
                delta.x -= gGameState->positionX;
                delta.y -= gGameState->positionY;
                delta.z -= gGameState->positionZ;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SlidingDoors_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct ContactPosition delta;
                    delta = *(struct ContactPosition *)&o->pos;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
            }
            case 3:
            case 7:
                if (!(o->activationFlags & 1))
                    SlidingDoors_UpdatePoseA((u8 *)o);
                break;
            case 15: {
                struct ContactPosition delta;
                SlidingDoors_UpdatePoseA((u8 *)o);
                if (gGameState->unk876 <= 2)
                    break;
                delta = *(struct ContactPosition *)&o->pos;
                delta.x -= gGameState->positionX;
                delta.y -= gGameState->positionY;
                delta.z -= gGameState->positionZ;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                break;
            }
            case 16: {
                struct ContactPosition delta;
                struct Obj1FScriptArgs args;
                struct ContactPosition tether;
                if (o->activationFlags & 1)
                    break;
                SlidingDoors_UpdatePoseC((u8 *)o);
                delta = *(struct ContactPosition *)&o->pos;
                delta.x -= gGameState->positionX;
                delta.y -= gGameState->positionY;
                delta.z -= gGameState->positionZ;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SlidingDoors_HasPrimaryId((u8 *)self, o->id)) {
                    if (!o->autoActivate) {
                        if ((gPadState[1] & 1) &&
                            !SlidingDoors_CheckActorCompatibility((u8 *)self, (u8 *)o) &&
                            o->interactionScript) {
                            args.kind = 4;
                            args.data = (u8 *)&o->args;
                            Script_Call(o->interactionScript, &args);
                        }
                    } else {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                if (TetherActor_CopyPosition(&tether)) {
                    struct ContactPosition delta;
                    delta = *(struct ContactPosition *)&o->pos;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SlidingDoors_HasPrimaryId((u8 *)self, o->id) && o->autoActivate) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
        }
    }
    for (i = 0; i < self->secondaryCount; i++) {
        if (!SlidingDoors_HasPrimaryId((u8 *)self, self->secondary[i]))
            SlidingDoors_BeginDeactivation(self, self->secondary[i]);
    }
    for (i = 0; i < self->primaryCount; i++) {
        if (!SlidingDoors_HasSecondaryId((u8 *)self, self->primary[i]))
            SlidingDoors_BeginActivation(self, self->primary[i]);
    }
    for (i = 0; i < self->count; i++) {
        struct SceneObjSlot *o = &self->slots[i];
        if (o->callback)
            Callback_InvokeActor(o, o->callback);
    }
    return 0;
}
