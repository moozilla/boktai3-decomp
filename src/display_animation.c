#include "display_animation.h"
#include "display_sprite_playback.h"

s32 DisplayAnimation_SetEntry(struct DisplayAnimation *display, u32 index, u32 variant, u32 flags,
                              u32 mode, u32 sequenceFlags, u32 keyframeFlags)
{
    union DisplayAnimationBody *body;
    if (display->kind == 0)
        return -1;
    if (display->kind == 1) {
        body = &display->resource;
        display->variant = variant;
        display->flags = flags;
        if (flags & 1)
            sequenceFlags |= 1;
        display->index = index;
        if (body->sequence.sequence &&
            Anim_SetSequence(&display->resource.sequence.playback, body->sequence.sequence,
                             (u16)index, display->variant, (u8)sequenceFlags))
            display->finished = 0;
    } else {
        body = &display->resource;
        display->variant = variant;
        display->flags = flags;
        display->index = index + display->variant;
        Anim_SetKeyframes(&display->resource.keyframes.widget, &body->keyframes.resource,
                          (u16)display->index, (u16)mode, (u16)keyframeFlags);
        if (!(keyframeFlags & 1))
            display->finished = 0;
        if (display->flags)
            body->keyframes.widget.flags |= 4;
        else
            body->keyframes.widget.flags &= ~4;
    }
    return 0;
}

s32 DisplayAnimation_UpdatePlayback(struct DisplayAnimation *display)
{
    u32 kind = display->kind;
    if (kind == 0)
        return -1;
    if (kind == 1) {
        if (display->resource.sequence.sequence)
            display->finished = PlayerCue_StepSequenceEndedWithFirstFlipMask(
                (void *)&display->resource.sequence.sprite, &display->resource.sequence.playback,
                kind);
    } else {
        struct SceneInputResource *resource = &display->resource.keyframes.resource;
        display->finished = Anim_Step(&display->resource.keyframes.widget, resource);
    }
    return display->finished;
}

s32 DisplayAnimation_TestFinished(struct DisplayAnimation *display)
{
    if (display->kind == 1)
        return display->finished;
    {
        struct SceneInputWidget *state = &display->resource.keyframes.widget;
        if (state->status & 2)
            return 1;
        return 0;
    }
}

s32 DisplayAnimation_GetFrameBoundary(struct DisplayAnimation *display, u32 *out)
{
    if (display->kind == 2) {
        struct SceneInputWidget *state = &display->resource.keyframes.widget;
        if (state->status & 4) {
            *out = state->frame;
            return 1;
        }
    } else {
        struct DisplaySequenceState *state = &display->resource.sequence.playback;
        if (!state->elapsed) {
            *out = state->index;
            return 1;
        }
    }
    return 0;
}

s32 DisplayAnimation_Destroy(struct DisplayAnimation *display)
{
    u32 mode = display->kind;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        Sprite_Remove(&display->resource.sequence.sprite);
    } else {
        Anim_Remove(&display->resource.keyframes.widget);
    }
    display->kind = 0;
    return 0;
}

s32 DisplayAnimation_InitializeSequence(struct DisplayAnimation *display, s32 resourceId,
                                        u32 sequenceId, s32 flags,
                                        const struct DisplayPosition *position)
{
    struct DisplayAnimationSequence *sequence;
    struct DisplaySequenceSprite *sprite;
    if (display->kind)
        goto failed;
    Memory_Clear(display, sizeof(struct DisplayAnimation));
    display->kind = 1;
    sequence = &display->resource.sequence;
    if (!AnimData_Load(&sequence->graphics, resourceId))
        goto failed;
    if (!sequenceId)
        goto noSequence;
    sequence->sequence = ScriptProc_Start(0x922E, (u16)sequenceId);
    if (sequence->sequence)
        goto setup;
failed:
    return -1;
noSequence:
    sequence->sequence = (void *)sequenceId;
setup:
    sprite = &sequence->sprite;
    Sprite_Init(sprite, &sequence->graphics, flags);
    *(struct DisplayPosition *)&sequence->sprite.position = *position;
    display->position = (struct DisplayPosition *)&sequence->sprite.position;
    display->displayFlags = &sprite->flags;
    return 0;
}

s32 Widget_InitLinked(void *, void *, u32, s32, u32, u32, u32, const struct DisplayPosition *);

