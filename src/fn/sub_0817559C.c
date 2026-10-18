#define SceneCatalog_SetSelection     sub_0817559C
#define SceneCatalog_RefreshSelection sub_08174E38
#include "global.h"
struct SceneCatalogAnimation {
    u8 unknown0[8];
    u32 flags;
};
struct SceneCatalog {
    u8 unknown0[0x64];
    u8 animationResource[0x159C];
    struct SceneCatalogAnimation selectionAnimation;
    u8 unknown160C[0x329E];
    u8 selection;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void sub_08174E38(struct SceneCatalog *);
void sub_0817559C(struct SceneCatalog *s, u32 selection)
{
    struct SceneCatalogAnimation *animation = &s->selectionAnimation;
    animation->flags &= ~1;
    Anim_LoadKeyframe(animation, s->animationResource, 0x7C, 0);
    s->selection = selection;
    sub_08174E38(s);
}
