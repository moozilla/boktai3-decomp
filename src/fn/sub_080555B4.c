#include "display_track.h"
void Shadow_Destroy(void);
s32 DisplayTrack_DestroyManager(struct DisplayTrackManager *manager)
{
    s32 i;
    for (i = 0; i < manager->count; i++) {
        u32 offset = i * sizeof(struct DisplayTrack);
        void **resource;
        u8 *base;
        /* Manager+44 is the first sprite; manager+D0 is its owned resource. */
        Sprite_Remove((void *)(offset + (u32)manager + 0x44));
        base = (u8 *)manager + 0xD0;
        resource = (void **)(base + offset);
        if (*resource) {
            Shadow_Destroy();
            Heap_Free(*resource);
        }
    }
    gDisplayTrackManager = 0;
    return 0;
}
