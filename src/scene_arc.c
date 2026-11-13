#include "scene_arc.h"
#include "display_sprite_playback.h"

void SceneArc_HandleCollision(void)
{
}

void SceneArc_HandleCollision(void);

s32 GroundEffect_Initialize(struct SceneGroundEffect *, const struct ScenePoolVec32 *, u32);

void SpatialIndex_InitEntry(void *, void *, void *, void *, u32, u32, u32, u32, u32, void (*)(void),
                            void *);

void SceneArc_InitializeSlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, s32 index)
{
    struct SceneArcPackedVector origin, extent, offset;
    slot->frame = 0;
    AnimData_Load(&slot->animationData, 0xe74b);
    Sprite_Init(&slot->sprite, &slot->animationData, 0x11);
    slot->start.x = 0;
    slot->start.y = 0;
    slot->start.z = 0;
    origin.x = 0;
    origin.y = 0;
    origin.z = 0;
    extent.x = 8;
    extent.y = 4;
    extent.z = 8;
    offset.x = 0;
    offset.y = 4;
    offset.z = 0;
    GroundEffect_Initialize(&slot->ground, &slot->position, 1);
    SpatialIndex_InitEntry(slot->collider, &origin, &extent, &offset, 12, 5, 5, 30, 12,
                           SceneArc_HandleCollision, slot);
}

u32 GroundEffect_Destroy(struct SceneGroundEffect *);

void SpatialIndex_RemoveEntry(void *);

void SceneArc_DestroySlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, s32 index)
{
    Sprite_Remove(&slot->sprite);
    GroundEffect_Destroy(&slot->ground);
    SpatialIndex_RemoveEntry(slot->collider);
}

void GroundEffect_DisableProjection(struct SceneGroundEffect *);

void SceneArc_ReleaseSlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, u32 index)
{
    u32 mask;
    slot->sprite.flags |= 1;
    GroundEffect_DisableProjection(&slot->ground);
    mask = 1 << index;
    manager->activeMask &= ~mask;
}

s32 __divsi3(s32, s32);

s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);

s32 Terrain_GetHeightAtPosition(struct ScenePoolVec32 *);

void Collider_SetFixedPositionWithYOffset(void *, struct ScenePoolVec32 *, u32);

void SpatialIndex_InsertEntry(void *);

void SceneArc_ReleaseSlot(struct SceneArcManager *, struct SceneArcSlot *, u32);

void SpriteEffect_CreateAtPosition(struct ScenePoolVec32 *, void *, void *, u32, u32, u32, u32, u32,
                                   u32, u32, u32, u32, u32, u32, u32);

static inline s32 SignedShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -((-value) >> shift);
}

static inline void ProjectArc(struct SceneArcSlot *slot, struct ContactSwarmSprite *sprite,
                              s32 lift)
{
    s32 height = lift << 12;
    struct ScenePoolVec32 *position = &slot->position;
    struct ContactPosition *screen = &sprite->position;
    screen->x = (slot->position.x >> 12) - gSceneCamera->x;
    screen->y = ((position->z + (position->y - height)) >> 12) + 96;
    screen->z = position->z >> 12;
    sprite->position.y = (u16)sprite->position.y - lift;
}

static inline u8 PlayerSuspendsArc(void)
{
    if (gParticipantActors[0]) {
        u32 flags = gParticipantActors[0]->flags;
        if (flags & 4)
            return 1;
    }
    return 0;
}

static inline void MakeVelocity(struct SceneVec *out, s32 angle, s32 speed)
{
    out->x = SignedShift(gUnk_086149C4[(angle + 64) & 255] << 13, 12);
    out->y = SignedShift(gUnk_086149C4[angle & 255] * speed, 12);
    out->z = 0;
}

/* Apply the current frame and flips, then advance the stored playback state.
 * The duration calculation uses the newly selected command's low four bits. */

static inline void HideSprite(struct ContactSwarmSprite *sprite)
{
    sprite->flags |= 1;
}

void SceneArc_UpdateSlot(struct SceneArcManager *manager, struct SceneArcSlot *slot, u32 index)
{
    void *collider = slot->collider;
    u32 frame = slot->frame;
    u32 duration = slot->flightDuration;
    struct ContactSwarmSprite *sprite;
    struct ScenePoolVec32 *position;
    u32 paused;
    if (frame <= duration) {
        u32 remainder;
        s32 lift;
        s32 terrain;
        s32 screenY;
        sprite = &slot->sprite;
        remainder = duration - frame;
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
        terrain = Terrain_GetHeightAtFixedPosition(&slot->position);
        screenY = sprite->position.y;
        position = &slot->position;
        if (terrain < screenY) {
            sprite->position.y = terrain;
            slot->grounded = 1;
            slot->frame = slot->endFrame;
            Sound_PlaySongChecked(0x166);
        } else if (slot->frame == slot->flightDuration) {
            Sound_PlaySongChecked(0x166);
        }
    } else {
        s32 lift;
        u32 remaining;
        u32 interval;
        struct SceneArcPackedVector direction;
        struct SceneVec velocity;
        s32 angle;
        s32 terrain;
        s32 screenY;
        sprite = &slot->sprite;
        lift = 0;
        remaining = duration - frame;
        interval = duration - slot->endFrame;
        direction.x = (slot->target.x >> 12) - (slot->start.x >> 12);
        direction.y = slot->target.y - (slot->start.y >> 12);
        direction.z = (slot->target.z >> 12) - (slot->start.z >> 12);
        angle = Math_Atan2(direction.x, direction.z);
        MakeVelocity(&velocity, angle, 8192);
        if (slot->grounded != 1) {
            slot->position.x += velocity.x;
            slot->position.y += velocity.y;
            slot->position.z += velocity.z;
        }
        if (slot->position.z <= 0x5fff)
            slot->position.z = 0x6000;
        else if (slot->position.z > 0x34000)
            slot->position.z = 0x34000;
        if (interval != 0) {
            s32 phase = Div(remaining << 7, interval);
            const s16 *table = gUnk_086149C4;
            lift = SignedShift(table[phase & 255], 10);
        }
        ProjectArc(slot, sprite, lift);
        terrain = Terrain_GetHeightAtFixedPosition(&slot->position);
        screenY = sprite->position.y;
        position = &slot->position;
        if (terrain < screenY) {
            sprite->position.y = terrain;
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
                HideSprite(&slot->sprite);
                GroundEffect_DisableProjection(&slot->ground);
            }
        }
        if (slot->frame == slot->releaseFrame) {
            SceneArc_ReleaseSlot(manager, slot, index);
            return;
        }
    }
    PlayerCue_StepSequenceEnded(&slot->sprite, &slot->animation);
    slot->frame++;
}

