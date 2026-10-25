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
/* Camera-relative proximity test used by every case: horizontal distance squared and |dy|. */
static inline int Obj1F_Near(u8 *o, struct Obj1FPos *ref)
{
    s16 dx = *(s16 *)(o + 0x68) - ref->x;
    s16 dy = *(s16 *)(o + 0x6A) - ref->y;
    s16 dz = *(s16 *)(o + 0x6C) - ref->z;
    if (dx * dx + dz * dz > 0x12CC6)
        return 0;
    if (dy < 0)
        dy = -dy;
    if (dy > 0xFE)
        return 0;
    return 1;
}
static inline struct Obj1FPos *Obj1F_Cam(void)
{
    return (struct Obj1FPos *)((u8 *)gUnk_02000710 + 0x30);
}
static inline void Obj1F_Append(u8 *self, u16 id)
{
    ((u16 *)(self + 0x1e))[self[0x1c]] = id;
    self[0x1c]++;
}
/* Poses an object, then also activates it when the tether position is near. */
static inline void Obj1F_PoseATether(u8 *self, u8 *o, struct Obj1FPos *tmp)
{
    u16 id = *(u16 *)(o + 0x34);
    if (o[0x36] & 1)
        return;
    SceneObject_UpdatePoseA(o);
    if (Obj1F_Near(o, Obj1F_Cam()) && !SceneObjects_HasPrimaryId(self, id))
        Obj1F_Append(self, id);
    SceneObjects_AddPrimaryId(self, o);
    if (!TetherActor_CopyPosition(tmp))
        return;
    if (!Obj1F_Near(o, tmp))
        return;
    if (SceneObjects_HasPrimaryId(self, id))
        return;
    Obj1F_Append(self, id);
}
static inline void Obj1F_PoseC(u8 *self, u8 *o, struct Obj1FPos *tmp, struct Obj1FScriptArgs *args)
{
    u16 id = *(u16 *)(o + 0x34);
    if (o[0x36] & 1)
        return;
    SceneObject_UpdatePoseC(o);
    if (Obj1F_Near(o, Obj1F_Cam()) && !SceneObjects_HasPrimaryId(self, id)) {
        if (o[0x41] == 0) {
            if ((gUnk_03005260[1] & 1) && !SceneObject_CheckActorCompatibility(self, o) &&
                *(u32 *)(o + 0x48)) {
                args->kind = 4;
                args->data = o + 0x50;
                Script_Call(*(u32 *)(o + 0x48), args);
            }
        } else {
            Obj1F_Append(self, id);
        }
    }
    if (TetherActor_CopyPosition(tmp) && Obj1F_Near(o, tmp) &&
        !SceneObjects_HasPrimaryId(self, id) && o[0x41])
        Obj1F_Append(self, id);
}

#ifdef NONMATCHING
// fuzzy: 24.9%
// scope shape 959 (differences once low registers are set aside)
// confidence=low: SceneObjects per-frame update; handles queued activation messages, then classifies each object by its pose kind into the activate list, then diffs the two id lists.
s32 sub_0801F93C(u8 *self)
{
    s32 n;
    s32 i;
    u8 *msg;
    struct Obj1FPos tetherA, tetherB, tetherC, tetherD, tetherE, tetherF;
    struct Obj1FScriptArgs argsA, argsB;
    if (*(u16 *)(self + 0x1a) == 0)
        return 0;
    for (i = 0; i < self[0x1c]; i++)
        ((u16 *)(self + 0x26))[i] = ((u16 *)(self + 0x1e))[i];
    self[0x1d] = self[0x1c];
    self[0x1c] = 0;
    n = MessageQueue_GetMessagesById(*(u16 *)(self + 0x18), self + 0x38);
    for (msg = *(u8 **)(self + 0x38); n-- > 0; msg += 8) {
        u16 *m = *(u16 **)(msg + 4);
        switch (m[0]) {
            case 0:
                SceneObjects_BeginActivation(self, m[1]);
                break;
            case 1:
                SceneObjects_BeginDeactivation(self, m[1]);
                break;
            case 2:
                SceneObjects_SetActive(self, m[1]);
                break;
            case 3:
                SceneObjects_SetInactive(self, m[1]);
                break;
            case 4:
                SceneObjects_Lock(self, m[1]);
                break;
            case 5:
                SceneObjects_Unlock(self, m[1]);
                break;
        }
    }
    for (i = 0; i < *(u16 *)(self + 0x1a); i++) {
        u8 *o = *(u8 **)(self + 0x42C) + i * 0x8C;
        u16 id;
        *(u16 *)(o + 0x10) = o[0x39];
        if (o[0x44]) {
            if (*(u32 *)((u8 *)gUnk_02000710 + 0x5A0) == *(u16 *)(o + 0x42))
                *(u32 *)o &= ~1;
            else
                *(u32 *)o |= 1;
        }
        id = *(u16 *)(o + 0x34);
        switch (o[0x40]) {
            case 0:
                Obj1F_PoseATether(self, o, &tetherA);
                break;
            case 1:
                SceneObject_UpdatePoseB(o);
                break;
            case 2:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
                Obj1F_PoseC(self, o, &tetherB, &argsA);
                break;
            case 3:
            case 7:
                if (!(o[0x36] & 1))
                    SceneObject_UpdatePoseA(o);
                break;
            case 4:
                Obj1F_PoseATether(self, o, &tetherC);
                break;
            case 5:
                Obj1F_PoseATether(self, o, &tetherD);
                break;
            case 6:
                if (o[0x36] & 1)
                    break;
                SceneObject_UpdatePoseA(o);
                if (Obj1F_Near(o, Obj1F_Cam()) && !SceneObjects_HasPrimaryId(self, id))
                    Obj1F_Append(self, id);
                SceneObjects_AddPrimaryId(self, o);
                if (TetherActor_CopyPosition(&tetherE) && Obj1F_Near(o, &tetherE) &&
                    !SceneObjects_HasPrimaryId(self, id))
                    Obj1F_Append(self, id);
                if (!(o[0x36] & 1))
                    SceneObject_UpdatePoseA(o);
                break;
            case 15:
                SceneObject_UpdatePoseA(o);
                if (*(s16 *)((u8 *)gUnk_02000710 + 0x876) <= 2)
                    break;
                if (Obj1F_Near(o, Obj1F_Cam()) && !SceneObjects_HasPrimaryId(self, id))
                    Obj1F_Append(self, id);
                break;
            case 16:
                Obj1F_PoseC(self, o, &tetherF, &argsB);
                break;
        }
    }
    for (i = 0; i < self[0x1d]; i++) {
        u16 *p = (u16 *)(self + 0x26) + i;
        if (!SceneObjects_HasPrimaryId(self, *p))
            SceneObjects_BeginDeactivation(self, *p);
    }
    for (i = 0; i < self[0x1c]; i++) {
        u16 *p = (u16 *)(self + 0x1e) + i;
        if (!SceneObjects_HasSecondaryId(self, *p))
            SceneObjects_BeginActivation(self, *p);
    }
    for (i = 0; i < *(u16 *)(self + 0x1a); i++) {
        u8 *o = *(u8 **)(self + 0x42C) + i * 0x8C;
        if (*(void **)(o + 0x30))
            Callback_InvokeActor(o, *(void **)(o + 0x30));
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0801F93C);
#endif
