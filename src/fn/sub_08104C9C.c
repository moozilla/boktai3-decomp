#define ActorGroup_UpdateMemberRemoval sub_08104C9C
#define ActorGroup_CallMemberScript    sub_081042DC
/* Provisional ActorGroup names: static behavior established; semantic ownership tentative. */
#include "global.h"
struct ActorGroupMember {
    u32 flags;
    u8 unk4[0x54];
    u8 collider[0x54];
    u8 marker[0x9A];
    u8 value146, value147;
    u8 unk148[10];
    u16 state;
    s16 index;
    u8 unk156[2];
    s8 markerId;
    u8 unk159[5];
    u8 started;
};
struct ActorGroupRemovalPool {
    u8 unk0[0x62C];
    u32 active;
};
void sub_0810523C(struct ActorGroupMember *);
void sub_081055C0(struct ActorGroupMember *, s32);
void ScaledShadow_Destroy(void *);
void sub_081042DC(struct ActorGroupMember *);
void Sound_PlaySong(u32);
void Sprite_Remove(void *);
void Collider_Remove(void *);
void sub_08104C9C(struct ActorGroupRemovalPool *group, struct ActorGroupMember *member)
{
    if (member->started) {
        sub_0810523C(member);
        if (member->value147 - member->value146 >= 0)
            Sound_PlaySong(0x3C7);
        sub_081055C0(member, member->value147 - member->value146);
        if (member->markerId != -1) {
            ScaledShadow_Destroy(member->marker);
            member->markerId = -1;
        }
        Sound_PlaySong(0x3C6);
        sub_081042DC(member);
        member->started = 0;
    } else {
        member->flags |= 1;
        Sprite_Remove(member);
        Collider_Remove(member->collider);
        member->state = 0;
        {
            s32 index = member->index;
            group->active &= ~(1 << index);
        }
        member->index = -1;
    }
}
