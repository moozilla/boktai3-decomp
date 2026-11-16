#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_animation.h"

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void SceneActorAnimation_UpdateDirectionalEntry(u8 *p, s32 arg1)
{
    if (TakeFlag(p + 0x2B2)) {
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
        *(u8 *)(p + 0x2B3) = 0;
    }
    if (arg1 > 0x10 && *(u8 *)(p + 0x2B3) && *(u8 *)(p + 0x2B0) != 1) {
        *(u8 *)(p + 0x2B0) = 1;
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
    }
}

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

void Mummy_ResumeSavedTarget(struct ChaseActor *actor, s32 frame)
{
    struct BatAnim *anim = &((struct BatDrawable *)actor->base.drawable)->anim;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->base.restart = 0;
    }
    if (frame > 42) {
        u8 *group = (u8 *)&actor->group.common;
        u16 blocked;
        if (!actor->base.restart) {
            return;
        }
        actor->base.stepDone = 1;
        SceneActor_SetPositionXZ(&actor->base.body.id,
                                 (*(struct BatRoutePoint **)(group + 0x1FC))->x,
                                 (*(struct BatRoutePoint **)(group + 0x1FC))->z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        blocked = Bat_MaskFlags(actor, 128);
        if (blocked) {
            void *target = *(void **)(group + 0x48);
            Bat_SetControl(actor, 0x25, target);
            {
                u32 behavior = 12;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            *(u16 *)(*(u8 **)((u8 *)actor + 0x5D4) + 6) = 1;
            {
                u16 *flags = (u16 *)(group + 0x40);
                u32 mask = 4;
                *flags = (*(&mask)) | (*flags);
            }
        } else {
            void *target;
            u32 behavior = 7;
            actor->base.state2A8 = blocked;
            actor->base.phase2A9 = blocked;
            actor->base.behavior2AA = behavior;
            actor->base.counter2AC = blocked;
            actor->base.field2B1 = 1;
            target = actor->base.onAlternateEntry;
            Bat_SetControl(actor, 1, target);
            *(u16 *)(*(u8 **)((u8 *)actor + 0x5D4) + 6) = blocked;
            actor->base.field2C2 = Div(64, 3);
            {
                u32 mask = 16;
                u32 *flags = &actor->base.behaviorFlags;
                *flags |= mask;
            }
        }
    } else if (anim->mode == 2) {
        u16 frame = anim->frame;
        u32 loop = anim->loop;
        if (loop) {
            if (frame == (loop - 1)) {
                goto rewind;
            }
            return;
        } else if (frame) {
            return;
        }
    rewind:
        anim->frame = 0;
    }
}

// confidence=low: hidden recovery disables the shadow and waits to enter revival.

static inline void Mummy_GetDirectionVariant(struct ChaseActor *actor, u8 *variant, u8 *mirror,
                                             u32 zero)
{
    s32 quadrant = ((actor->base.body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = zero;
        *variant = quadrant;
    }
}

static inline void Mummy_SelectAnimation(struct ChaseActor *actor, u32 selection, u32 variant,
                                         u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->base.drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->base.animationIndex = selection;
        drawable = actor->base.drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->base.restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->base.animationIndex;
        restart = &actor->base.restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(
                &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
                actor->base.drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

static inline u8 TakeSaved(u8 *flag, u8 **saved)
{
    u32 value = *flag;
    *saved = flag;
    if (value) {
        **saved = 0;
        return 1;
    }
    return 0;
}

void Mummy_UpdateRevival(void *);

void Mummy_UpdateHiddenRecovery(struct ChaseActor *actor, s32 frame)
{
    u32 queued;
    u8 *prepared;
    u32 mask = 1;
    if (!actor->base.drawableKind) {
        u32 *flags = actor->base.drawable;
        *flags |= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        tile->flags |= mask;
    }
    queued = TakeSaved(&actor->base.field2B2, &prepared);
    if (queued) {
        ActorShadow_Disable((u8 *)actor + 0x320);
    } else if (frame > 0x1A) {
        void *callback;
        u32 kind, zero;
        {
            u8 directionVariant, mirror;
            Mummy_GetDirectionVariant(actor, &directionVariant, &mirror, queued);
            Mummy_SelectAnimation(actor, 8, directionVariant, mirror | 4);
        }
        callback = Mummy_UpdateRevival;
        kind = 13;
        zero = 0;
        *prepared = 1;
        actor->base.stepDone = zero;
        actor->base.controlKind = kind;
        actor->base.target = callback;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
    } else
        actor->base.restart = queued;
}

/* Observed in this callback: owner flags at +0 and inner +0x20/+8.
 * The snapshot fields below are copied and reset at the listed offsets. */

typedef struct Inner {
    u8 pad0[8];
    u32 unk8;
} Inner;

typedef struct Owner {
    u32 unk0;
    u8 pad4[0x1c];
    Inner inner;
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

void Mummy_ResetRevivalMotion(struct ChaseActor *actor);

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

void Mummy_UpdateRevival(void *sIn)
{
    Obj *s = (Obj *)sIn;
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
    if (!peek_flag(s))
        return;
    {
        Mummy_ResetRevivalMotion(s);
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

s32 Mod(s32, s32);

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

struct S;

void Golem_UpdateDeath_080D211C(struct S *p, s32 n);

struct Sub {
    u8 f0[8];
    u16 h8;
    u8 fA[4];
    u16 hE;
};

struct Ctx {
    u8 f0[0x48];
    struct Sub sub;
};

void Mummy_UpdateDying(u8 *s, s32 arg)
{
    struct Sub *c = &(*(struct Ctx **)(s + 4))->sub;
    u8 *part = s + 0x3D4;
    s32 zero;
    u32 mask;
    s32 off;
    if (TakeFlag(s + 0x2B2)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry(s, 8);
        test = 2;
        flags = (u16 *)(s + 0x2D4);
        value = *flags;
        test &= value;
        if (test == 0) {
            test = 1;
            test |= value;
            *flags = test;
        }
    }
    if (Mod(arg, 60) == 0) {
        SceneActorAttachment_ReplaceResponseSlot(s, 2, 0, s[0x98]);
    }
    off = 0xF9;
    off <<= 1;
    if ((*((s16 *)(part + off))) <= 0) {
        mask = 0x80;
        mask <<= 5;
        off += 0xDE;
        zero = (*((u32 *)(s - (-off)))) & mask;
        if (zero == 0) {
            void (*fn)(void) = Golem_UpdateDeath_080D211C;
            u32 code = 6;
            u32 *flags;
            u32 clear;
            s[0x2B2] = 1;
            s[0x2B0] = zero;
            s[0x2B5] = code;
            *((void (**)(void))(s + 0x21C)) = fn;
            *((u16 *)(s + 0x2C8)) = zero;
            flags = (u32 *)(s + 0x114);
            clear = ~1;
            *flags &= clear;
            *((u32 *)(s + 0x104)) = zero;
            return;
        }
    }
    if (c->h8 == 2)
        c->hE = 0;
}

/* Golem_UpdateDeath_080D211C: Golem_UpdateDeath variant: entry 9, and resets the facing entry while +0x2B3 is set (provisional name) */

static inline u32 orr(u32 k, u16 *a)
{
    return k | *a;
}

struct S {
    u8 f0[0x2b0];
    u8 b0;
    u8 f1;
    u8 b2;
    u8 b3;
    u8 f2[0x20];
    u16 h4;
    u16 h6;
};

static inline u8 tk(u8 *a)
{
    if (*a) {
        *a = 0;
        return 1;
    }
    return 0;
}

void Golem_UpdateDeath_080D211C(struct S *p, s32 n)
{
    if (tk(&p->b2)) {
        s32 m;
        u16 *a;
        SceneActorAnimation_SelectDirectionalEntry(p, 9);
        a = &p->h4;
        m = -2;
        *a = m & *a;
    }
    if (n == 0x28) {
        u16 k = 0x10;
        p->h6 = k | p->h6;
    }
    if (p->b3) {
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
        if (n > 0x3C)
            p->b0 = 1;
    }
}
