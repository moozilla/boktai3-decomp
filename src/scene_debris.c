#include "scene_debris_types.h"
#include "global.h"
#include "actor_attachment_state.h"
#include "background_controller.h"
#include "player_cue_sequence.h"
#include "ram.h"
#include "scene_debris.h"
#include "render.h"

s32 PanelSprite_Create(struct SceneDebris *owner, u32 resourceId, u32 key, u32 sequence, u32 row,
                       u32 variant, u32 spriteFlags, u32 priority)
{
    struct DisplayAnimationSequence *visual;
    owner->visual = Heap_Alloc(0x5C);
    if (!owner->visual)
        return -1;
    Memory_Clear(owner->visual, 0x5C);
    owner->visualType = 0;
    visual = (void *)owner->visual;
    if (!AnimData_Load(visual, resourceId))
        return -1;
    Sprite_Init(&visual->sprite, visual, 0);
    visual->sequence = ScriptProc_Start(0x922E, (u16)key);
    Anim_SetSequence(&visual->playback, visual->sequence, (u16)sequence, (u8)row, (u8)variant);
    Memory_Clear(&visual->sprite.position, 8);
    visual->sprite.flags = spriteFlags | 0x11;
    visual->sprite.priority = priority;
    return 1;
}

static inline void ClearPosition(void *src, void *dst)
{
    CpuSet(src, dst, 0x05000002);
}

s32 PanelKeyframeSprite_Create(struct SceneDebris *owner, u32 key, u32 index, u32 flags,
                               u32 priority)
{
    struct DisplayAnimationKeyframes *visual;
    struct {
        u32 zero;
        s32 position[2];
    } local;
    void *position;
    struct SceneInputWidget *widget;
    u32 spriteFlags;
    u32 frame;
    owner->visual = Heap_Alloc(0x80);
    if (!owner->visual)
        return -1;
    Memory_Clear(owner->visual, 0x80);
    owner->visualType = 1;
    visual = (void *)owner->visual;
    position = local.position;
    local.zero = 0;
    ClearPosition(&local.zero, position);
    ResourceHeader_Relocate(visual, ScriptProc_Start(0xCB05, (u16)key));
    spriteFlags = flags | 0x11;
    widget = &visual->widget;
    frame = (u16)index;
    Widget_InitLinked(widget, visual, frame, spriteFlags, (u8)priority, 2, 0x3C, position);
    Anim_SetKeyframes(widget, visual, frame, 2, 0);
    Anim_Step(widget, visual);
    return 1;
}

void Sprite_Remove(void *);

void Heap_Free(void *);

void SceneDebris_ReleaseSprite(struct SceneDebris *debris)
{
    union SceneDebrisVisual *visual = debris->visual;
    if (visual->sequence.sprite.unknown4[0] != 0) {
        Sprite_Remove(&visual->sequence.sprite);
        Heap_Free(debris->visual);
        debris->active = 0;
    }
}

void Anim_Remove(void *);

void SceneDebris_ReleaseAnimation(struct SceneDebris *debris)
{
    union SceneDebrisVisual *visual = debris->visual;
    if (visual->keyframes.widget.unknown4[0] != 0) {
        Anim_Remove(&visual->keyframes.widget);
        Heap_Free(debris->visual);
        debris->active = 0;
    }
}

void SceneDebris_ReleaseVisual(struct SceneDebris *debris)
{
    if (debris->visualType == 0)
        SceneDebris_ReleaseSprite(debris);
    else
        SceneDebris_ReleaseAnimation(debris);
}

void SceneDebris_OnMotionEnd(struct SceneDebris *debris)
{
    if (debris->entered) {
        SceneDebris_ReleaseVisual(debris);
        debris->active = 0;
        debris->entered = 0;
    }
}

