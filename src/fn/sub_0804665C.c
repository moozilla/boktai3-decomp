#include "contact_sequence_actor.h"
s32 ContactSequencePool_Update(struct ContactSequenceOwner *);
s32 ContactSequencePool_Destroy(struct ContactSequenceOwner *);
s32 ContactSequencePool_Initialize(struct ContactSequenceOwner *, s32, u32, s32, u32, u32);
struct ContactSequenceOwner *ContactSequencePool_Create(s32 initialHealth, u32 contactValue,
                                                        s32 resourceValue, u32 script,
                                                        u32 reactionValue)
{
    struct ContactSequenceOwner *owner = Task_Create(8, sizeof(*owner));
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))ContactSequencePool_Update,
                          (void (*)(void *))ContactSequencePool_Destroy);
        if (ContactSequencePool_Initialize(owner, initialHealth, contactValue, resourceValue,
                                           script, reactionValue) < 0) {
            Task_Kill(owner);
            return NULL;
        }
    }
    return owner;
}
