#include "objects/pile_driver/scene_panel_subject_tail.h"
struct ScenePanelSubject *PileDriverSubject_Create(const struct ContactPosition *home, u32 row,
                                                   u32 variant, u32 state, u32 kind,
                                                   u32 contactLimit, u32 colliderSetting,
                                                   u32 appearDelay)
{
    struct ScenePanelSubject *subject = Task_Create(8, 0x240);
    if (subject) {
        Task_SetCallbacks(subject, (void (*)(void *))PileDriverSubject_Update,
                          (void (*)(void *))PileDriverSubject_Destroy);
        if (PileDriverSubject_Initialize(subject, home, row, variant, state, kind, contactLimit,
                                         colliderSetting, appearDelay) < 0) {
            Task_Kill(subject);
            return 0;
        }
    }
    return subject;
}
