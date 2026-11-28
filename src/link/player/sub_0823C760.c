#include "global.h"
#include "player/player.h"
#include "scene/areas/area_effect_link.h"

s32 Player_GetLinkedSmoothedDirection(struct Player *);
s32 LinkPlayer_HasSlotValueAtLeast(struct Player *, s32);
void Player_PlayLocalSong(s32, u32);
void Player_SetTetherMode(struct Player *, s32, s32);
s32 Div(s32, s32);
void LinkPlayer_SetScaledDirectionalVelocity(struct Player *, s32);
void Player_PlayActionCueSound(struct Player *, u32);
void Player_UpdateActionFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);

static inline u8 LinkPlayer_GetSlotValue(struct Player *p)
{
    s32 slot = p->slot;

    if (gLinkSession == NULL || slot < 0)
        return 0;
    return gLinkSession->playerValues[slot];
}

void sub_0823C760(struct Player *p)
{
    if (p->tetherMode != 2) {
        if (LinkPlayer_HasSlotValueAtLeast(p, 1)) {
            Player_PlayLocalSong(p->slot, 0x392);
            p->unk4EF[1] = LinkPlayer_GetSlotValue(p) * 2 + 14;
            if (p->unk4EF[1] > 0x20)
                p->unk4EF[1] = 0x20;
            p->unk4EF[2] = Div(0x200, p->unk4EF[1]);
            Player_SetTetherMode(p, 2, 0);
        } else {
            Player_PlayLocalSong(p->slot, 0x192);
            Player_SetTetherMode(p, 2, 1);
        }
    }
    switch (p->tetherPhase) {
        case 0:
            if (++p->tetherFrame >= p->unk4EF[2]) {
                Player_SetTetherMode(p, 0, 0);
                break;
            }
            if (p->tetherFrame < p->unk4EF[2] - 8) {
                p->actionEighteenParameter = 0x172;
                p->actionEighteenDuration = 0x20;
            }
            if ((s16)p->directionHistory[0] != -1 && p->cameraOffsetActive == 0)
                p->facing = Player_GetLinkedSmoothedDirection(p);
            LinkPlayer_SetScaledDirectionalVelocity(p, p->unk4EF[1]);
            Player_PlayActionCueSound(p, 1);
            Player_UpdateActionFacingSide(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 1), 0x20);
            break;
        case 1:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40);
            p->tetherFrame++;
            if (p->tetherFrame > 7)
                Player_SetTetherMode(p, 0, 0);
            break;
    }
}
