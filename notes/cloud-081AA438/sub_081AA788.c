#include "global.h"
#ifdef NONMATCHING
// fuzzy: 75.0%
// Vanargand projectile volley. Logic and instruction count match (714 vs 715); what differs is
// allocation: the ROM spills the copied actor pointer to sp+0x30 and the motion position to
// sp+0x48, keeps the half-select table copy at sp+0x1C (mine lands at sp+0x24, after the
// descent origin), and keeps the random table in r6 once the actor pointer is dead.
/* Vanargand_RunProjectileVolley: waits for the paired halves, plays the attack animation, then alternates rising/descending motion tracks firing a projectile at a live half (or straight ahead) each lap until four laps are done (provisional name) */
#define Vanargand_RunProjectileVolley sub_081AA788
struct VanargandPart {
    u8 pad0[0x48];
    u8 animation[0x10];
    void *sequence;
    u32 field5C;
};
struct VanargandHalf {
    u8 pad0[0xB4];
    s16 position[4];
    u8 padBC[0x20C - 0xBC];
    s16 health;
    u8 pad20E[0x21C - 0x20E];
};
struct Vanargand {
    u8 pad0[0x54];
    s16 position[4];
    u8 motionData[0x9C - 0x5C];
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
    u8 pad108[0x110 - 0x108];
    void *target;
    u8 pad114[0x18C - 0x114];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad45C[0x500 - 0x45C];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    struct VanargandHalf *attachment;
    u8 pad570[0x57C - 0x570];
    void *projectiles;
    u8 pad580[0x5B0 - 0x580];
    u16 delay;
    u8 pad5B2[0x5B4 - 0x5B2];
    s16 laps;
    u8 pad5B6[0x5C7 - 0x5B6];
    u8 difficulty;
};
struct VanargandHalfSelect {
    u32 index[2];
};
extern const struct VanargandHalfSelect gUnk_0824F788;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
u32 Callback_InvokeActor(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
u32 PairedSceneActor_AreSelectedHalvesFinished(void *);
s32 ScenePair_CountPositiveCounters(void *);
void Anim_SetSequence(void *, void *, int, int, int);
void Sprite_ApplyAnimation(u8 *, u8 *);
void Sound_PlaySong(u32);
s32 sub_08249350(s32, s32);
s32 VanargandProjectile_Create(void *, void *, void *, u32, u32, u32);
s32 sub_081B71D4(void *, void *, void *, void *, u32, u32, u32);

static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}

static inline s32 nextRandom(const u16 *table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)((u32)table + offset);
}

static inline s32 half(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}

static inline void setOrigin(s16 *origin, s32 x, s32 y, s32 z)
{
    s16 *q = origin;
    *q = x;
    origin[1] = y;
    origin[2] = z;
}

static inline s32 randomValue(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}

static inline void fireAimed(void *target, void *aim, struct Vanargand *p, struct Vanargand *self,
                             s16 *position, s32 speed)
{
    u8 heading = p->heading;
    const u16 *table = gUnk_0203B400;
    s32 offset = (nextRandom(table) >> 3) % 32 - 16;
    s32 dir = heading + offset;
    sub_081B71D4(self->projectiles, position, target, aim, dir, speed,
                 sub_08249350(nextRandom(table) >> 3, 6));
}

static inline void fire(void *target, struct Vanargand *p, struct Vanargand *self, s16 *position,
                        s32 speed)
{
    u8 heading = p->heading;
    const u16 *table = gUnk_0203B400;
    s32 offset = (nextRandom(table) >> 3) % 32 - 16;
    s32 dir = heading + offset;
    VanargandProjectile_Create(self->projectiles, position, target, dir, speed,
                               sub_08249350(nextRandom(table) >> 3, 6));
}

static inline void fireVolley(struct Vanargand *p, void *obj, s16 *position)
{
    struct Vanargand *self = obj;
    struct VanargandHalfSelect select;
    s32 speed;
    select = gUnk_0824F788;
    if (p->difficulty > 1)
        speed = (randomValue() >> 3) % 8 + 0x14;
    else
        speed = (randomValue() >> 3) % 8 + 0xC;
    if (ScenePair_CountPositiveCounters(self->attachment) == 0)
        speed += 8;
    {
        struct VanargandHalf *half0 = &self->attachment[select.index[self->laps & 1]];
        if (half0->health > 0) {
            fireAimed(p->target, half0->position, p, self, position, speed);
        } else {
            struct VanargandHalf *half1 = &self->attachment[select.index[(self->laps + 1) & 1]];
            if (half1->health > 0)
                fireAimed(p->target, half1->position, p, self, position, speed);
            else
                fire(p->target, p, self, position, speed);
        }
    }
}

