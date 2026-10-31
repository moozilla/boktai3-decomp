#include "player_cue.h"
#include "player_cue_sequence.h"

// confidence=low: caller selects this update for player state five.
void Player_AddControlFlags(void *, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
static inline u32 Event(struct Player *p, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *events = &p->statBonusFlags;
        u32 result = mask;
        result &= *events;
        return result;
    }
}
static inline u32 Extra(struct Player *p, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *events = &p->actionFlags;
        u32 result = mask;
        result &= *events;
        return result;
    }
}

static inline s32 SignEvent(struct Player *actor)
{
    if (!actor)
        return 0;
    return (s32)actor->statBonusFlags < 0;
}
void Player_UpdateStateMotionCue(struct Player *p)
{
    if (!(p->tetherFrame)) {
        Sound_PlaySong(490);
        if (p->scriptStatus[0])
            p->spriteResource = 45;
        else if (*(gGameState->counters628 + p->timedEventSlot) > 0)
            p->spriteResource = 46;
        else if (Event(p, 0x10000000))
            p->spriteResource = 0x179;
        else if (SignEvent(p))
            p->spriteResource = 0x175;
        else if (Extra(p, 1))
            p->spriteResource = 0x177;
        else if (Event(p, 0x40000000))
            p->spriteResource = 0x178;
        else if (Event(p, 0x20000000))
            p->spriteResource = 0x176;
        else
            p->spriteResource = 51;
    }
    Player_AddControlFlags(p, 64);
    Player_SetAnimation(p, 154, 64, 1);
    p->previousPosition = p->position;
    p->actionVariantResource = (u32)(gPaletteBankData + ((p->spriteResource) * 32));
    if ((p->tetherFrame) <= 9) {
        struct DisplaySequenceState *s = &PlayerCue_Graphics(p)->sequence;
        struct ContactSwarmSprite *draw;
        Anim_SetSequence(s, (void *)(PlayerCue_Graphics(p)->alternateResource),
                         (PlayerCue_Graphics(p)->sequenceSelection) + 9, 0, 0);
        draw = &PlayerCue_Graphics(p)->sprite;
        PlayerCue_StepSequence(draw, s);
    } else if (((p->tetherFrame) >> 2) & 1)
        Player_AddControlFlags(p, 1);
    if ((++(p->tetherFrame)) > 39) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        PlayerCue_EnterStandUpdate(p);
        p->pendingAnimationTrigger = 1;
    }
}
