#include "scene_panel_subject.h"
void ScenePanelSubject_ApplyContact(struct ContactCollider *contact, struct ContactCollider *other,
                                    void *owner)
{
    u32 mask = 127;
    if (contact->reactionFlags & mask) {
        struct ScenePanelSubjectView *subject = owner;
        if (subject->contactCooldown == 0 && subject->state == 2) {
            s32 amount = contact->unknown3C - other->unknown3C;
            if (amount < 0)
                amount = 1;
            subject->contactKind = 4;
            subject->contactProgress += amount;
            if (subject->contactProgress >= subject->contactLimit) {
                subject->contactProgress = subject->contactLimit;
                ScenePanelSubject_SetState((void *)subject, 3);
            } else {
                subject->contactCooldown = 12;
                Sound_PlaySong(0x127);
            }
        }
    }
}
