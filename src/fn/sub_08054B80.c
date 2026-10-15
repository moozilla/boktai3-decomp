// MATCHING_CONCESSION: register-pinning
#include "display_track.h"
struct DisplayTrackTagView {
    u8 prefix[0x1C];
    u16 tag;
};
struct DisplayTrack *DisplayTrack_FindFromScript(void)
{
    struct DisplayTrackManager **manager = &gUnk_02000494;
    struct DisplayTrackManager *current;
    struct DisplayTrackManager *loaded;
    register s32 length asm("r1");
    u32 tag, offset;
    s32 i;
    register s32 count asm("r4");
    struct DisplayTrackTagView *scan;
    if (!*manager)
        goto missing;
    if (Script_SeekToKeyword('n'))
        goto search;
    goto missing;
found: {
    u32 addressOffset = offset + 0x1C;
    return (void *)((u8 *)current + addressOffset);
}
search:
    tag = Script_GetValue();
    i = 0;
    loaded = *manager;
    length = loaded->count;
    if (i >= length)
        goto missing;
    current = loaded;
    count = length;
    scan = (void *)current;
    offset = 0;
    do {
        if (scan->tag == tag)
            goto found;
        scan = (void *)((u8 *)scan + sizeof(struct DisplayTrack));
        offset += sizeof(struct DisplayTrack);
    } while (++i < count);
missing:
    return 0;
}
