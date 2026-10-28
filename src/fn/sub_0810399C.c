/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "global.h"
#include "ram.h"
struct ActorGroupMember {
    u8 unk0[0xB8];
    u16 flags;
    u8 unkBA[0x46];
    u8 resource[0x48];
    u8 effect[0x54];
};
struct ActorGroup {
    u8 unk0[0x1C];
    struct ActorGroupMember members[12];
    u32 active;
};
void Sound_StopSong(u32);
void TerrainModifiers_UnregisterCell(void *);
void SceneBody_Unregister(void *);
void Sprite_Remove(void *);
void ActorGroup_ReleaseMember(void *);
static inline u8 HasBit(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
u32 ActorGroup_Destroy(struct ActorGroup *group)
{
    s32 i;
    u32 *active;
    Sound_StopSong(0x158);
    Sound_StopSong(0x261);
    Sound_StopSong(0x289);
    Sound_StopSong(0x28A);
    Sound_StopSong(0x288);
    i = 0;
    active = &group->active;
    for (; i < 12; i++) {
        if (HasBit(*active, 1 << i)) {
            struct ActorGroupMember *member = &group->members[i];
            if (HasBit(member->flags, 1))
                TerrainModifiers_UnregisterCell(member->effect);
            ActorGroup_ReleaseMember(member);
            SceneBody_Unregister(member->resource);
            Sprite_Remove(member);
        }
    }
    return (u32)(gNamedActorManager = 0);
}
