#define ActorGroup_FindMemberByName NamedObject_FindByName
/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "global.h"
#include "ram.h"
struct ActorGroupMember {
    u8 unk0[0xB4];
    s16 index;
    u16 state;
    u16 flags;
    u8 unkBA[0x22];
    s32 id;
    u8 unkE0[0x20];
    u8 resource[0x48];
    u8 effect[0x54];
};
struct ActorGroup {
    u8 unk0[0x1C];
    struct ActorGroupMember members[12];
    u32 active;
};
s32 Script_GetKeywordValue(u32, s32);
struct ActorGroupMember *NamedObject_FindByName(s32);
void TerrainModifiers_UnregisterCell(void *);
void ActorGroup_ReleaseMember(void *);
void SceneBody_Unregister(void *);
void Sprite_Remove(void *);
static inline u8 HasBit(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 MemberHasBit(struct ActorGroupMember *m, u32 mask)
{
    if (m->flags & mask)
        return 1;
    return 0;
}
static inline void ClearFlags(struct ActorGroupMember *m, u32 mask)
{
    m->flags &= ~mask;
}
void ActorGroup_RemoveFromScript(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct ActorGroupMember *member;
    struct ActorGroup *group;
    if (gNamedActorManager && name) {
        member = NamedObject_FindByName(name);
        group = gNamedActorManager;
        if (MemberHasBit(member, 1)) {
            TerrainModifiers_UnregisterCell(member->effect);
            ClearFlags(member, 1);
        }
        ActorGroup_ReleaseMember(member);
        SceneBody_Unregister(member->resource);
        member->id = 0;
        member->state = 6;
        Sprite_Remove(member);
        {
            s32 index = member->index;
            group->active &= ~(1 << index);
        }
        member->index = -1;
    }
}
