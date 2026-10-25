// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 95.5%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: address scheduling, inlined tests and register allocation.
#include "scene_actor_reset.h"
#include "actor_animation_controller.h"
void Ghost_GetDirectionalFrame(u16, u32 *, u32 *);
s32 Mod(s32, s32);
struct GhostVisual {
    struct ActorSprite sprite;
    u8 pad24[0x48 - 0x24];
    struct ActorAnim anim;
    void *resource;
    u16 life, unknown5E;
};
struct GhostVisualContext {
    u8 pad[0x3D4];
    struct GhostVisual visuals[4];
    u16 index, interval;
};
static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
static inline void Step(struct GhostVisual *visual)
{
    struct ActorSprite *o = &visual->sprite;
    struct ActorAnim *a = &visual->anim;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != (((*entry & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else {
            if (++a->frame >= a->count) {
                if ((u8)(a->flags & 8)) {
                    a->timer = a->rate;
                    a->frame = a->count - 1;
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
            } else
                done = 0;
        }
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
}
// confidence=low: refreshes one of four timed Ghost afterimage sprites at the actor's position.
inline static void Step_1_1(struct ActorSprite *o, struct ActorAnim *a, struct GhostVisual *visual)
{
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    entry = a->data + a->frame;
    o->frame = (*entry) >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
        if (left != ((((*entry) & 0x30) >> 4) & mask))
            o->flags |= 4;
        else
            o->flags &= ~4;
    }
    {
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        u32 left = (u8)(mask & input);
        if (left != ((((*entry) & 0x30) >> 4) & mask))
            o->flags |= 8;
        else
            o->flags &= ~8;
    }
    n = ++a->timer;
    if (n >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8) {
                    a->timer = a->rate;
                } else {
                    a->frame = a->count - 1;
                }
                done = 1;
            } else {
                a->frame--;
                done = 0;
            }
        } else if ((++a->frame) >= a->count) {
            if ((u8)(a->flags & 8)) {
                a->timer = a->rate;
                a->frame = a->count - 1;
                done = 1;
            } else {
                a->frame = 0;
                done = 1;
            }
        } else
            done = 0;
        entry = a->data + a->frame;
        a->factor = frame_factor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
}

void Ghost_EmitAfterimage(struct SceneActorRegistryResetView *actor)
{
    struct GhostVisualContext *ctx = actor->context;
    u32 variant;
    u32 flags;
    if (Mod(actor->field2CA, ctx->interval) == 0) {
        Ghost_GetDirectionalFrame(actor->direction, &variant, &flags);
        {
            struct ActorAnim *source = (void *)(((u8 *)actor->drawable) + 0x48);
            u32 sequence = source->unused;
            {
                u32 k1 = 0x42C;
                Anim_SetSequence(&ctx->visuals[ctx->index].anim,
                                 *((void **)((ctx->index * 0x60) + ((u32)(((u8 *)ctx) + k1)))),
                                 sequence, (u8)variant, (u8)flags);
            }
        }
        Step_1_1(&(&ctx->visuals[ctx->index])->sprite, &(&ctx->visuals[ctx->index])->anim,
                 &ctx->visuals[ctx->index]);
        *((struct ContactPosition *)(((u32)(((u8 *)ctx) + 0x3F0)) + (ctx->index * 0x60))) =
            actor->position;
        ctx->visuals[ctx->index].sprite.flags &= ~1;
        ctx->visuals[ctx->index].life = 8;
        if ((++ctx->index) > 3)
            ctx->index = 0;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Ghost_EmitAfterimage);
#endif