static inline void startDescent(struct Vanargand *p, struct Vanargand *q)
{
    s16 origin[4];
    s32 x = p->position[0], y = p->position[1], z = p->position[2];
    origin[0] = x + 0x100;
    origin[1] = y;
    origin[2] = z + 0x100;
    if (q->difficulty > 1)
        MotionTrack_Initialize(q->track, p->position, origin, 0, 2);
    else
        MotionTrack_Initialize(q->track, p->position, origin, 0, 4);
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

void sub_081AA788(struct Vanargand *p)
{
    struct Vanargand *q = p;
    u8 *animation = p->animation;
    s16 origin[4];
    s16 delta[4];
    {
        u32 mask = 0x40;
        p->flags |= mask;
    }
    Vanargand_SetPendingCue(p, 0);
    if ((u8)Callback_InvokeActor(p, p->callback))
        return;
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        p->laps = 0;
        PairedSceneActor_ApplyToSelectedHalves(p->attachment, 3, 0x13);
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (PairedSceneActor_AreSelectedHalvesFinished(q->attachment)) {
                PairedSceneActor_ApplyToSelectedHalves(q->attachment, 3, 2);
                advance(p, 1);
            }
            break;
        case 1:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, animation, animation + 0x60, 8, 2, 0);
                p->timer++;
                {
                    struct VanargandPart *part = &q->part;
                    Anim_SetSequence(part->animation, part->sequence, 0, 0, 0);
                    q->partActive = 1;
                    part->field5C = 0;
                }
            }
            if (p->ready) {
                advance(p, 2);
            }
            break;
        case 2: {
            u32 timer;
            u32 mask;
            setFlags(p, 0x80000);
            if (p->timer <= 3)
                q->delay = 8;
            timer = ++p->timer;
            mask = 0x10;
            if (q->difficulty > 1)
                mask = 8;
            if (timer & mask) {
                advance(p, 3);
            }
            break;
        }
        case 3: {
            u8 frame;
            u8 *sprite;
            setFlags(p, 0x180000);
            if (p->timer == 0) {
                s16 *dir;
                s32 a, b;
                setOrigin(origin, p->position[0] - 0x100, p->position[1], p->position[2] - 0x100);
                MotionTrack_Initialize(q->track, p->position, origin, 0, 1);
                SceneActors_StartUnmirroredAnimation(p, animation, animation + 0x60, 9, 2, 0);
                dir = (s16 *)(animation + 0x128);
                *(u32 *)delta = 0;
                a = delta[0];
                b = delta[1];
                dir[0] = half((a + b) * 32 / 3 + 0x40);
                dir[1] = 0x40;
                dir[2] = half((b - a) * 32 / 3 + 0x40);
                fireVolley(p, p, p->position);
                Sound_PlaySong(0x583);
                p->timer++;
            }
            sprite = animation + 0xA8;
            if (p->heading <= 0x10)
                frame = 0x17;
            else if (p->heading <= 0x2F)
                frame = 0x18;
            else if (p->heading <= 0x40)
                frame = 0x19;
            else if (p->heading > 0x9F)
                frame = 0x16;
            else
                frame = 0x1A;
            Sprite_ApplyAnimation(sprite, sprite + 0x2C);
            *(u16 *)(sprite + 0x10) = frame;
            MotionTrack_Update(q->track);
            p->timer++;
            if (complete(q)) {
                q->laps++;
                advance(p, 4);
            }
            break;
        }
        case 4:
            setFlags(p, 0x180000);
            if (p->timer == 0) {
                startDescent(p, q);
                p->timer++;
            }
            MotionTrack_Update(q->track);
            if (complete(q)) {
                if (q->laps > 3) {
                    advance(p, 5);
                } else {
                    advance(p, 2);
                }
            }
            break;
        case 5:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, animation, animation + 0x60, 8, 2, 2);
                p->timer++;
                {
                    struct VanargandPart *part = &q->part;
                    Anim_SetSequence(part->animation, part->sequence, 4, 0, 0);
                    q->partActive = 1;
                    part->field5C = 0;
                }
            }
            if (p->ready) {
                advance(p, 6);
            }
            break;
        case 6:
            p->finished = 1;
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081AA788);
#endif
