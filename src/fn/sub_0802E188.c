#include "global.h"
#include "input.h"
// MATCHING_CONCESSION: register-pinning
struct Position {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Words4 {
    u32 values[4];
};
struct Actor {
    u8 pad0[0xA];
    u8 cue;
    u8 padB[3];
    u16 flags;
    u8 pad10[0x40 - 0x10];
    u32 script;
    struct Words4 parameters;
    u8 pad54[8];
    u32 identity;
    u8 pad60[8];
    struct Position position;
    u8 pad70[0x238 - 0x70];
    s8 cues[4];
    u8 pad23C[0x2EC - 0x23C];
    struct Actor *next;
};
struct Context {
    u8 pad0[0x1C];
    struct Actor *head;
    u8 pad20[0x434 - 0x20];
    struct Position *target;
    u8 *targetActor;
};
extern u8 *gUnk_02000580[];
s32 Scene_CanOpenMenu(void);
void Actor_SetAttachmentEffect(void *, s32, s32, s32);
void Script_Call(u32, void *);
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
static inline u32 Flag(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline s32 PlayerKind(void)
{
    u8 *player = gUnk_02000580[0];
    register u32 offset asm("r1") = 0x457;
    asm("" ::"r"(offset));
    return player[offset];
}
// confidence=low: highlights the nearest eligible actor and invokes its script on input.
// The mask as an inline argument matches (found by tools/fix.py).
static inline u32 ActorFlag(u32 mask, struct Actor *actor)
{
    return actor->flags & mask;
}
#define SceneActorAnimation_UpdateInteractionCue sub_0802E188
void SceneActorAnimation_UpdateInteractionCue(struct Context *input)
{
    struct Context *context = input;
    s32 buttons = gPadState[1];
    s32 pressed = 1;
    s32 kind;
    struct Actor *nearest;
    struct Actor *actor;
    s32 playerKind;
    {
        s32 scratch = pressed;
        scratch &= buttons;
        pressed = scratch;
    }
    playerKind = PlayerKind();
    if (playerKind == 10)
        goto disableInput;
    if (playerKind != 11)
        goto inputReady;
disableInput: {
    register s32 zero asm("r6") = 0;
    asm("" ::"r"(zero));
    pressed = zero;
}
inputReady:;
    {
        register u8 *rawTarget asm("r0") = context->targetActor;
        u8 *target;
        if (!rawTarget)
            goto blocked;
        target = rawTarget;
        if ((u8)(target[0x418] - 3) <= 2)
            goto blocked;
        if (Flag((u32 *)(target + 0x20), 0x100000))
            goto blocked;
        if (U16(target, 0x538) && (u16)(U16(target, 0x534) - 5) <= 1)
            goto blocked;
        {
            u8 mode = target[0x457];
            u8 allow = 1;
            if (mode > 1 && mode != 7)
                allow = 0;
            if (!allow)
                goto blocked;
        }
        {
            u8 state = context->targetActor[0x418];
            kind = 0;
            if (state == 0)
                goto selected;
            kind = 1;
            if (state == 1)
                goto selected;
            kind = 3;
            if (state != 6)
                goto selected;
            kind = 2;
            goto selected;
        }
    }
blocked:
    kind = 3;
selected:
    nearest = 0;
    if (Scene_CanOpenMenu()) {
        s32 best = 1000000;
        actor = context->head;
        while (actor) {
            if (ActorFlag(0x800, actor) && actor->script) {
                struct Position *position = &actor->position;
                struct Position *target = context->target;
                s32 x = target->x - position->x;
                s32 z = target->z - position->z;
                s32 distance = (s32)((u32)x * x + (u32)z * z);
                if (distance < best) {
                    best = distance;
                    nearest = actor;
                }
            }
            actor = actor->next;
        }
    }
    if (nearest && !pressed) {
        actor = context->head;
        if (actor) {
            s8 *base = nearest->cues;
            s32 index;
            s8 *cue;
            u32 blockedMask;
            index = kind;
            cue = base + index;
            blockedMask = 0x2000;
            asm("" ::"r"(cue), "r"(blockedMask));
            do {
                if (actor == nearest && !(nearest->flags & blockedMask)) {
                    if (!nearest->cue) {
                        Actor_SetAttachmentEffect(nearest, *cue, 180, 180);
                        nearest->cue = 1;
                    }
                } else if (actor->cue) {
                    Actor_SetAttachmentEffect(actor, 8, 180, 180);
                    actor->cue = 0;
                }
                actor = actor->next;
            } while (actor);
        }
    } else {
        actor = context->head;
        while (actor) {
            if (actor->cue) {
                Actor_SetAttachmentEffect(actor, 8, 180, 180);
                actor->cue = 0;
            }
            actor = actor->next;
        }
    }
    if (nearest && pressed) {
        struct {
            u32 identity;
            s32 x, y, z;
            struct Words4 parameters;
        } operands;
        operands.identity = nearest->identity;
        operands.x = nearest->position.x;
        operands.y = nearest->position.y;
        operands.z = nearest->position.z;
        {
            u32 *source = nearest->parameters.values;
            u32 *destination = operands.parameters.values;
            s32 count = 3;
            do {
                *destination++ = *source++;
            } while (--count >= 0);
        }
        // The prepared operands are retained even though dispatch uses a null argument block.
        Script_Call(nearest->script, 0);
    }
}
