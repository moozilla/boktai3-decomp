#include "scene_panel_subject_tail.h"
#include "scene_panel_subject_playback.h"
s32 ScenePanelSubject_Initialize(struct ScenePanelSubject *subject,
                                 const struct ContactPosition *home, u32 row, u32 variant,
                                 u32 initialState, u32 kind, u32 contactLimit, u32 colliderSetting,
                                 u32 appearDelay)
{
    struct ContactSwarmSprite *sprite;
    subject->home = *home;
    subject->kind = kind;
    subject->sequenceRow = row;
    subject->sequenceVariant = variant;
    subject->appearDelay = appearDelay;
    subject->contactLimit = contactLimit;
    subject->progressStep = Div(subject->contactLimit, 6);
    sprite = &subject->drawable;
    AnimData_Load(subject->graphics, 0xC038);
    Sprite_Init(sprite, subject->graphics, 0);
    sprite->position = subject->home;
    subject->sequenceResource = ScriptProc_Start(0x922E, 0x3449);
    ScenePanelSubject_InitCollider(subject, colliderSetting);
    ScenePanelSubject_SetState((void *)subject, initialState);
    switch (subject->state) {
        case 0:
            subject->contactProgress = 0;
            subject->paletteBank = 0x24C;
            subject->drawable.priority = 3;
            subject->drawable.flags |= 1;
            break;
        case 2:
            subject->contactProgress = 0;
            subject->paletteBank = 0x24C;
            Anim_SetSequence(&subject->sequence, subject->sequenceResource, 1, subject->sequenceRow,
                             subject->sequenceVariant);
            ScenePanelSubject_StepPlayback(subject);
            Collider_Refresh(&subject->collider);
            break;
        case 4: {
            const u16 *limit = &subject->contactLimit;
            subject->contactProgress = *limit;
        }
            subject->paletteBank = 0x24D;
            Anim_SetSequence(&subject->sequence, subject->sequenceResource, 3, subject->sequenceRow,
                             subject->sequenceVariant);
            ScenePanelSubject_StepPlayback(subject);
            Collider_Refresh(&subject->collider);
            break;
    }
    AnimData_SelectPaletteBank(subject->graphics, subject->paletteBank);
    subject->timer = 0;
    subject->changed = 0;
    sub_08232264(subject->effects, &subject->drawable.position, 0, 0, 0);
    subject->unknownEE = 0;
    return 0;
}
