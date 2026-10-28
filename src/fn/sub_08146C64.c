// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "engine_globals.h"
// confidence=low: phases coordinate attachment creation, facing and a fading control cue.
extern u8 *gUnk_02000488;
extern u8 *gGameState;
void Sound_PlaySong(u32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, int, int, int, int);
s32 Math_Atan2(s32, s32);
void Player_UpdateFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_SetActionPhase(struct Player *, u8);
void Player_AddControlFlags(struct Player *, s32);
void Player_UpdateAttachmentCue(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (!p->tetherFrame) {
                s32 song = 0xE7;
                if (gUnk_02000488 && *(s16 *)(gGameState + 0x5AA) == 9)
                    song = 0x3F6;
                Sound_PlaySong(song);
                p->kindTwoAttachmentDescriptor = 0;
                p->kindTwoAttachmentCounter = 0;
                p->kindTwoAttachment = ActorAttachment_UpdateFromDescriptor(
                    p->kindTwoAttachment, &p->positionalEffectGroup, p->kindTwoAttachmentDescriptor,
                    1, 0, 0, 0, 0x50);
                Sound_PlaySong(0x1B1);
                if (gUnk_02000488) {
                    register s32 direction asm("r1");
                    direction =
                        7 & (((u16)Math_Atan2(*(s16 *)(gUnk_02000488 + 0x50) - p->position.x,
                                              *(s16 *)(gUnk_02000488 + 0x54) - p->position.z) +
                              0x70) >>
                             5);
                    p->facing = direction;
                    Player_UpdateFacingSide(p);
                }
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
            if (++p->tetherFrame > 0x59) {
                Sound_PlaySong(0xD0);
                Player_SetActionPhase(p, 1);
            }
            break;
        case 1:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x31), 0x40, 1))
                Player_SetActionPhase(p, 2);
            break;
        case 2:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x32), 0x40, 1);
            if (++p->tetherFrame > 15) {
                Player_SetActionPhase(p, 3);
                gEngineFlags |= 0x400;
            }
            break;
        case 4:
            if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 3))
                Player_AddControlFlags(p, 5);
            if (++p->tetherFrame > 0x37)
                Player_SetActionPhase(p, 5);
            break;
        case 5:
            Player_AddControlFlags(p, 5);
            break;
    }
}
