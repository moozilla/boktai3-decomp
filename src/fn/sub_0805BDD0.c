#include "scene_panel_config.h"
void ScenePanel_CreateSubjects(struct ScenePanelConfigView *parent)
{
    u32 initialStates[4];
    struct ScenePanelSpawnPosition position;
    s32 contactLimit, colliderSetting;
    u32 category = parent->category;
    u32 kind = 0;
    s32 i;
    if (category == 1)
        kind = 1;
    if (!Script_SeekToKeyword('g')) {
        contactLimit = 100;
        colliderSetting = 0;
    } else {
        contactLimit = Script_GetValue();
        colliderSetting = Script_GetValue();
    }
    if (!Script_SeekToKeyword('a')) {
        for (i = 0; i < parent->subjectCount; i++) {
            parent->subjects[i] = 0;
            initialStates[i] = 0;
        }
    } else {
        for (i = 0; i < parent->subjectCount; i++) {
            parent->subjects[i] = 0;
            initialStates[i] = Script_GetValue();
        }
    }
    parent->parameter = Script_GetKeywordValue('S', 0);
    {
        for (i = 0; i < parent->subjectCount; i++) {
            u32 row, variant, appearDelay;
            s32 negative = -514;
            position = *(struct ScenePanelSpawnPosition *)&parent->origin;
            switch (i) {
                case 0:
                    position.x += negative;
                    position.z += 76;
                    row = 1;
                    variant = 0;
                    appearDelay = 6;
                    break;
                case 2:
                    position.x += 76;
                    position.z += negative;
                    row = 1;
                    variant = 1;
                    appearDelay = 3;
                    break;
                case 3:
                    position.x -= 76;
                    position.z += 514;
                    row = 0;
                    variant = 0;
                    appearDelay = 0;
                    break;
                case 1:
                    position.x += 514;
                    position.z -= 76;
                    row = 0;
                    variant = 1;
                    appearDelay = 9;
                    break;
                default:
                    row = 0;
                    variant = 0;
                    appearDelay = 0;
            }
            parent->subjects[i] = (struct SceneRigSubject *)ScenePanelSubject_Create(
                (const void *)&position, row, variant, initialStates[i], kind, contactLimit,
                colliderSetting, appearDelay);
        }
    }
}
