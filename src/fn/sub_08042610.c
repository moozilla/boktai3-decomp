#include "display_animation.h"
struct DisplayResourceHeader {
    u32 words[8];
};
s32 sub_0821983C(void *, void *, u32, s32, u32, u32, u32, const struct DisplayPosition *);
s32 DisplayAnimation_InitializeKeyframes(struct DisplayAnimation *display, u32 resourceId,
                                         s32 flags, const struct DisplayPosition *position)
{
    struct DisplayAnimationResource *resource;
    const struct DisplayAnimationResource *source;
    u32 inactive = display->kind;
    if (inactive)
        goto failed;
    Memory_Clear(display, sizeof(struct DisplayAnimation));
    display->kind = 2;
    resource = &display->resource;
    source = ScriptProc_Start(0xCB05, (u16)resourceId);
    if (source)
        goto setup;
failed:
    return -1;
setup:
    *(struct DisplayResourceHeader *)resource = *(const struct DisplayResourceHeader *)source;
    ResourceHeader_Relocate(resource, (void *)source);
    {
        s32 result =
            sub_0821983C((u8 *)display + 0x2C, resource, 0, flags, 1, inactive, 60, position);
        display->position = (struct DisplayPosition *)((u8 *)display + 0x4C);
        display->displayFlags = (u32 *)((u8 *)display + 0x34);
        return result;
    }
}
