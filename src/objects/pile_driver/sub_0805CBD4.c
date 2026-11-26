#include "objects/pile_driver/pile_driver_subject.h"
void PileDriverSubject_ResetPosition(struct ScenePanelSubject *subject)
{
    s32 random_2;
    s32 offset;
    s32 random;
    const u16 *table;
    subject->drawable.position = subject->home;
    subject->positionReset = 1;
    table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    random_2 = Mod(*((const u16 *)(offset + ((u32)table))), 10);
    subject->drawable.position.x += random_2 - 5;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    random = Mod(*(const u16 *)(offset + (u32)table), 10);
    subject->drawable.position.z += random - 5;
}
