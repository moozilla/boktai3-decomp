#define SpriteActor_AdvanceContactState sub_08129834
#include "global.h"
struct SpriteActor {
    u8 unknown[0x8C];
    u16 field8C;
    u8 unknown8E[2];
    u16 field90;
    u8 unknown92[0x60];
    u8 contactState;
};
void sub_08129834(void *collision, void *other, struct SpriteActor *actor)
{
    if (actor->contactState == 1) {
        actor->field8C = 0;
        actor->field90 = 0;
        actor->contactState = 2;
    }
}
