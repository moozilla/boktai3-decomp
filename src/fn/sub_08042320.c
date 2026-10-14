#include "display_animation.h"
s32 DisplayAnimation_SetEntry(struct DisplayAnimation *display, u32 index, u32 variant, u32 flags,
                              u32 mode, u32 sequenceFlags, u32 keyframeFlags)
{
    struct DisplayAnimationResource *resource;
    if (display->kind == 0)
        return -1;
    if (display->kind == 1) {
        resource = &display->resource;
        display->variant = variant;
        display->flags = flags;
        if (flags & 1)
            sequenceFlags |= 1;
        display->index = index;
        if (resource->sequence && Anim_SetSequence((u8 *)display + 0x54, resource->sequence,
                                                   (u16)index, display->variant, (u8)sequenceFlags))
            display->finished = 0;
    } else {
        resource = &display->resource;
        display->variant = variant;
        display->flags = flags;
        display->index = index + display->variant;
        Anim_SetKeyframes((u8 *)display + 0x2C, resource, (u16)display->index, (u16)mode,
                          (u16)keyframeFlags);
        if (!(keyframeFlags & 1))
            display->finished = 0;
        if (display->flags)
            resource->flags |= 4;
        else
            resource->flags &= ~4;
    }
    return 0;
}
