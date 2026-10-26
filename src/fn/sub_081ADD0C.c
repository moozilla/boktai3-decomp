#include "global.h"
#include "render.h"

struct A {
    u8 pad[0x574];
    u32 counter;
};

void sub_081AD3E8(struct A *p);
void Vanargand_RunModeCallback(struct A *p);
void Vanargand_ApplyRequestedMode(struct A *p);
void Vanargand_UpdateStateCallback(struct A *p);
void sub_081AD564(struct A *p);

void Vanargand_Update(struct A *p)
{
    if ((gEnginePauseFlags & 7) == 0) {
        sub_081AD3E8(p);
        Vanargand_RunModeCallback(p);
        Vanargand_ApplyRequestedMode(p);
        Vanargand_UpdateStateCallback(p);
        sub_081AD564(p);
        p->counter = ((p->counter + 1) & 0xFFFF) + 1;
    }
}
