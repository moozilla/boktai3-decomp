// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct FragmentSprite {
    u32 flags;
    u8 active, unknown5[2], priority;
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
};
struct Fragment {
    u32 flags;
    u8 active, unk_05[2], priority;
    s8 scaleX, scaleY;
    u8 unk_0A[2];
    void *graphics;
    u16 frame;
    u8 unk_12[10];
    struct ContactPosition position;
    u8 unk_24[8];
    struct DisplaySequenceState sequence;
    u8 collider[0x54];
    struct ContactPosition center, offset;
    u32 angle, angularSpeed;
    s16 direction;
    u16 unk_AA;
    s16 radius, scaleTimer;
    void *resource;
    s32 lifetime;
    u16 unk_B8;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered;
};
struct Pool {
    u8 unk_00[0x18];
    u8 graphics[0x20];
    u32 used;
};
s32 Mod(s32, s32);
s32 Div(s32, s32);
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);
void Sprite_Remove(void *);
static inline void FragmentEffect_StepAnimation(struct Fragment *sprite, u32 initializing)
{
    PlayerCue_StepSequence(sprite, &sprite->sequence);
}
// Tentative: shrink the fragment, then release its sprite and pool slot.
void FragmentEffect_ShrinkAndRemove(struct Pool *pool, struct Fragment *p)
{
    s16 *scaleTimer;
    u16 *timer;
    if (p->entered) {
        p->flags = (p->flags & ~1) | 2;
        p->scaleTimer = 20;
        p->timer = 0;
        p->entered = 0;
        p->scaleX = 64;
        p->scaleY = 64;
        p->timer = 0;
    }
    {
        u16 *frame = &p->timer;
        s32 remainder = Mod(*frame, 6);
        timer = frame;
        if (!remainder)
            FragmentEffect_SpawnRandomParticle(&p->position);
    }
    {
        register s16 *initial asm("r0") = &p->scaleTimer;
        s32 remaining = *initial;
        scaleTimer = initial;
        if (remaining > 0) {
            p->scaleX += Div(1 - p->scaleX, *scaleTimer);
            p->scaleY += Div(1 - p->scaleY, *scaleTimer);
            (*scaleTimer)--;
        }
    }
    FragmentEffect_StepAnimation(p, 0);
    if (*scaleTimer <= 0) {
        Sprite_Remove(p);
        pool->used &= ~(1 << p->slot);
        p->slot = -1;
        p->state = 0;
        p->entered = 0;
    } else {
        (*timer)++;
    }
}
struct Pool *FragmentEffect_GetOrCreateManager(void);
struct Fragment *sub_0812C3D0(struct Pool *);
void FragmentEffect_InitCollider(struct Fragment *, u32, u32, u32, u32, u32, u32, u32);
extern const s16 gUnk_086149C4[];
static inline s32 FragmentEffect_Truncate(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline void FragmentEffect_ComputeOffset(struct ContactPosition *out, u32 heading,
                                                s32 radius)
{
    u32 angle = heading >> 12;
    s32 scale = radius * 128;
    out->x = FragmentEffect_Truncate(scale * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = FragmentEffect_Truncate(scale * gUnk_086149C4[angle & 255]);
}
#define FragmentEffect_Create sub_0812CAAC
void FragmentEffect_Create(const struct ContactPosition *position, u32 angle, u32 angularSpeed,
                           s32 radius, s32 direction, s32 lifetime, u32 a, u32 b, u32 c, u32 d)
{
    struct Pool *pool = (struct Pool *)gFragmentEffectManager;
    struct Fragment *p;
    struct FragmentSprite *sprite;
    struct ContactPosition offset;
    if (!pool) {
        pool = FragmentEffect_GetOrCreateManager();
        if (!pool)
            return;
    }
    p = sub_0812C3D0(pool);
    if (!p)
        return;
    p->state = 1;
    p->lifetime = lifetime;
    p->unk_B8 = 0;
    p->angle = angle << 12;
    p->angularSpeed = angularSpeed;
    p->radius = radius;
    p->direction = direction;
    p->entered = 1;
    p->center = *position;
    sprite = (struct FragmentSprite *)p;
    sprite->position = p->center;
    {
        u32 heading = p->angle;
        FragmentEffect_ComputeOffset(&offset, heading, p->radius);
    }
    sprite->position.x += offset.x;
    sprite->position.z += offset.z;
    p->resource = ScriptProc_Start(0x922e, 0x121b);
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(&p->sequence, p->resource, 7, 0, 0);
    AnimData_SelectPaletteBank(sprite->graphics, 0x127);
    FragmentEffect_StepAnimation(p, 1);
    sprite->priority = 2;
    sprite->flags &= ~1;
    FragmentEffect_InitCollider(p, a, b, c, d, 0, 0, 0);
}
