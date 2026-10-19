// MATCHING_CONCESSION: register-pinning
#include "global.h"
/* Vanargand_RunDebugSlam: debug-positioned (A/start) leap that lands on the player, plays the slam sprite sequences, flashes the screen, shakes and applies contact damage while the effect stage runs, then recovers (provisional name) */
#define Vanargand_RunDebugSlam sub_081ABA64
struct VanargandAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame;
    u16 padA;
    u16 scale;
    u16 timer;
    void *sequence;
};
struct VanargandSprite {
    u32 flags;
    u8 pad4[0xC];
    u16 frame;
    u8 pad12[0xA];
    u8 effect[0x10];
    u8 resource[0x38];
    struct VanargandAnim anim;
};
struct VanargandBody {
    u8 animation[0x20];
    u8 afterimageSource[0x40];
    u8 drawable[0x30];
    u8 action[0x14];
    u8 variant;
    u8 effectState;
    u8 padA6[2];
    struct VanargandSprite sprite;
    u8 pad120[0x128 - 0x120];
    s16 direction[4];
};
struct VanargandPart {
    u8 pad0[0x48];
    u8 animation[0x10];
    void *sequence;
    u32 field5C;
};
struct VanargandPoint {
    s16 x, y, z, pad;
};
struct VanargandDirection {
    s16 x, y;
    u8 pad[4];
};
struct Vanargand {
    u8 pad0[0x54];
    s16 position[4];
    s16 velocity[4];
    u8 motionData[0x9C - 0x64];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0xCC - 0xB3];
    u8 heading;
    u8 padCD[0x104 - 0xCD];
    void *callback;
    u8 pad108[0x10C - 0x108];
    void *player;
    u8 pad110[0x18C - 0x110];
    struct VanargandBody body;
    u8 pad2BC[0x3FC - 0x2BC];
    struct VanargandPart part;
    u8 pad45C[0x500 - 0x45C];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x58E - 0x570];
    u16 contactDamage;
    u8 pad590[0xF18 - 0x590];
    u32 fieldF18, fieldF1C;
    u16 fieldF20, fieldF22;
    u32 fieldF24;
    u8 padF28[0xF44 - 0xF28];
    void *afterimages;
};
extern u16 gUnk_03005260[];
extern const s16 gUnk_086149C4[];
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
u32 Callback_InvokeActor(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
void Anim_SetSequence(void *, void *, int, int, int);
void Sprite_ApplyAnimation(void *, void *);
void Sound_PlaySong(u32);
void Debug_NudgeWorldPositionWithA(void *);
s32 sub_081A6AEC(void *);
u32 sub_081A6A08(void *);
void VanargandAfterimagePool_Spawn(void *, void *);
void Vanargand_StartActionSequence(void *);
void Vanargand_CreateTransientEffect(void *, u8);
void Vanargand_AdvanceEffectStageOne(void *);
void Vanargand_AdvanceEffectStageThree(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 PlayerContact_ApplySyntheticContact(void *, s32, u32, u32, u32, u32);

static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}

static inline s32 half(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}

static inline s32 scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline void setDirection(s16 *direction, u32 packed)
{
    struct VanargandDirection v;
    s32 a, b;
    *(u32 *)&v = packed;
    a = v.x;
    b = v.y;
    direction[0] = half((a + b) * 32 / 3 + 0x40);
    direction[1] = 0x40;
    direction[2] = half((b - a) * 32 / 3 + 0x40);
}

static inline u32 frameFactor(u16 v)
{
    u32 value = v;
    u32 mask = 15;
    return mask & value;
}

static inline u8 step(struct VanargandSprite *o, struct VanargandAnim *a)
{
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        u32 mask = 1;
        u32 left = (u8)(input & mask);
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
                if (a->flags & 8)
                    a->timer = a->rate;
                else
                    a->frame = a->count - 1;
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
        a->factor = frameFactor(*entry);
        a->rate = (a->factor * a->scale) >> 6;
        if (!a->rate)
            a->rate = 1;
    } else
        done = 0;
    return done;
}

static inline void polar(s16 *out, u32 angle, s32 distance)
{
    out[0] = scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * distance);
    out[1] = 0;
    out[2] = scale12(gUnk_086149C4[angle] * distance);
}

