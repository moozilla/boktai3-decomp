#include "global.h"

/* Observed component starts; component extents remain unknown. */
struct Ratatosk {
    u8 unknown000[0x188];
    u8 collider;
    u8 unknown189[0x40C - 0x189];
    u8 shadow;
    u8 unknown40D[0xF18 - 0x40D];
    void *childTasks[6];
};

void Task_Kill(void *);
void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
u32 Ratatosk_RemoveAnimation(u8 *);
void SceneActors_Destroy(u8 *);

u32 Ratatosk_Destroy(struct Ratatosk *actor)
{
    if (actor->childTasks[0])
        Task_Kill(actor->childTasks[0]);
    if (actor->childTasks[1])
        Task_Kill(actor->childTasks[1]);
    if (actor->childTasks[2])
        Task_Kill(actor->childTasks[2]);
    if (actor->childTasks[3])
        Task_Kill(actor->childTasks[3]);
    if (actor->childTasks[4])
        Task_Kill(actor->childTasks[4]);
    if (actor->childTasks[5])
        Task_Kill(actor->childTasks[5]);
    ScaledShadow_Destroy(&actor->shadow);
    Collider_Remove(&actor->collider);
    Ratatosk_RemoveAnimation((u8 *)actor);
    SceneActors_Destroy((u8 *)actor);
    return 0;
}
