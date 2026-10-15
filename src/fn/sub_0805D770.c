#include "scene_panel_subject_tail.h"
void ScenePanelSubject_UpdatePaletteBank(struct ScenePanelSubjectView *subject)
{
    if (subject->requested) {
        if (subject->requestLevel <= 11)
            subject->requestLevel++;
        subject->requested = 0;
    } else if (subject->requestLevel)
        subject->requestLevel--;
    if (subject->requestLevel == 12)
        AnimData_SelectPaletteBank(subject->graphics, 0x250);
    else if (subject->requestLevel)
        AnimData_SelectPaletteBank(subject->graphics, 0x24F);
    else if (subject->contactKind) {
        AnimData_SelectPaletteBank(subject->graphics, 0x192);
        subject->contactKind--;
    } else
        AnimData_SelectPaletteBank(subject->graphics, subject->paletteBank);
}