static inline void pulse(struct Vanargand *q, u16 a, u16 b)
{
    q->fieldF1C = 0;
    q->fieldF20 = a;
    q->fieldF22 = b;
}

static inline void descend(struct Vanargand *p, struct Vanargand *q)
{
    struct VanargandPoint origin;
    struct VanargandPoint *o = &origin;
    s32 x = p->position[0], y = p->position[1], z = p->position[2];
    o->x = x - 0x100;
    o->y = y;
    o->z = z - 0x100;
    MotionTrack_Initialize(q->track, p->position, &origin, 0, 4);
}

static inline void showFrame(u16 frame, struct VanargandSprite *sprite)
{
    Sprite_ApplyAnimation(sprite, sprite->resource);
    sprite->frame = frame;
}

static inline void setVelocity(s16 *velocity, s32 x, s32 y, s32 z)
{
    velocity[0] = x;
    velocity[1] = y;
    velocity[2] = z;
}

static inline void advance(struct Vanargand *p, u8 phase)
{
    p->phase = phase;
    p->timer = 0;
}

static inline void setFlags(struct Vanargand *p, u32 mask)
{
    p->flags |= mask;
}

static inline void startPart(struct Vanargand *q, int sequence)
{
    struct VanargandPart *part = &q->part;
    Anim_SetSequence(part->animation, part->sequence, sequence, 0, 0);
    q->partActive = 1;
    part->field5C = 0;
}

static inline void applyContact(struct Vanargand *p, struct Vanargand *q)
{
    if (sub_081A6A08(p)) {
        if (q->body.variant == 0)
            PlayerContact_ApplySyntheticContact(p->player, q->contactDamage, 2, 0x20, 0, 1);
        else
            PlayerContact_ApplySyntheticContact(p->player, q->contactDamage, 2, 0x20, 0, 2);
    }
}

