#include "global.h"
struct Item {
    u8 pad[0x25];
    u8 kind;
};
struct Style {
    u16 args[6];
};
extern const struct Style gUnk_0860582C[];
void sub_0811D428(void *, int, int, int, int, int, int, u32, u32);
void sub_0811F338(void *, void *, int);
void sub_0811E534(void *, int);
void ScenePool_InitActorBounds(void *, struct Item *);
void CharacterModel_SetComponentKeyframes(void *, void *);
void CharacterModel_LoadSecondaryPalette(void *, int);
void ScenePool_InitActorAppearance(u8 *p, struct Item *item, u32 a, u32 b)
{
    const struct Style *style = &gUnk_0860582C[item->kind];
    void *drawable;
    sub_0811D428(p + 0x48, style->args[0], style->args[1], style->args[2], style->args[3],
                 style->args[4], style->args[5], a, b);
    sub_0811F338(p, p, 0);
    sub_0811E534(p + 0x318, (s8)p[0x56]);
    p[0x324] = 1;
    ScenePool_InitActorBounds(p, item);
    drawable = p + 0x600;
    CharacterModel_SetComponentKeyframes(p, drawable);
    CharacterModel_LoadSecondaryPalette(drawable, p[0x4b]);
}
