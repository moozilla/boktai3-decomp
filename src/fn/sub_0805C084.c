#include "scene_panel_config.h"
void ScenePanel_InitResourceConfiguration(struct ScenePanelConfigView *parent)
{
    s32 found = Script_SeekToKeyword('b');
    if (found) {
        parent->resourceIndex = Script_GetValue();
        gUnk_02000710->unk618 = Script_GetValue();
        gUnk_02000710->unk61C = Script_GetValue();
    } else {
        parent->resourceIndex = found;
        {
            struct GameState *volatile *slot = &gUnk_02000710;
            (*slot)->unk618 = 6000;
            (*slot)->unk61C = 6000;
        }
    }
    if (gUnk_02000710->unk618 > (gUnk_02000710->unk61C >> 2))
        parent->warningStage = 0;
    else
        parent->warningStage = 1;
    switch (parent->resourceIndex) {
        case 0:
            parent->warningSounds[0] = 0x534;
            parent->warningSounds[1] = 0x533;
            parent->warningSounds[2] = 0x359;
            parent->warningSounds[3] = 0x535;
            break;
        case 1:
            parent->warningSounds[0] = 0x4D5;
            parent->warningSounds[1] = 0x4D1;
            parent->warningSounds[2] = 0x4CF;
            parent->warningSounds[3] = 0x4D7;
            break;
        case 2:
            parent->warningSounds[0] = 0x53E;
            parent->warningSounds[1] = 0x53D;
            parent->warningSounds[2] = 0x2EB;
            parent->warningSounds[3] = 0x53F;
            break;
        case 3:
            parent->warningSounds[0] = 0x548;
            parent->warningSounds[1] = 0x549;
            parent->warningSounds[2] = 0x547;
            parent->warningSounds[3] = 0x54A;
            break;
        case 4:
            parent->warningSounds[0] = 0x551;
            parent->warningSounds[1] = 0x553;
            parent->warningSounds[2] = 0x552;
            parent->warningSounds[3] = 0x554;
            break;
        default:
            parent->warningSounds[0] = 0;
            parent->warningSounds[1] = 0;
            parent->warningSounds[2] = 0;
            parent->warningSounds[3] = 0;
    }
    if (Script_SeekToKeyword('T'))
        parent->resourceId = Script_GetValue();
    else
        parent->resourceId = 10;
    ScenePanel_InitScheduledAction((void *)parent);
}