void sub_081ABA64(struct Vanargand *p)
{
    struct Vanargand *q = p;
    struct VanargandBody *body = &p->body;
    {
        u32 mask = 8;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 1);
    if ((u8)Callback_InvokeActor(p, p->callback))
        return;
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        PairedSceneActor_ApplyToSelectedHalves(q->attachment, 3, 0x13);
        p->phase = 1;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            Debug_NudgeWorldPositionWithA(p->position);
            if (gUnk_03005260[1] & 0x100) {
                if (gUnk_03005260[0] & 1)
                    body->variant = 1;
                else
                    body->variant = 0;
                advance(p, 2);
            }
            break;
        case 1:
            if (p->timer == 0) {
                s16 target[4];
                s32 distance;
                u32 angle;
                SceneActors_StartUnmirroredAnimation(p, body, body->drawable, 2, 1, 0);
                distance = sub_081A6AEC(p);
                if (distance > 0x200)
                    distance = 0x200;
                angle = p->heading;
                if ((u32)(angle - 0x20) <= 0x7F)
                    angle = 0x60;
                else
                    angle = 0xE0;
                polar(target, angle, distance);
                target[0] += 0x380;
                target[1] = 0x300;
                target[2] += 0x380;
                MotionTrack_Initialize(q->track, p->position, target, 0, 4);
                p->timer++;
                startPart(q, 0);
            }
            MotionTrack_Update(q->track);
            if (complete(q))
                advance(p, 2);
            else
                VanargandAfterimagePool_Spawn(q->afterimages, q->body.afterimageSource);
            break;
        case 2:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, body, body->drawable, 0xA, 2, 0);
                if (body->variant == 0) {
                    pulse(q, 2, 6);
                } else {
                    pulse(q, 3, 6);
                }
                q->fieldF18 = 0;
            }
            if (p->timer == 7) {
                struct VanargandSprite *sprite = &body->sprite;
                showFrame(0x18, sprite);
                setDirection(body->direction, 0);
            } else if (p->timer == 10) {
                setDirection(body->direction, 0x10000);
            }
            if (p->timer > 6)
                setFlags(p, 0x80000);
            if (p->ready)
                advance(p, 3);
            else
                p->timer++;
            break;
        case 3:
            setFlags(p, 0x80000);
            if (p->timer == 0) {
                struct VanargandSprite *sprite = &body->sprite;
                Anim_SetSequence(&body->sprite.anim, sprite->anim.sequence, 3, 0, 0);
                p->timer++;
            }
            {
                u32 t = p->timer;
                if (t == 1) {
                    if (step(&body->sprite, &body->sprite.anim))
                        p->timer++;
                } else {
                    p->timer = t + 1;
                    if (t + 1 > 10)
                        advance(p, 4);
                }
            }
            break;
        case 4: {
            struct VanargandSprite *sprite = &body->sprite;
            struct VanargandAnim *anim = &body->sprite.anim;
            setFlags(p, 0x80000);
            if (p->timer == 0) {
                Anim_SetSequence(anim, sprite->anim.sequence, 4, 0, 0);
                p->timer++;
                if (q->fieldF24 == 1)
                    Sound_PlaySong(0x543);
            }
            if (!step(sprite, anim) || p->timer <= 10)
                p->timer++;
            else
                advance(p, 5);
            break;
        }
        case 5:
            setFlags(p, 0x80000);
            if (p->timer == 0) {
                struct VanargandSprite *sprite;
                struct VanargandAnim *anim;
                SceneActors_StartUnmirroredAnimation(p, body, body->drawable, 0xB, 2, 0);
                sprite = &body->sprite;
                anim = &body->sprite.anim;
                Anim_SetSequence(anim, sprite->anim.sequence, 5, 0, 0);
                setDirection(body->direction, 0x30000);
                step(sprite, anim);
                Vanargand_StartActionSequence(q->body.action);
                Vanargand_CreateTransientEffect(body->sprite.effect, body->variant);
                Sound_PlaySong(0x584);
            }
            if (++p->timer > 0x3C) {
                advance(p, 6);
                if (q->body.variant == 0)
                    ScreenTransition_SendCommand(0, 3, 0x1F, 0x1F, 0x1F, 0xFFFF, 0);
                else
                    ScreenTransition_SendCommand(0, 3, 4, 4, 4, 0xFFFF, 0);
            }
            break;
        case 6:
            setFlags(p, 0x80000);
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, body, body->drawable, 0xB, 2, 0);
                Vanargand_AdvanceEffectStageOne(q->body.action);
                descend(p, q);
            }
            step(&body->sprite, &body->sprite.anim);
            applyContact(p, q);
            MotionTrack_Update(q->track);
            if (q->body.effectState == 3)
                advance(p, 7);
            else
                p->timer++;
            break;
        case 7:
            setFlags(p, 0x80000);
            if (p->timer == 0)
                SceneActors_StartUnmirroredAnimation(p, body, body->drawable, 0xB, 2, 0);
            step(&body->sprite, &body->sprite.anim);
            if (p->timer <= 0x3B) {
                applyContact(p, q);
                if (p->timer & 1) {
                    setVelocity(p->velocity, -2, 0, -2);
                }
            }
            if (p->timer == 0x3C)
                Vanargand_AdvanceEffectStageThree(q->body.action);
            if (q->body.effectState == 0)
                advance(p, 8);
            else
                p->timer++;
            break;
        case 8:
            setFlags(p, 0x80000);
            if (p->timer == 0) {
                struct VanargandSprite *sprite = &body->sprite;
                Anim_SetSequence(&body->sprite.anim, sprite->anim.sequence, 6, 0, 0);
                p->timer++;
            }
            if (p->timer == 1) {
                if (step(&body->sprite, &body->sprite.anim))
                    p->timer++;
            } else {
                setVelocity(p->velocity, p->timer, 0, p->timer);
                if (++p->timer > 15)
                    advance(p, 9);
            }
            break;
        case 9:
            if (p->timer == 0) {
                struct VanargandSprite *sprite;
                SceneActors_StartUnmirroredAnimation(p, body, body->drawable, 0xA, 2, 2);
                sprite = &body->sprite;
                showFrame(0x18, sprite);
                setDirection(body->direction, 0x10000);
                if (body->variant == 0) {
                    pulse(q, 4, 5);
                } else {
                    pulse(q, 5, 5);
                }
                q->fieldF18 = 0;
                startPart(q, 4);
            }
            if (p->timer == 3) {
                setDirection(body->direction, 0);
            }
            if (p->timer <= 6)
                setFlags(p, 0x80000);
            if (p->ready)
                advance(p, 10);
            else
                p->timer++;
            break;
        case 10:
            p->finished = 1;
            advance(p, 0);
            break;
    }
}
