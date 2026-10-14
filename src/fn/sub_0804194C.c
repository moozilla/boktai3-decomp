// MATCHING_CONCESSION: register-pinning
#include "engine.h"
#define ModifierMenu_InitializePreview sub_0804194C
struct PreviewOrigin {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct PreviewActor {
    u8 unknown0[0x18];
    s16 id;
};
extern u32 gUnk_030053F0;
extern struct PreviewActor *gUnk_020004B4;
void sub_0811C9F8(u8 *, u8 *, u32);
void sub_08041908(u8 *);
void sub_0811CB68(u8 *, u8 *, struct PreviewOrigin *, u32);
void sub_0811CD24(u8 *, s32);
void sub_0811CD5C(u8 *, s32);
void sub_0811C844(u8 *, s32);
void ModifierMenu_InitializePreview(u8 *menu)
{
    struct PreviewOrigin origin;
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
        struct PreviewActor *actor = gUnk_020004B4;
        if (!actor) {
            id = -1;
            asm("" : "+r"(id));
        } else
            id = actor->id;
        menu[0x1086] = id;
    } else {
        menu[0x1086] = flags;
    }
    resource = menu + 0x83C;
    model = menu + 0xE34;
    sub_0811C9F8(resource, model, 0x3D99);
    sub_08041908(menu);
    sub_0811CB68(resource, model, &origin, 0);
    sub_0811CD24(model, 16);
    sub_0811CD5C(model, 1);
    sub_0811C844(model, gUnk_02000710->unk81A);
}
