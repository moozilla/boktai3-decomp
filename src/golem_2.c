#include "golem_2.h"

#define Golem_TickFacingQuadrant sub_080CC830

extern const u8 gUnk_08606134[];

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void sub_0807C72C(u8 *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(u8 *);
void sub_080C4EF0(u8 *);
void Camera_RequestShake(u32);
void Skeleton_TryTriggerAction(void *);
void Skeleton_ClearPendingFlag(void *);
void sub_080C53A8(void *);

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline u32 Flags16(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D4;
    return *(u16 *)((u8 *)a + off) & m;
}

static inline u32 Flags32(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D0;
    return *(u32 *)((u8 *)a + off) & m;
}

static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Flags16(a, 0x40) || Flags32(a, 0x20000) || a->health <= 0)
            return 1;
    }
    return 0;
}

void Golem_OnUnusedState(void)
{
}

INCLUDE_ASM("asm/nonmatching", sub_080C9848);

INCLUDE_ASM("asm/nonmatching", sub_080C9C04);

INCLUDE_ASM("asm/nonmatching", sub_080CBE22);

void Golem_UpdateScriptedAction(struct SceneActorRegistryResetView *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        void (*fn)(void) = SceneActorReaction_PlayAnimationBUntilComplete;
        u8 code = 10;
        u32 zero;
        u32 *f;
        s32 mask;
        u16 *h;
        u16 value;
        s32 off;
        {
            u8 *taken = &obj->field2B2;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 field = 0x2B0;
            do {
                ((u8 *)obj)[field] = zero;
            } while (0);
            field += 5;
            ((u8 *)obj)[off = field] = code;
            obj->target = (void *)fn;
            field += 0x13;
            *((u16 *)(((u8 *)obj) + field)) = zero;
        } while (0);
        f = (u32 *)(((u8 *)obj) + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(((u8 *)obj) + 0x104)) = zero;
        {
            u32 byte = obj->fieldE9;
            obj->direction = byte;
        }
        sub_0807C72C((u8 *)obj, 0, 0, obj->category);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (u16 *)(((u8 *)obj) + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        off = 0x416;
        value = *((u16 *)(((u8 *)obj) + off));
        off += 2;
        *((u16 *)(((u8 *)obj) - (-off))) = value;
    }
    if (obj->field2B0) {
        sub_080C4EF0((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

INCLUDE_ASM("asm/nonmatching", sub_080CBF24);

INCLUDE_ASM("asm/nonmatching", sub_080CC80C);

void Golem_InitActionHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->alternateBehaviorCallbacks = (void **)gUnk_08606134;
}

s32 Golem_TickFacingQuadrant(struct SceneActorRegistryResetView *a)
{
    u8 *base = (u8 *)&a->id;
    struct GolemState *s = a->context;
    if (!Unavailable(a)) {
        if (s->countdown626)
            s->countdown626--;
    }
    {
        u32 quadrant = (base[9] + 0x60) >> 6;
        u32 mask = 3;
        mask &= quadrant;
        a->unknown68C = mask;
    }
    a->savedDirection690 = a->direction68E;
    return 1;
}

s32 Golem_UpdateEffectsAndContacts(struct SceneActorRegistryResetView *actor)
{
    struct GolemState *state = actor->context;
    u32 m = 0x40;
    if (!(actor->flags2D4 & m)) {
        u16 *p = &state->shakeTimer6AE;
        if (*p != 0) {
            Camera_RequestShake(state->shakeStrength6AC);
            *p = *p - 1;
        }
    }
    Skeleton_TryTriggerAction(actor);
    Skeleton_ClearPendingFlag(actor);
    sub_080C53A8(actor);
    return 1;
}
