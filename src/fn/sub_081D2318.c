#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "player.h"
#include "engine.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Npc {
    u8 unk_00[7];
    u8 done;
    u8 unk_08;
    u8 entered;
    u8 unk_0A;
    u8 attachmentEnabled;
    u8 unk_0C[2];
    u16 displayFlags;
    u8 unk_10[9];
    u8 facing4, facing8;
    u8 unk_1B[0x65 - 0x1B];
    u8 angle;
    u8 unk_66[2];
    struct Vec position;
    u8 unk_70[0x1F0 - 0x70];
    u8 animationStarted;
    u8 unk_1F1[0x225 - 0x1F1];
    u8 savedAngle;
    u8 unk_226[2];
    s32 interactionRadius;
};
struct Context {
    u8 unk_00[0x434];
    struct Vec *target;
    struct Player *targetActor;
};
s32 Math_Atan2(s32, s32);
void TaggedSceneNpc_PlayTaggedAnimation(void *, s32, s32, s32, s32);
void Actor_SetAttachmentEffect(void *, s32, s32, s32);
static inline s32 Abs(s32 x)
{
    if (x < 0)
        return -x;
    return x;
}
static inline u32 Flag(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline s32 AllowsFacing(struct Player *p)
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
// confidence=low: turns toward a conversation target and marks nearby facing agreement.

void TaggedSceneNpc_UpdateTargetFacing(struct Context *ctx, struct Npc *p)
{
    struct Vec *pos = &p->position;
    s32 facing;
    if (ctx->target) {
        struct Vec *target = ctx->target;
        s32 angle = Math_Atan2(target->x - pos->x, target->z - pos->z);
        {
            s32 diff = (s8)(angle - p->angle);
            s32 value;
            register s32 octant asm("r2");
            s32 mask;
            s32 quadrantMask;
            if (diff > 8) {
                value = p->angle;
                value += 8;
                p->angle = value;
            } else {
                if (diff < -8) {
                    value = p->angle;
                    value -= 8;
                } else {
                    value = p->angle;
                    value += diff;
                }
                p->angle = value;
                /* Retain the low halfword before deriving the facing indices. */
                value = (u32)value << 16;
                value = (u32)value >> 16;
            }
            octant = value;
            octant += 112;
            octant >>= 5;
            mask = 7;
            mask &= octant;
            p->facing8 = mask;
            value += 96;
            value >>= 6;
            quadrantMask = 3;
            quadrantMask &= value;
            p->facing4 = quadrantMask;
        }
        /* The target actor stores its heading in this byte. */
        facing = (u8)ctx->targetActor->unk2D;
    } else
        facing = 0;
    if (p->entered) {
        p->entered = 0;
        p->done = 0;
        TaggedSceneNpc_PlayTaggedAnimation(p, 0, 1, 0, 4);
        p->animationStarted = 1;
        {
            u8 angle = p->angle;
            p->savedAngle = angle;
        }
        if (((ctx->targetActor && !AllowsFacing(ctx->targetActor)) && (!(gEngineFlags & 0x200))) &&
            (p->attachmentEnabled)) {
            Actor_SetAttachmentEffect(p, 3, 0, 0);
        }
    } else
        TaggedSceneNpc_PlayTaggedAnimation(p, 0, 1, 0, 1);
    {
        u32 mask = 0x1000;
        p->displayFlags |= mask;
    }
    {
        struct Vec *target = ctx->target;
        s32 radius = p->interactionRadius;
        u8 near;
        if (((Abs(target->x - pos->x) < radius) && (Abs(target->y - pos->y) <= 255)) &&
            (Abs(target->z - pos->z) < radius))
            near = 1;
        else
            near = 0;
        if (near) {
            struct Vec *destination = ctx->target;
            s32 direction = (s8)Math_Atan2(pos->x - destination->x, pos->z - destination->z);
            s32 delta = facing - direction;
            s32 oppositeFacing = (p->angle) + 128;
            u8 opposite;
            opposite = facing - oppositeFacing;
            if ((Abs((s8)delta) <= 63) && (Abs((s8)opposite) <= 31)) {
                u32 mask = 0x800;
                p->displayFlags |= mask;
            }
        }
    }
}
