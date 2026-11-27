#include "global.h"
#include "engine/video/render.h"

struct A {
    u8 pad[0x574];
    u32 counter;
};

void Vanargand_UpdateAnimationAndTargets(struct A *p);
void Vanargand_RunModeCallback(struct A *p);
void Vanargand_ApplyRequestedMode(struct A *p);
void Vanargand_UpdateStateCallback(struct A *p);
void Vanargand_UpdatePresentation(struct A *p);

void Vanargand_Update(struct A *p)
{
    if ((gEnginePauseFlags & 7) == 0) {
        Vanargand_UpdateAnimationAndTargets(p);
        Vanargand_RunModeCallback(p);
        Vanargand_ApplyRequestedMode(p);
        Vanargand_UpdateStateCallback(p);
        Vanargand_UpdatePresentation(p);
        p->counter = ((p->counter + 1) & 0xFFFF) + 1;
    }
}
