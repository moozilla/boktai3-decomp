#include "global.h"
#include "engine.h"

struct A8CBDraw {
    u8 unk_00[0x16];
    u16 flags;
};

struct A8CBObject {
    u8 unk_00[0x84C];
    u32 timer;
    u8 resetKeys;
    u8 unk_851[3];
    void (*update)(struct A8CBObject *);
    u8 unk_858[0x34];
    struct A8CBDraw draw;
};

struct A8CBObject *sub_08120FF4(s32 id);
void ActorMotion_UpdateFallFrame(struct A8CBObject *);

void RegistryA8CB_StartFallFromScript(void)
{
    struct A8CBObject *object;
    s32 id = Script_GetKeywordValue('n', -1);
    if (id < 0)
        return;
    object = sub_08120FF4(id);
    if (object) {
        void (*update)(struct A8CBObject *) = ActorMotion_UpdateFallFrame;
        u8 *resetKeys = &object->resetKeys;
        u32 zero = 0;
        u16 one = 1;
        *resetKeys = one;
        object->update = update;
        object->timer = zero;
        {
            struct A8CBDraw *draw = &object->draw;
            draw->flags = one | draw->flags;
        }
    }
}
