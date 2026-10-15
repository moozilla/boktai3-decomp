#include "scene_panel_config.h"
void ScenePanel_CreateSubjects(struct ScenePanelConfigView *parent)
{
    u32 profiles[4];
    struct ScenePanelSpawnPosition position;
    s32 valueA, valueB;
    u32 category = parent->category;
    u32 alternate = 0;
    s32 i;
    if (category == 1)
        alternate = 1;
    if (!Script_SeekToKeyword('g')) {
        valueA = 100;
        valueB = 0;
    } else {
        valueA = Script_GetValue();
        valueB = Script_GetValue();
    }
    if (!Script_SeekToKeyword('a')) {
        for (i = 0; i < parent->subjectCount; i++) {
            parent->subjects[i] = 0;
            profiles[i] = 0;
        }
    } else {
        for (i = 0; i < parent->subjectCount; i++) {
            parent->subjects[i] = 0;
            profiles[i] = Script_GetValue();
        }
    }
    parent->parameter = Script_GetKeywordValue('S', 0);
    {
        for (i = 0; i < parent->subjectCount; i++) {
            u32 first, second, direction;
            s32 negative = -514;
            position = *(struct ScenePanelSpawnPosition *)&parent->origin;
            switch (i) {
                case 0:
                    position.x += negative;
                    position.z += 76;
                    first = 1;
                    second = 0;
                    direction = 6;
                    break;
                case 2:
                    position.x += 76;
                    position.z += negative;
                    first = 1;
                    second = 1;
                    direction = 3;
                    break;
                case 3:
                    position.x -= 76;
                    position.z += 514;
                    first = 0;
                    second = 0;
                    direction = 0;
                    break;
                case 1:
                    position.x += 514;
                    position.z -= 76;
                    first = 0;
                    second = 1;
                    direction = 9;
                    break;
                default:
                    first = 0;
                    second = 0;
                    direction = 0;
            }
            parent->subjects[i] = sub_0805DCAC(&position, first, second, profiles[i], alternate,
                                               valueA, valueB, direction);
        }
    }
}