s32 DisplayAnimation_InitializeKeyframes(struct DisplayAnimation *display, u32 resourceId,
                                         s32 flags, const struct DisplayPosition *position)
{
    struct SceneInputResource *resource;
    const struct SceneInputResource *source;
    u32 inactive = display->kind;
    if (inactive)
        goto failed;
    Memory_Clear(display, sizeof(struct DisplayAnimation));
    display->kind = 2;
    resource = &display->resource.keyframes.resource;
    source = ScriptProc_Start(0xCB05, (u16)resourceId);
    if (source)
        goto setup;
failed:
    return -1;
setup:
    *resource = *source;
    ResourceHeader_Relocate(resource, (void *)source);
    {
        s32 result = Widget_InitLinked(&display->resource.keyframes.widget, resource, 0, flags, 1,
                                       inactive, 60, position);
        display->position = (struct DisplayPosition *)&display->resource.keyframes.widget.position;
        display->displayFlags = &display->resource.keyframes.widget.flags;
        return result;
    }
}

s32 DisplayAnimation_SetPosition(struct DisplayAnimation *display,
                                 const struct DisplayPosition *position)
{
    if (!display->kind)
        return -1;
    *display->position = *position;
    /* No return on success: the ROM leaves the incoming display pointer in r0. */
}

s32 DisplayAnimation_SetFlags(struct DisplayAnimation *display, u32 mask)
{
    u32 *flags;
    if (display->kind == 0) {
        return -1;
    }
    flags = display->displayFlags;
    *flags |= mask;
    return 0;
}

s32 DisplayAnimation_ClearFlags(struct DisplayAnimation *display, u32 mask)
{
    u32 *flags;
    if (display->kind == 0) {
        return -1;
    }
    flags = display->displayFlags;
    *flags &= ~mask;
    return 0;
}

u32 DisplayAnimation_TestFlags(struct DisplayAnimation *display, u32 mask)
{
    if (display->kind == 0)
        return 0;
    return *display->displayFlags & mask;
}

s32 DisplayAnimation_SetPriority(struct DisplayAnimation *display, u32 priority)
{
    if (!display->kind)
        return -1;
    if (display->kind == 1) {
        struct DisplaySequenceSprite *sprite = &display->resource.sequence.sprite;
        sprite->priority = priority;
    } else {
        struct SceneInputWidget *state = &display->resource.keyframes.widget;
        state->attribute1A = priority;
    }
    return 0;
}

s32 DisplayAnimation_SetPalette(struct DisplayAnimation *display, u32 scaleX, u32 scaleY, u32 angle)
{
    if (!display->kind)
        return -1;
    if (display->kind == 1) {
        struct DisplaySequenceSprite *sprite = &display->resource.sequence.sprite;
        sprite->palette = scaleX;
        sprite->paletteVariant = scaleY;
        sprite->attribute = angle;
    } else {
        struct SceneInputWidget *state = &display->resource.keyframes.widget;
        state->scaleX = scaleX;
        state->scaleY = scaleY;
        state->rotation = angle;
    }
    return 0;
}

void Anim_SelectPaletteBank(void *, u32);

s32 DisplayAnimation_SelectPaletteBank(struct DisplayAnimation *display, u32 index)
{
    if (!display->kind)
        return -1;
    if (display->kind == 1)
        AnimData_SelectPaletteBank(&display->resource.sequence.graphics, index);
    else
        Anim_SelectPaletteBank(&display->resource.keyframes.widget, index);
    return 0;
}

s32 DisplayAnimation_SetTileSource(struct DisplayAnimation *display, u32 paletteAddress)
{
    u32 mode = display->kind;
    if (mode == 0) {
        return -1;
    }
    if (mode == 1) {
        display->resource.sequence.graphics.palette = (void *)paletteAddress;
    } else {
        display->resource.keyframes.widget.palette = (u16 *)paletteAddress;
    }
    return 0;
}

u32 DisplayAnimation_GetTileIndex(struct DisplayAnimation *display)
{
    u32 mode = display->kind;
    if (mode == 0) {
        return 0;
    }
    if (mode == 1) {
        return display->resource.sequence.graphics.paletteIndex;
    }
    {
        struct SceneInputWidget *widget = &display->resource.keyframes.widget;
        return widget->paletteBank;
    }
}
