#include "contact_sequence_actor.h"
#define ContactSequencePool_Create     sub_0804665C
#define ContactSequencePool_Update     sub_0804650C
#define ContactSequencePool_Destroy    sub_080465CC
#define ContactSequencePool_Initialize sub_080465F4
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
