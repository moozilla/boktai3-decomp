#include "scene_panel_subject.h"
void ScenePanelSubject_ResetPosition(struct ScenePanelSubject *subject)
{
    s32 random_2;
    s32 offset;
    s32 random;
    s32 current;
    const u16 *table;
    subject->drawable.position = subject->home;
    subject->positionReset = 1;
    table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    random_2 = Mod(*((const u16 *)(offset + ((u32)table))), 10);
    current = (u16)subject->drawable.position.x;
    current -= 5;
    subject->drawable.position.x = current + random_2;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    random = Mod(*(const u16 *)(offset + (u32)table), 10);
    *(&current) = (u16)subject->drawable.position.z;
    current -= 5;
    subject->drawable.position.z = current + random;
}