struct Packed {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

s32 Div(s32, s32);

s32 Terrain_GetCellClearance(void *);

void SceneDebris_UpdateBouncingMotion(struct SceneDebris *debris)
{
    struct Packed world, projected;
    s32 clearance;
    if (debris->entered) {
        u32 *flags;
        if (!debris->visualType)
            flags = &debris->visual->sequence.sprite.flags;
        else
            flags = &debris->visual->keyframes.widget.flags;
        {
            u32 mask = ~1;
            *flags &= mask;
        }
        debris->entered = 0;
    }
    debris->position.x += debris->velocity.x;
    debris->position.y += debris->velocity.y;
    debris->position.z += debris->velocity.z;
    debris->velocity.y++;
    if (debris->position.z < 0) {
        debris->position.z = 0;
        debris->velocity.z = Div(-debris->velocity.z * debris->restitution, 10);
    } else if (debris->position.z > 63) {
        debris->position.z = 63;
        debris->velocity.z = Div(-debris->velocity.z * debris->restitution, 10);
    }
    world.x = debris->position.x;
    world.y = debris->position.y;
    world.z = debris->position.z;
    clearance = Terrain_GetCellClearance(&world);
    if (clearance < debris->position.y) {
        debris->position.y = clearance;
        debris->velocity.y = Div(-debris->velocity.y * debris->restitution, 10);
        debris->velocity.x = Div(debris->velocity.y * debris->restitution, 10);
    }
    projected.x = debris->position.x - gSceneCamera->x;
    projected.y = debris->position.y - gSceneCamera->y;
    projected.z = 0;
    if (!debris->visualType) {
        struct DisplayAnimationSequence *visual = &debris->visual->sequence;
        visual->sprite.position.x = ((struct ContactPosition *)&projected)->x;
        visual->sprite.position.y = ((struct ContactPosition *)&projected)->y;
        visual->sprite.position.z = ((struct ContactPosition *)&projected)->z;
    } else {
        struct DisplayAnimationKeyframes *visual = &debris->visual->keyframes;
        visual->widget.position.x = ((struct ContactPosition *)&projected)->x;
        visual->widget.position.y = ((struct ContactPosition *)&projected)->y;
        visual->widget.position.z = ((struct ContactPosition *)&projected)->z;
    }
    if (!debris->visualType)
        PlayerCue_StepSequenceEnded((void *)&debris->visual->sequence.sprite,
                                    &debris->visual->sequence.playback);
    else
        Anim_Step(&debris->visual->keyframes.widget, debris->visual);
    if (debris->timer)
        debris->timer--;
    else {
        debris->update = SceneDebris_OnMotionEnd;
        debris->entered = 1;
    }
}

// confidence=low: planar debris lifetime follows completion of its visual animation.

/* ROM copies an uninitialized projected depth halfword; keep it uninitialized to match. */

void SceneDebris_UpdateAnimatedMotion(struct SceneDebris *debris)
{
    struct Packed projected;
    s32 ended;
    struct ContactPosition *output;
    if (debris->entered) {
        u32 *flags;
        if (!debris->visualType)
            flags = &debris->visual->sequence.sprite.flags;
        else
            flags = &debris->visual->keyframes.widget.flags;
        {
            u32 mask = ~1;
            *flags &= mask;
        }
        debris->entered = 0;
    }
    debris->position.x = debris->position.x + debris->velocity.x;
    debris->position.y = debris->position.y + debris->velocity.y;
    projected.x = debris->position.x - gSceneCamera->x;
    projected.y = debris->position.y - gSceneCamera->y;
    output = (void *)&projected;
    if (!debris->visualType) {
        struct DisplayAnimationSequence *visual = &debris->visual->sequence;
        visual->sprite.position.x = ((struct ContactPosition *)&projected)->x;
        visual->sprite.position.y = output->y;
        visual->sprite.position.z = output->z;
    } else {
        struct DisplayAnimationKeyframes *visual = &debris->visual->keyframes;
        visual->widget.position.x = ((struct ContactPosition *)&projected)->x;
        visual->widget.position.y = output->y;
        visual->widget.position.z = output->z;
    }
    if (!debris->visualType) {
        ended = PlayerCue_StepSequenceEnded((void *)&debris->visual->sequence.sprite,
                                            &debris->visual->sequence.playback);
    } else
        ended = Anim_Step(&debris->visual->keyframes.widget, debris->visual);
    if (ended) {
        debris->update = SceneDebris_OnMotionEnd;
        debris->entered = 1;
    }
}

s32 SceneDebrisPool_Update(struct DebrisPool *pool)
{
    s32 index;
    struct SceneDebris *debris;
    index = 0;
    debris = pool->entries;
    do {
        s32 offset = index * (s32)sizeof(*debris);
        void (**callback)(struct SceneDebris *);
        if (debris->active != 0) {
            callback = &pool->entries[0].update;
            callback = (void *)((u8 *)callback + offset);
            Callback_InvokeActor(debris, *callback);
        }
        debris = debris + 1;
        index++;
    } while (index <= 0x3f);
    return 0;
}

s32 SceneDebrisPool_Destroy(struct DebrisPool *pool)
{
    s32 index;
    u32 z;
    struct SceneDebris *debris = pool->entries;
    index = 0x3f;
    do {
        if (debris->active != 0)
            SceneDebris_ReleaseVisual(debris);
        debris++;
    } while (--index >= 0);
    z = 0;
    gDebrisPool = (void *)z;
    return 0;
}

s32 SceneDebrisPool_Initialize(struct DebrisPool *pool)
{
    gDebrisPool = pool;
    return 0;
}

struct DebrisPool *SceneDebrisPool_EnsureSingleton(void)
{
    struct DebrisPool *pool;
    if (gDebrisPool != 0)
        return gDebrisPool;
    pool = Task_Create(0xa, sizeof(*pool));
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))SceneDebrisPool_Update,
                          (void (*)(void *))SceneDebrisPool_Destroy);
        if (SceneDebrisPool_Initialize(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

void AnimData_SelectPaletteBank(void *, s32);

// confidence=low: claims a free debris slot, copies position/velocity, builds its panel sprite and picks the motion callback.

/* SceneDebris_Create retains its address symbol for callers outside this unit. */
s32 sub_08113398(const struct ContactPosition *position, const struct ContactPosition *velocity,
                 u32 resourceId, u32 sequenceResource, u32 sequence, u32 keyframe, u32 row,
                 u32 variant, s32 palette, u32 timer, u32 motion, u32 visualFlags, u32 priority,
                 u32 restitution)
{
    struct DebrisPool *pool;
    s32 i;
    if (!gDebrisPool) {
        pool = SceneDebrisPool_EnsureSingleton();
        if (!pool)
            return -1;
    } else
        pool = gDebrisPool;
    for (i = 0; i <= 63; i++) {
        if (!pool->entries[i].active) {
            struct SceneDebris *debris = &pool->entries[i];
            debris->position.x = position->x;
            debris->position.y = position->y;
            debris->position.z = position->z;
            if (!velocity) {
                debris->velocity.x = 0;
                debris->velocity.y = 0;
                debris->velocity.z = 0;
            } else {
                debris->velocity.x = velocity->x;
                debris->velocity.y = velocity->y;
                debris->velocity.z = velocity->z;
            }
            debris->restitution = restitution;
            debris->active = 1;
            debris->entered = 1;
            debris->timer = timer;
            if (!keyframe) {
                union SceneDebrisVisual *visual;
                PanelSprite_Create(debris, resourceId, sequenceResource, sequence, row, variant,
                                   visualFlags, priority);
                visual = debris->visual;
                if (palette >= 0)
                    AnimData_SelectPaletteBank(visual, palette);
            } else {
                union SceneDebrisVisual *visual;
                PanelKeyframeSprite_Create(debris, resourceId, sequence, visualFlags, priority);
                visual = debris->visual;
                if (palette >= 0)
                    visual->keyframes.widget.palette = (void *)(gPaletteBankData + palette * 32);
            }
            switch (motion) {
                case 0:
                    debris->update = SceneDebris_UpdateAnimatedMotion;
                    break;
                case 1:
                    debris->update = SceneDebris_UpdateBouncingMotion;
                    break;
                default:
                    debris->update = SceneDebris_UpdateAnimatedMotion;
                    break;
            }
            return i;
        }
    }
    return -1;
}
