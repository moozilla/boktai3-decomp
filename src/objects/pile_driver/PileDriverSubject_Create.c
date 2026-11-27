#include "objects/pile_driver/pile_driver_subject_tail.h"
struct PileDriverSubject *PileDriverSubject_Create(const struct ContactPosition *home, u32 row,
                                                   u32 variant, u32 state, u32 kind,
                                                   u32 contactLimit, u32 colliderSetting,
                                                   u32 appearDelay)
{
    struct PileDriverSubject *subject = Task_Create(8, 0x240);
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
