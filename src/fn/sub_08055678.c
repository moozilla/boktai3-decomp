// MATCHING_CONCESSION: register-pinning
#include "script_keyframe_pool.h"
struct ScriptKeyframeEntry *ScriptKeyframe_FindFromScript(void)
{
    struct ScriptKeyframeManager **manager = &gUnk_02000498;
    struct ScriptKeyframeManager *current, *loaded;
    register s32 length asm("r1");
    u32 tag, offset;
    s32 i;
    register s32 count asm("r4");
    u16 *scan;
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
    scan = (void *)((u8 *)current + 0xB4);
    offset = 0;
    do {
        if (*scan == tag)
            goto found;
        scan = (void *)((u8 *)scan + sizeof(struct ScriptKeyframeEntry));
        offset += sizeof(struct ScriptKeyframeEntry);
    } while (++i < count);
missing:
    return 0;
}