void SceneArc_UpdateSlot(struct SceneArcManager *, struct SceneArcSlot *, u32);

u32 SceneArc_UpdateManager(struct SceneArcManager *manager)
{
    struct SceneArcSlot *slot = manager->slots;
    s32 index = 0;
    do {
        if (manager->activeMask & (1 << index))
            SceneArc_UpdateSlot(manager, slot, index);
        index++;
        slot++;
    } while (index <= 7);
    return 0;
}

void SceneArc_DestroySlot(struct SceneArcManager *, struct SceneArcSlot *, s32);

void ResourceRegistry_UnregisterResource(u32);

u32 SceneArc_DestroyManager(struct SceneArcManager *manager)
{
    struct SceneArcSlot *slot = manager->slots;
    s32 index = 0;
    do {
        SceneArc_DestroySlot(manager, slot, index);
        index++;
        slot++;
    } while (index <= 7);
    ResourceRegistry_UnregisterResource(0xfe2c);
    return 0;
}

void ResourceRegistry_RegisterResource(u32, void *, u32);

void SceneArc_InitializeSlot(struct SceneArcManager *, struct SceneArcSlot *, s32);

s32 SceneArc_InitializeManager(struct SceneArcManager *manager)
{
    struct SceneArcSlot *slot;
    s32 index;
    ResourceRegistry_RegisterResource(0xfe2c, manager, 0);
    manager->activeMask = 0;
    manager->animationResource = ScriptProc_Start(0x922e, 0x871c);
    slot = manager->slots;
    index = 0;
    do {
        SceneArc_InitializeSlot(manager, slot, index);
        index++;
        slot++;
    } while (index <= 7);
    return 0;
}

struct SceneArcManager *SceneArc_CreateManager(void)
{
    struct SceneArcManager *manager = Task_Create(10, sizeof(*manager));
    if (manager) {
        Task_SetCallbacks(manager, (void *)SceneArc_UpdateManager, (void *)SceneArc_DestroyManager);
        if (SceneArc_InitializeManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

struct SceneArcSlot *SceneArc_FindFreeSlot(struct SceneArcManager *manager, s32 *out)
{
    struct SceneArcSlot *slot = manager->slots;
    s32 index = 0;
    u32 one = 1, mask = manager->activeMask;
    do {
        if ((one << index) & mask) {
            index++;
            slot++;
        } else {
            *out = index;
            return slot;
        }
    } while (index <= 7);
    return 0;
}

struct SceneArcManager *ResourceRegistry_GetResource(u32);

struct SceneArcSlot *SceneArc_FindFreeSlot(struct SceneArcManager *, s32 *);

struct SceneArcManager *SceneArc_CreateManager(void);

void GroundEffect_EnableProjection(struct SceneGroundEffect *);

s32 SceneArc_Create(const struct ScenePoolVec32 *from, const struct ScenePoolVec32 *to,
                    u32 flightDuration, u32 endFrame, s32 height)
{
    const struct ScenePoolVec32 *startArg = from;
    const struct ScenePoolVec32 *targetArg = to;
    u32 duration = flightDuration;
    u32 end = endFrame;
    struct SceneArcManager *manager = ResourceRegistry_GetResource(0xfe2c);
    struct SceneArcSlot *slot;
    s32 index;
    u32 zero;
    struct ContactSwarmSprite *sprite;
    if (!manager) {
        SceneArc_CreateManager();
        return -1;
    }
    slot = SceneArc_FindFreeSlot(manager, &index);
    if (!slot)
        return -1;
    sprite = &slot->sprite;
    zero = 0;
    slot->frame = zero;
    slot->flightDuration = duration;
    slot->endFrame = end;
    slot->releaseFrame = end + 8;
    slot->height = height;
    slot->start = *startArg;
    slot->position = *startArg;
    slot->target = *targetArg;
    slot->grounded = zero;
    {
        struct ScenePoolVec32 *position = &slot->position;
        struct ContactPosition *screen = &slot->sprite.position;
        screen->x = (slot->position.x >> 12) - gSceneCamera->x;
        {
            s32 y = position->y;
            screen->y = ((position->z + y) >> 12) + 96;
        }
        screen->z = position->z >> 12;
    }
    sprite->frame = zero;
    sprite->flags &= ~1;
    Anim_SetSequence(&slot->animation, manager->animationResource, 2, 0, zero);
    GroundEffect_EnableProjection(&slot->ground);
    manager->activeMask |= 1 << index;
    return 0;
}
