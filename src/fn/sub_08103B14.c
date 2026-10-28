/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "global.h"
#include "ram.h"
struct ActorGroupMember {
    u8 unk0[0xB8];
    u16 flags;
    u8 unkBA[0x22];
    s32 id;
    u8 unkE0[0xBC];
};
struct ActorGroup {
    u8 unk0[0x1C];
    struct ActorGroupMember members[12];
    u32 active;
};
u32 TetherActor_TestStatusFlags(u32);
static inline u8 HasBit(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
s32 ActorGroup_GetSelectedMemberId(void)
{
    struct ActorGroup *group;
    struct ActorGroupMember *member;
    s32 i;
    if (!gNamedActorManager)
        goto fail;
    group = gNamedActorManager;
    if (!group->active)
        goto fail;
    if (TetherActor_TestStatusFlags(1))
        goto scan;
    goto fail;
found:
    return member->id;
scan:
    for (i = 0; i < 12; i++) {
        if (HasBit(group->active, 1 << i)) {
            member = &group->members[i];
            if (HasBit(member->flags, 0x10))
                goto found;
        }
    }
fail:
    return -1;
}
