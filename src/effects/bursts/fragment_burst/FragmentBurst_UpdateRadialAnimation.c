#include "effects/bursts/fragment_burst.h"
#include "engine/video/display_sprite_playback.h"

extern const s16 gUnk_086149C4[];

static inline s32 FragmentBurst_TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}

static inline void FragmentBurst_MakeRadialOffset(struct ContactPosition *out, s32 angle, s32 scale)
{
    out->x = FragmentBurst_TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    out->y = 0;
    {
        u32 index = 255;

        index &= angle;
        out->z = FragmentBurst_TruncateScale(gUnk_086149C4[index] * scale);
    }
}

static inline void FragmentBurst_AddPosition(struct ContactPosition *dst, struct ContactPosition *a,
                                             struct ContactPosition *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}

static inline struct DisplaySequenceState *
FragmentSequence_Prepare(struct FragmentBurst *const *owner, struct FragmentBurstPool *pool,
                         u32 flipX, u32 flipY, void **resource, u32 *mirrors)
{
    struct DisplaySequenceState *base = (*owner)->sequences;
    *resource = pool->sequenceResource;
    *mirrors = flipX | flipY;
    return base;
}

/* Tentative: animate four radial burst sprites and advance after the last sequence ends. */
void FragmentBurst_UpdateRadialAnimation(struct FragmentBurstPool *pool,
                                         struct FragmentBurst *burst)
{
    s32 i;
    u32 ended;

    if (burst->enter) {
        struct DisplaySequenceSprite *sprite = burst->sprites;
        struct FragmentBurst *sequenceOwner = burst;

        for (i = 0; i < 4; i++) {
            struct ContactPosition offset;
            u32 flipX, flipY;

            sprite->flags &= ~0x10;
            sprite->position = burst->position;
            FragmentBurst_MakeRadialOffset(&offset, i << 6, 128);
            FragmentBurst_AddPosition(&sprite->position, &sprite->position, &offset);
            switch (i) {
                case 0:
                    flipX = 2;
                    flipY = 0;
                    break;
                case 1:
                    flipX = 2;
                    flipY = 1;
                    break;
                case 2:
                    flipX = 0;
                    flipY = 1;
                    break;
                default:
                    flipX = 0;
                    flipY = 0;
                    break;
            }
            {
                void *resource;
                u32 mirrors;
                struct DisplaySequenceState *base = FragmentSequence_Prepare(
                    &sequenceOwner, pool, flipX, flipY, &resource, &mirrors);
                Anim_SetSequence(&base[i], resource, 16, 0, mirrors);
            }
            sprite->flags &= ~1;
            sprite++;
        }
        burst->timer = 0;
        burst->enter = 0;
    }
    for (i = 0; i < 4; i++)
        ended = PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)&burst->sprites[i],
                                            &burst->sequences[i]);
    Collider_SetPosition(&burst->collider, &burst->position, 0);
    Collider_Refresh(&burst->collider);
    if (ended) {
        burst->state = 3;
        burst->enter = 1;
    } else {
        burst->timer++;
    }
}
