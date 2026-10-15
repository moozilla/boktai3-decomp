#include "scene_panel_subject.h"
#define ScenePanelSubject_SetState sub_0805CB88
void ScenePanelSubject_SetState(struct SceneRigSubject *owner, u32 state)
{
    struct ScenePanelSubjectView *subject = (void *)owner;
    subject->state = state;
    subject->stateFrame = 0;
    subject->update = gUnk_08605AF8[subject->state];
    if (subject->state == 3)
        Sound_PlaySong(0xFB);
    else if (subject->state == 5)
        Sound_PlaySong(0xFC);
}
