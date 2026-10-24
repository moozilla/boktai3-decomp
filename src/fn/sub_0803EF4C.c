#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Allocation and projection/animation scheduling still differ; reviewed extent is 1112 bytes.
#include "scene_arc.h"
s32 __divsi3(s32, s32);
s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);
s32 Terrain_GetHeightAtPosition(struct ScenePoolVec32 *);
void Collider_SetFixedPositionWithYOffset(void *, struct ScenePoolVec32 *, u32);
void SpatialIndex_InsertEntry(void *);
void GroundEffect_DisableProjection(struct SceneGroundEffect *);
void SceneArc_ReleaseSlot(struct SceneArcManager *, struct SceneArcSlot *, u32);
void SpriteEffect_CreateAtPosition(struct ScenePoolVec32 *, void *, void *, u32, u32, u32, u32, u32,
                                   u32, u32, u32, u32, u32, u32, u32);
struct ArcDirection {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
static inline s32 SignedShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -((-value) >> shift);
}
static inline void ProjectArc(struct SceneArcSlot *slot, struct SceneArcSprite *sprite, s32 lift)
{
    s32 height = lift << 12;
    struct ScenePoolVec32 *position = &slot->position;
    struct SceneVec *screen = &sprite->screen;
    screen->x = (slot->position.x >> 12) - gUnk_020004B8->x;
    screen->y = ((position->z + (position->y - height)) >> 12) + 96;
    screen->z = position->z >> 12;
    screen->y = (u16)screen->y - lift;
}
static inline u8 PlayerSuspendsArc(void)
{
    if (gUnk_020004F0[0] && (gUnk_020004F0[0]->flags & 4))
        return 1;
    return 0;
}
void SceneArc_UpdateSlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, u32 index)
{
    void *collider = slot->collider;
    u32 frame = slot->frame;
    u32 duration = slot->flightDuration;
    struct SceneArcSprite *sprite;
    struct ScenePoolVec32 *position;
    u32 paused;
    if (frame <= duration) {
        u32 remainder = duration - frame;
        s32 lift;
        s32 terrain;
        sprite = &slot->sprite;
        slot->position.x =
            __divsi3(((slot->target.x >> 12) * frame) + ((slot->start.x >> 12) * remainder),
                     duration)
            << 12;
        slot->position.y =
            __divsi3(((slot->target.y >> 12) * frame) + ((slot->start.y >> 12) * remainder),
                     slot->flightDuration)
            << 12;
        slot->position.z =
            __divsi3(((slot->target.z >> 12) * frame) + ((slot->start.z >> 12) * remainder),
                     slot->flightDuration)
            << 12;
        lift = SignedShift(
            gUnk_086149C4[Div(slot->frame << 7, slot->flightDuration) & 255] * slot->height, 12);
        ProjectArc(slot, sprite, lift);
        position = &slot->position;
        terrain = Terrain_GetHeightAtFixedPosition(position);
        if (terrain < sprite->screen.y) {
            sprite->screen.y = terrain;
            slot->grounded = 1;
            slot->frame = slot->endFrame;
            goto flightEnded;
        }
        if (slot->frame == slot->flightDuration) {
        flightEnded:
            Sound_PlaySongChecked(0x166);
        }
    } else {
        s32 lift = 0;
        u32 remaining = duration - frame;
        u32 interval = duration - slot->endFrame;
        struct ArcDirection direction;
        struct SceneVec velocity;
        s32 angle;
        s32 terrain;
        sprite = &slot->sprite;
        direction.x = (slot->target.x >> 12) - (slot->start.x >> 12);
        direction.y = slot->target.y - (slot->start.y >> 12);
        direction.z = (slot->target.z >> 12) - (slot->start.z >> 12);
        angle = Math_Atan2(direction.x, direction.z);
        velocity.x = SignedShift(gUnk_086149C4[(angle + 64) & 255] << 13, 12);
        velocity.y = SignedShift(gUnk_086149C4[angle & 255] * 8192, 12);
        velocity.z = 0;
        if (slot->grounded != 1) {
            slot->position.x += velocity.x;
            slot->position.y += velocity.y;
            slot->position.z += velocity.z;
        }
        if (slot->position.z <= 0x5fff)
            slot->position.z = 0x6000;
        else if (slot->position.z > 0x34000)
            slot->position.z = 0x34000;
        if (interval != 0)
            lift = SignedShift(gUnk_086149C4[Div(remaining << 7, interval) & 255], 10);
        ProjectArc(slot, sprite, lift);
        position = &slot->position;
        terrain = Terrain_GetHeightAtFixedPosition(position);
        if (terrain < sprite->screen.y) {
            sprite->screen.y = terrain;
            slot->grounded = 1;
        }
    }
    paused = PlayerSuspendsArc();
    if (!paused) {
        if (slot->frame >= slot->endFrame) {
            Collider_SetFixedPositionWithYOffset(collider, position, 0);
            SpatialIndex_InsertEntry(collider);
            if (slot->frame == slot->endFrame) {
                Sound_PlaySongChecked(0x195);
                slot->position.y = -(Terrain_GetHeightAtPosition(position) << 12);
                SpriteEffect_CreateAtPosition(position, 0, 0, 0x8639, 0x75f6, paused, paused,
                                              paused, paused, 0x153, 30, paused, paused, 1, paused);
                sprite->flags |= 1;
                GroundEffect_DisableProjection(&slot->ground);
            }
        }
        if (slot->frame == slot->releaseFrame) {
            SceneArc_ReleaseSlot(manager, slot, index);
            return;
        }
    }
    {
        struct SceneArcAnimation *animation = &slot->animation;
        const u16 *key = &animation->data[animation->frame];
        u32 two;
        u32 limit;
        u32 rate;
        u32 flags;
        u32 next;
        sprite->frame = (*key) >> 6;
        if ((animation->flags & 1) != (((*key) >> 4) & 1))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
        two = 2;
        if (((u8)(animation->flags & two)) != (((*key) >> 4) & two))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
        animation->timer++;
        limit = 0xffff;
        rate = animation->rate;
        if (animation->timer >= rate) {
            animation->timer = 0;
            flags = animation->flags;
            if (flags & 4) {
                next = animation->frame;
                if (!next) {
                    if (flags & 8) {
                        animation->timer = rate;
                        goto newFrame;
                    }
                    next = animation->count;
                }
                next--;
                goto storeFrame;
            } else {
                next = animation->frame + 1;
                animation->frame = next;
                if ((next & limit) < animation->count)
                    goto newFrame;
                if ((u8)(flags & 8)) {
                    animation->timer = rate;
                    next = animation->count - 1;
                } else
                    next = (u8)(flags & 8);
            storeFrame:
                animation->frame = next;
            }
        newFrame:
            key = &animation->data[animation->frame];

            animation->factor = (*key) & 15;
            animation->rate = (animation->scale * animation->factor) >> 6;
            if (!animation->rate)
                animation->rate = 1;
        }
    }
    slot->frame++;
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", SceneArc_UpdateSlot);
#endif
