// MATCHING_CONCESSION: register-pinning
#include "scene_panel_subject.h"
#define ScenePanelSubject_ResetPosition sub_0805CBD4
void ScenePanelSubject_ResetPosition(struct ScenePanelSubjectView *subject)
{
    s32 offset;
    s32 random;
    register s32 current asm("r1");
    const u16 *table;
    subject->drawable.position = subject->home;
    subject->positionReset = 1;
    table = gUnk_0203B400;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    random = Mod(*(const u16 *)(offset + (u32)table), 10);
    current = (u16)subject->drawable.position.x;
    current -= 5;
    subject->drawable.position.x = current + random;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    random = Mod(*(const u16 *)(offset + (u32)table), 10);
    current = (u16)subject->drawable.position.z;
    current -= 5;
    subject->drawable.position.z = current + random;
}
