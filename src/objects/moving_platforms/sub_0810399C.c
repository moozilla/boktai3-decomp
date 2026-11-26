/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "objects/moving_platforms/named_object.h"
#include "engine/ram.h"
void Sound_StopSong(u32);
void TerrainModifiers_UnregisterCell(void *);
void SceneBody_Unregister(void *);
void Sprite_Remove(void *);
void MovingPlatform_ReleaseMember(struct NamedObject *);
static inline u8 HasBit(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
u32 MovingPlatform_Destroy(struct NamedObjectManager *group)
{
    s32 i;
    u32 *active;
    Sound_StopSong(0x158);
    Sound_StopSong(0x261);
    Sound_StopSong(0x289);
    Sound_StopSong(0x28A);
    Sound_StopSong(0x288);
    i = 0;
    active = &group->used;
    for (; i < 12; i++) {
        if (HasBit(*active, 1 << i)) {
            struct NamedObject *member = &group->slots[i];
            if (HasBit((u16)member->flags, 1))
                TerrainModifiers_UnregisterCell(member->registration);
            MovingPlatform_ReleaseMember(member);
            SceneBody_Unregister(&member->body);
            Sprite_Remove(member);
        }
    }
    return (u32)(gNamedActorManager = 0);
}
