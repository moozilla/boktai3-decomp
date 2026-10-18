#include "scene_panel_subject.h"
/* ScenePanelSubject_TickInactive: No-op callback for subject state 0; parallels the other Tick state handlers and distinguishes it from TickIdle */
#define ScenePanelSubject_TickInactive sub_0805CD60
#define ScenePanelSubject_Idle         sub_0805CD60
void ScenePanelSubject_Idle(struct ScenePanelSubjectView *subject)
{
}
