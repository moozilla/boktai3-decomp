#include "global.h"
#include "scene_actor_reaction.h"

/* Bat state callback: restores the saved snapshot (sibling of Vorn_RestoreSavedState)
 * and plays song 0xEE on one animation frame. Owner +0x48 is an animation record.
 * The snapshot fields below are copied and reset at the listed offsets. */

typedef struct Inner {
    u8 pad0[8];
    u32 unk8;
} Inner;

typedef struct Anim {
    u8 pad0[8];
    u16 unk8;
    u8 padA[4];
    u16 unkE;
} Anim;

typedef struct Owner {
    u32 unk0;
    u8 pad4[0x1c];
    Inner inner;
    u8 pad2C[0x1c];
    Anim anim;
} Owner;

typedef struct Obj {
    u8 pad0[0x4];
    Owner *unk4;
    u8 unk8;
    u8 pad9[0x50];
    u8 unk59;
    u8 pad5A[0x42];
    u16 unk9C;
    u8 pad9E[0x12];
    s16 unkB0;
    u8 padB2[0x2A];
    u32 unkDC;
    u8 padE0[0x38];
    u32 unk118;
    u32 unk11C;
    u8 unk120;
    u8 unk121;
    u8 unk122;
    u8 unk123;
    u32 unk124;
    u8 unk128;
    u8 pad129[0x3];
    u16 unk12C;
    u16 unk12E;
    u8 pad130[0x48];
    u16 unk178;
    u8 pad17A[0xA2];
    u32 unk21C;
    u8 pad220[0x88];
    u8 unk2A8;
    u8 unk2A9;
    u8 unk2AA;
    u8 pad2AB[0x5];
    u8 unk2B0;
    u8 unk2B1;
    u8 unk2B2;
    u8 unk2B3;
    u8 pad2B4[0x1];
    u8 unk2B5;
    u8 unk2B6;
    u8 pad2B7[0x9];
    u16 unk2C0;
    u16 unk2C2;
    u8 pad2C4[0x8];
    u32 unk2CC;
    u32 unk2D0;
    u16 unk2D4;
    u16 unk2D6;
    u32 unk2D8;
    u32 unk2DC;
    u32 unk2E0;
    u8 pad2E4[0x2];
    u16 unk2E6;
    u8 pad2E8[0x2];
    u16 unk2EA;
    u8 pad2EC[0x18];
    u32 unk304;
    u8 pad308[0x18];
    u8 unk320[0xac];
    u8 unk3CC;
} Obj;

void ActorShadow_Enable(void *);

void Ghoul_ResetRouteState(Obj *);

void Sound_PlaySong(u32);

void SceneActorPalette_ClearCompletedTransitions(void);

static inline u8 take_flag(Obj *s)
{
    if (s->unk2B2) {
        s->unk2B2 = 0;
        return 1;
    }
    return 0;
}

/* Keep this byte predicate local; its address is not retained by the body. */

static inline u8 peek_flag(Obj *s)
{
    return s->unk2B3;
}

