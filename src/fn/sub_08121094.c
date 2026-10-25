#include "global.h"
#include "engine.h"

struct DrawFlags {
    u8 unk_00[0x16];
    u16 flags;
};

struct A8CBObject {
    u8 unk_00[0x88C];
    struct DrawFlags draw;
};

struct A8CBObject *sub_08120FF4(void *owner);

static inline void ClearDrawFlags(struct DrawFlags *draw, s32 mask)
{
    draw->flags &= mask;
}

void RegistryA8CB_DisableCommandsFromScript(void)
{
    s32 name;
    struct A8CBObject *object;

    name = Script_GetKeywordValue('n', -1);
    if (name < 0)
        return;
    object = sub_08120FF4((void *)name);
    if (object == NULL)
        return;
    ClearDrawFlags(&object->draw, ~1);
}
