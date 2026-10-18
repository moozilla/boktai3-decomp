#define ActorGroup_RemoveFromScript sub_08103C54
#define ActorGroup_FindMemberByName NamedObject_FindByName
#define ActorGroup_ReleaseMember    sub_08101AB4
#define gActorGroup                 gUnk_020001AC
/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "global.h"
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
extern struct ActorGroup *gUnk_020001AC;
s32 Script_GetKeywordValue(u32, s32);
struct ActorGroupMember *NamedObject_FindByName(s32);
void TerrainModifiers_UnregisterCell(void *);
void sub_08101AB4(void *);
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
void sub_08103C54(void)
{
    s32 name = Script_GetKeywordValue('n', 0);
    struct ActorGroupMember *member;
    struct ActorGroup *group;
    if (gUnk_020001AC && name) {
        member = NamedObject_FindByName(name);
        group = gUnk_020001AC;
        if (MemberHasBit(member, 1)) {
            TerrainModifiers_UnregisterCell(member->effect);
            ClearFlags(member, 1);
        }
        sub_08101AB4(member);
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