void Bat_RestoreSavedState(Obj *s)
{
    s32 n;
    if (take_flag(s)) {
        ActorShadow_Enable(s->unk320);
        if (!s->unk8) {
            u32 *q = &s->unk4->unk0;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            Inner *q = &s->unk4->inner;
            u32 mask = ~1;
            q->unk8 = mask & q->unk8;
        }
        {
            u16 one = 1;
            s->unk9C = one;
        }
        s->unk178 = 60;
    }
    {
        Anim *a = &s->unk4->anim;
        if (a->unk8 == 3 && a->unkE == 4)
            Sound_PlaySong(0xEE);
    }
    if (!peek_flag(s))
        return;
    {
        Ghoul_ResetRouteState(s);
        s->unk2CC = s->unk118;
        s->unk2D0 = s->unk11C;
        s->unk59 = s->unk120;
        s->unk2A8 = s->unk121;
        s->unk2A9 = s->unk122;
        s->unk2AA = s->unk123;
        s->unk21C = s->unk124;
        s->unk2B5 = s->unk128;
        s->unk2C0 = s->unk12E;
        s->unk2EA = s->unk12C;
        n = s->unkB0;
        if (n <= 0)
            n = 0;
        s->unk9C = n;
        s->unk2D6 = 0;
        s->unk2D4 = 0;
        s->unk2B0 = 0;
        s->unk2B1 = 1;
        s->unk2B2 = 1;
        s->unk2B3 = 0;
        s->unk304 = 0;
        s->unk2B6 = 0;
        s->unk2C2 = 64;
        s->unk2D8 = 0;
        s->unk2DC = 0;
        s->unk2E0 = 0;
        s->unk2E6 = 0;
        s->unk3CC = 0;
        s->unkDC = 0;
        SceneActorPalette_ClearCompletedTransitions();
    }
}

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

struct Seq {
    u8 pad0[7];
    u8 b7;
    u16 h8;
    u8 padA[4];
    u16 hE;
};

struct BatAnimationRequest {
    u8 pad0[4];
    u8 *inner;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Clr(u8 *q)
{
    ((struct Seq *)q)->hE = 0;
}

void Bat_UpdateAnimationRequest(u8 *p)
{
    struct Seq *s;

    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 5);
    s = (struct Seq *)(*(u8 **)(p + 4) + 0x48);
    if (s->h8 == 7 && s->hE == (s->b7 != 0) && s->b7 != 1) {
        s->h8 = 2;
        Clr(*(u8 **)(p + 4) + 0x48);
    }
}

/* Bat variant of SceneActorReaction_ResumeSavedTarget (tentative): after 60 frames it
 * resumes either the saved route target (kind 1) or, when flag 0x80 is set, group +0x48
 * (kind 0x1A); before that it rewinds a mode-2 animation. */

struct BatRoutePoint {
    u8 unknown0[0x1C];
    s16 x;
    u8 unknown1E[2];
    s16 z;
};

struct BatAnim {
    u8 unknown0[7];
    u8 loop;
    u16 mode;
    u8 unknownA[4];
    u16 frame;
};

struct BatDrawable {
    u8 unknown0[0x48];
    struct BatAnim anim;
};

void SceneActor_SetPositionXZ(void *, s32, s32);

static inline u32 Bat_MaskFlags(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void Bat_SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void Bat_ResumeSavedTarget(struct ChaseActor *actor, s32 frame)
{
    u8 *group = (u8 *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 12);
        actor->base.restart = 0;
    }
    if (frame > 60) {
        u16 blocked;
        if (!actor->base.restart)
            return;
        actor->base.stepDone = 1;
        blocked = Bat_MaskFlags(actor, 128);
        if (blocked) {
            void *target;
            SceneActor_SetPositionXZ(&actor->base.body.id,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = *(void **)(group + 0x48);
            Bat_SetControl(actor, 0x1A, target);
            {
                u32 behavior = 12;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u16 *flags = (u16 *)(group + 0x40);
                u16 mask = 4;
                *flags = mask | *flags;
            }
        } else {
            void *target;
            SceneActor_SetPositionXZ(&actor->base.body.id,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                     (*(struct BatRoutePoint **)(group + 0x1FC))->z);
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            target = actor->base.onAlternateEntry;
            Bat_SetControl(actor, 1, target);
            {
                u32 behavior = 7;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            {
                u32 mask = 16;
                actor->base.behaviorFlags |= mask;
            }
        }
    } else {
        struct BatAnim *anim = &((struct BatDrawable *)actor->base.drawable)->anim;
        if (anim->mode == 2) {
            u16 f = anim->frame;
            u8 loop = anim->loop;
            if (f == (loop != 0) && loop != 1)
                anim->frame = 0;
        }
    }
}
