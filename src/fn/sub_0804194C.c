// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
extern u32 gUnk_030053F0;
extern struct ModifierMenuActor *gUnk_020004B4;
void sub_0811C9F8(u8 *, u8 *, u32);
void sub_08041908(struct ModifierMenu *);
void sub_0811CB68(u8 *, u8 *, struct ModifierPreviewOrigin *, u32);
void sub_0811CD24(u8 *, s32);
void sub_0811CD5C(u8 *, s32);
void sub_0811C844(u8 *, s32);
void ModifierMenu_InitializePreview(struct ModifierMenu *menu)
{
    struct ModifierPreviewOrigin origin;
    u8 *resource;
    u8 *model;
    u32 flags;
    u32 mask;
    origin.x = 48;
    origin.y = 80;
    origin.z = 0;
    mask = 0x800;
    flags = (gUnk_030053F4 | gUnk_030053F0) & mask;
    if (flags) {
        s32 id;
        struct ModifierMenuActor *actor = gUnk_020004B4;
        if (!actor) {
            id = -1;
            asm("" : "+r"(id));
        } else
            id = actor->id;
        menu->previewActorId = id;
    } else {
        menu->previewActorId = flags;
    }
    resource = (u8 *)menu + 0x83C;
    model = (u8 *)menu + 0xE34;
    sub_0811C9F8(resource, model, 0x3D99);
    sub_08041908(menu);
    sub_0811CB68(resource, model, &origin, 0);
    sub_0811CD24(model, 16);
    sub_0811CD5C(model, 1);
    sub_0811C844(model, gUnk_02000710->unk81A);
}
