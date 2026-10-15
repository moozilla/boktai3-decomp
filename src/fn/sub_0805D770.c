#include "scene_panel_subject_tail.h"
#define ScenePanelSubject_UpdateGraphicTiles sub_0805D770
void ScenePanelSubject_UpdateGraphicTiles(struct ScenePanelSubjectTail *subject)
{
    if (subject->requested) {
        if (subject->requestLevel <= 11)
            subject->requestLevel++;
        subject->requested = 0;
    } else if (subject->requestLevel)
        subject->requestLevel--;
    if (subject->requestLevel == 12)
        AnimData_SelectTiles(subject->graphics, 0x250);
    else if (subject->requestLevel)
        AnimData_SelectTiles(subject->graphics, 0x24F);
    else if (subject->contactKind) {
        AnimData_SelectTiles(subject->graphics, 0x192);
        subject->contactKind--;
    } else
        AnimData_SelectTiles(subject->graphics, subject->tile);
}
