#include "global.h"

#include "engine.h"

extern u16 gUnk_03005260[];
s32 MessageQueue_GetMessagesById(u16, void *);
void SceneObjects_BeginActivation(u8 *, u16);
void SceneObjects_BeginDeactivation(u8 *, u16);
void SceneObjects_SetActive(u8 *, u16);
void SceneObjects_SetInactive(u8 *, u16);
void SceneObjects_Lock(u8 *, u16);
void SceneObjects_Unlock(u8 *, u16);
int SceneObjects_HasPrimaryId(u8 *, u16);
int SceneObjects_HasSecondaryId(u8 *, u16);
void SceneObjects_AddPrimaryId(u8 *, u8 *);
void SceneObject_UpdatePoseA(u8 *);
void SceneObject_UpdatePoseB(u8 *);
void SceneObject_UpdatePoseC(u8 *);
u32 TetherActor_CopyPosition(void *);
int SceneObject_CheckActorCompatibility(u8 *, u8 *);

struct Obj1FPos {
    s16 x, y, z;
};
struct Obj1FScriptArgs {
    u16 kind, unknown2;
    u8 *data;
};
struct Obj1FObject {
    u32 flags;
    u8 pad4[0xc];
    u16 pose;
    u8 pad12[0x1e];
    void *callback;
    u16 id;
    u8 flags36;
    u8 pad37[2];
    u8 pose39;
    u8 pad3a[6];
    u8 kind, active;
    u16 scene;
    u8 gated;
    u8 pad45[3];
    u32 script;
    u8 pad4c[4];
    u8 scriptData[0x18];
    struct Obj1FPos position;
    u8 pad70[0x1c];
};
struct Obj1FController {
    u8 pad0[0x18];
    u16 messageId, count;
    u8 primaryCount, secondaryCount;
    u16 primary[4], secondary[4];
    u8 pad2e[10];
    u8 *messages;
    u8 pad3c[0x3f0];
    struct Obj1FObject *objects;
};
static inline s32 Obj1F_GetCellDistanceFromOrigin(struct Obj1FPos *p)
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
    if (*(u32 *)((u8 *)gGameState + 0x5a0) == *scene)
        return 1;
    return 0;
}
// confidence=low: SceneObjects per-frame update; handles queued activation messages, then classifies each object by its pose kind into the activate list, then diffs the two id lists.
s32 sub_0801F93C(struct Obj1FController *self)
{
    s32 n;
    s32 i;
    u8 *msg;
    struct Obj1FPos camera;
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
                SceneObjects_BeginActivation((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
            case 1:
                SceneObjects_BeginDeactivation((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
            case 2:
                SceneObjects_SetActive((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
            case 3:
                SceneObjects_SetInactive((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
            case 4:
                SceneObjects_Lock((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
            case 5:
                SceneObjects_Unlock((u8 *)self, (*(u16 **)(msg + 4))[1]);
                break;
        }
    }
    camera.x = ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
    camera.y = 0;
    camera.z = ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
    Obj1F_GetCellDistanceFromOrigin(&camera);
    for (i = 0; i < self->count; i++) {
        struct Obj1FObject **base = &self->objects;
        s32 offset = i * sizeof(struct Obj1FObject);
        struct Obj1FObject *o = (struct Obj1FObject *)((u8 *)*base + offset);
        o->pose = o->pose39;
        if (o->gated) {
            if (Obj1F_SceneMatches(&o->scene))
                o->flags &= ~1;
            else
                o->flags |= 1;
        }
        switch (o->kind) {
            case 0: {
                struct Obj1FPos delta;
                struct Obj1FPos tether;
                if (o->flags36 & 1)
                    break;
                SceneObject_UpdatePoseA((u8 *)o);
                delta = o->position;
                delta.x -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
                delta.y -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->y;
                delta.z -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SceneObjects_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct Obj1FPos delta;
                    delta = o->position;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 1:
                SceneObject_UpdatePoseB((u8 *)o);
                break;
            case 2:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14: {
                struct Obj1FPos delta;
                struct Obj1FScriptArgs args;
                struct Obj1FPos tether;
                if (o->flags36 & 1)
                    break;
                SceneObject_UpdatePoseC((u8 *)o);
                delta = o->position;
                delta.x -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
                delta.y -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->y;
                delta.z -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                    if (!o->active) {
                        if ((gUnk_03005260[1] & 1) &&
                            !SceneObject_CheckActorCompatibility((u8 *)self, (u8 *)o) &&
                            o->script) {
                            args.kind = 4;
                            args.data = o->scriptData;
                            Script_Call(o->script, &args);
                        }
                    } else {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                if (TetherActor_CopyPosition(&tether)) {
                    struct Obj1FPos delta;
                    delta = o->position;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SceneObjects_HasPrimaryId((u8 *)self, o->id) && o->active) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 4: {
                struct Obj1FPos delta;
                struct Obj1FPos tether;
                if (o->flags36 & 1)
                    break;
                SceneObject_UpdatePoseA((u8 *)o);
                delta = o->position;
                delta.x -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
                delta.y -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->y;
                delta.z -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SceneObjects_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct Obj1FPos delta;
                    delta = o->position;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 5: {
                struct Obj1FPos delta;
                struct Obj1FPos tether;
                if (o->flags36 & 1)
                    break;
                SceneObject_UpdatePoseA((u8 *)o);
                delta = o->position;
                delta.x -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
                delta.y -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->y;
                delta.z -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SceneObjects_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct Obj1FPos delta;
                    delta = o->position;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
            case 6: {
                struct Obj1FPos delta;
                struct Obj1FPos tether;
                if (o->flags36 & 1)
                    break;
                SceneObject_UpdatePoseA((u8 *)o);
                delta = o->position;
                delta.x -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
                delta.y -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->y;
                delta.z -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                SceneObjects_AddPrimaryId((u8 *)self, (u8 *)o);
                if (TetherActor_CopyPosition(&tether)) {
                    struct Obj1FPos delta;
                    delta = o->position;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
            }
            case 3:
            case 7:
                if (!(o->flags36 & 1))
                    SceneObject_UpdatePoseA((u8 *)o);
                break;
            case 15: {
                struct Obj1FPos delta;
                SceneObject_UpdatePoseA((u8 *)o);
                if (*(s16 *)((u8 *)gGameState + 0x876) <= 2)
                    break;
                delta = o->position;
                delta.x -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
                delta.y -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->y;
                delta.z -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                    self->primary[self->primaryCount] = o->id;
                    self->primaryCount++;
                }
                break;
            }
            case 16: {
                struct Obj1FPos delta;
                struct Obj1FScriptArgs args;
                struct Obj1FPos tether;
                if (o->flags36 & 1)
                    break;
                SceneObject_UpdatePoseC((u8 *)o);
                delta = o->position;
                delta.x -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->x;
                delta.y -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->y;
                delta.z -= ((struct Obj1FPos *)((u8 *)gGameState + 0x30))->z;
                if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                    Obj1F_Abs(delta.y) <= 0xFE && !SceneObjects_HasPrimaryId((u8 *)self, o->id)) {
                    if (!o->active) {
                        if ((gUnk_03005260[1] & 1) &&
                            !SceneObject_CheckActorCompatibility((u8 *)self, (u8 *)o) &&
                            o->script) {
                            args.kind = 4;
                            args.data = o->scriptData;
                            Script_Call(o->script, &args);
                        }
                    } else {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                if (TetherActor_CopyPosition(&tether)) {
                    struct Obj1FPos delta;
                    delta = o->position;
                    delta.x -= (&tether)->x;
                    delta.y -= (&tether)->y;
                    delta.z -= (&tether)->z;
                    if (delta.x * delta.x + delta.z * delta.z <= 0x12CC7 &&
                        Obj1F_Abs(delta.y) <= 0xFE &&
                        !SceneObjects_HasPrimaryId((u8 *)self, o->id) && o->active) {
                        self->primary[self->primaryCount] = o->id;
                        self->primaryCount++;
                    }
                }
                break;
            }
        }
    }
    for (i = 0; i < self->secondaryCount; i++) {
        if (!SceneObjects_HasPrimaryId((u8 *)self, self->secondary[i]))
            SceneObjects_BeginDeactivation((u8 *)self, self->secondary[i]);
    }
    for (i = 0; i < self->primaryCount; i++) {
        if (!SceneObjects_HasSecondaryId((u8 *)self, self->primary[i]))
            SceneObjects_BeginActivation((u8 *)self, self->primary[i]);
    }
    for (i = 0; i < self->count; i++) {
        struct Obj1FObject **base = &self->objects;
        s32 offset = i * sizeof(struct Obj1FObject);
        struct Obj1FObject *o = (struct Obj1FObject *)((u8 *)*base + offset);
        if (o->callback)
            Callback_InvokeActor(o, o->callback);
    }
    return 0;
}
