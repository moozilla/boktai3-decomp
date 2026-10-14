#include "display_animation.h"
#define DisplayAnimation_InitializeSequence sub_08042588
s32 DisplayAnimation_InitializeSequence(struct DisplayAnimation *display, s32 resourceId,
                                        u32 sequenceId, s32 flags,
                                        const struct DisplayPosition *position)
{
    struct DisplayAnimationResource *resource;
    struct DisplaySequenceSprite *sprite;
    if (display->kind)
        goto failed;
    Memory_Clear(display, sizeof(struct DisplayAnimation));
    display->kind = 1;
    resource = &display->resource;
    if (!AnimData_Load(resource, resourceId))
        goto failed;
    if (!sequenceId)
        goto noSequence;
    resource->sequence = ScriptProc_Start(0x922E, (u16)sequenceId);
    if (resource->sequence)
        goto setup;
failed:
    return -1;
noSequence:
    resource->sequence = (void *)sequenceId;
setup:
    sprite = (struct DisplaySequenceSprite *)((u8 *)resource + 0x1C);
    Sprite_Init(sprite, resource, flags);
    *(struct DisplayPosition *)((u8 *)resource + 0x38) = *position;
    display->position = (struct DisplayPosition *)((u8 *)resource + 0x38);
    display->displayFlags = &sprite->flags;
    return 0;
}
