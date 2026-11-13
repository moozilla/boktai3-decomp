#ifndef GUARD_CHARACTER_PREVIEW_ACTOR_H
#define GUARD_CHARACTER_PREVIEW_ACTOR_H
#include "modifier_list.h"
/* Actor setup plus its five-part preview model, shared by link menus. */
struct CharacterPreviewActor {
    u8 unknown0[0x48];
    u8 kind, variant, form, palette, extra;
    u8 unknown4D[0x5F8 - 0x4D];
    struct CharacterPreviewModel model;
    u8 unknown848[2], player, unknown84B[5];
};
#endif
