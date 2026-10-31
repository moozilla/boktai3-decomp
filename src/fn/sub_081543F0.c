#include "player_cue_sequence.h"

// confidence=low: tentative cue role from callback phases.

struct PlayerSideFadeCue {
    u8 pad0[0x14C];
    struct ContactSwarmSprite sprite;
    u8 pad1[0x3A2 - 0x14C - sizeof(struct ContactSwarmSprite)];
    u8 heading, side, facing;
    u8 pad2[0x3FC - 0x3A5];
    struct DisplaySequenceState sequence;
    u32 unknown40C;
    const void *resource;
    const void *resource2;
    u8 pad3[0x458 - 0x418];
    u8 phase;
    u8 pad4;
    u16 timer;
    u8 pad5[0x4E8 - 0x45C];
    u16 scale;
    u8 pad6[0x58C - 0x4EA];
    void (*updateCallback)(struct PlayerSideFadeCue *);
};
void ActorSequence_Begin(struct PlayerSideFadeCue *, s32, s32);
void Player_AddControlFlags(struct PlayerSideFadeCue *, u32);
void PlayerCue_UpdateHidden(struct PlayerSideFadeCue *);
void PlayerCue_CompleteAction(struct PlayerSideFadeCue *);
void PlayerCue_EnterStandUpdate(struct PlayerSideFadeCue *);
void Player_RestartAnimationFromResource(struct PlayerSideFadeCue *);
void Player_UpdateFacingSide(struct PlayerSideFadeCue *);

void PlayerCue_UpdateSideFade(struct PlayerSideFadeCue *p)
{
    switch (p->phase) {
        case 0:
            if (!p->timer) {
                p->facing = 4;
                Player_UpdateFacingSide(p);
            }
            Anim_SetSequence(&p->sequence, p->resource, 0, p->heading, p->side);
            PlayerCue_StepSequence(&p->sprite, &p->sequence);
            if ((++p->timer) > 27)
                ActorSequence_Begin(p, 7, 1);
            break;

        case 1:
            p->sprite.flags |= 2;
            p->sprite.unknown8[0] = 64 - (((u8)p->timer) << 2);
            if (p->timer > 8) {
                p->sprite.unknown8[1] = p->timer << 3;
                p->scale = (p->timer - 8) * 20;
                Player_AddControlFlags(p, 4);
            }
            if ((++p->timer) > 15)
                ActorSequence_Begin(p, 7, 2);
            break;

        case 2:
            Player_AddControlFlags(p, 4);
            p->scale = (p->timer * 200) + 160;
            if ((++p->timer) > 7) {
                Player_AddControlFlags(p, 1);
                ActorSequence_Begin(p, 0, 0);
                p->updateCallback = PlayerCue_UpdateHidden;
                PlayerCue_CompleteAction(p);
            }
            break;

        case 3:
            if (!p->timer) {
                p->facing = 4;
                Player_UpdateFacingSide(p);
            }
            Anim_SetSequence(&p->sequence, p->resource, 0, p->heading, p->side);
            PlayerCue_StepSequence(&p->sprite, &p->sequence);
            Player_AddControlFlags(p, 4);
            p->sprite.flags |= 2;
            p->sprite.unknown8[0] = 4;
            p->sprite.unknown8[1] = 120;
            p->scale = ((8 - p->timer) * 200) + 160;
            if ((++p->timer) > 7)
                ActorSequence_Begin(p, 7, 4);
            break;

        case 4:
            Anim_SetSequence(&p->sequence, p->resource, 0, p->heading, p->side);
            PlayerCue_StepSequence(&p->sprite, &p->sequence);
            p->sprite.flags |= 2;
            p->sprite.unknown8[0] = 64 - ((15 - ((u8)p->timer)) << 2);
            if (p->timer <= 7) {
                p->sprite.unknown8[1] = (15 - p->timer) << 3;
                p->scale = (8 - p->timer) * 20;
            } else {
                p->sprite.unknown8[1] = 64;
                p->scale = 0;
                Player_AddControlFlags(p, 4);
            }
            if ((++p->timer) > 15)
                ActorSequence_Begin(p, 7, 5);
            break;

        case 5:
            p->sprite.flags &= ~2;
            Anim_SetSequence(&p->sequence, p->resource, 0, p->heading, p->side);
            PlayerCue_StepSequence(&p->sprite, &p->sequence);
            if ((++p->timer) > 3) {
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                p->updateCallback = Player_RestartAnimationFromResource;
                PlayerCue_CompleteAction(p);
            }
            break;
    }
}
