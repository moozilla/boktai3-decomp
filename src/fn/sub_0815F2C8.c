#include "global.h"
#include "player.h"
#include "contact_swarm.h"
#include "ram.h"
#include "player_cue_sequence.h"
void TimedEffect_GetOrCreateManager(s32);
void *ActorDebris_Create(void *, void *, void *);
void Sprite_Init(void *, void *, s32);
void Player_InitSlotFromScript(struct Player *);
void Player_InitActionMode(struct Player *);
void Player_InitBodyFromScript(struct Player *, s32);
void sub_08140250(struct Player *);
void Player_InitActionRequest(struct Player *);
void Player_RefreshActionEquipment(struct Player *);
void sub_08140244(struct Player *);
void Player_InitEquipmentHook(struct Player *);
void Player_InitVitalsAndCollisionFromScript(struct Player *);
void Player_InitEffects(struct Player *);
void Player_RegisterSceneMessageReceiver(struct Player *);
void Player_InitEventProcedureFromScript(struct Player *);
void Player_SaveStateSnapshot(struct Player *);
void SceneHud_RefreshControls(void);
void *PlayerBarrier_Create(s32, s32);
void PlayerAttackEffect_Create(void *, void *, void *);
static inline void StepPlayback(struct ContactSwarmSprite *sprite,
                                struct DisplaySequenceState *state)
{
    PlayerCue_StepSequence(sprite, state);
}
s32 Player_InitializeFromScript(struct Player *p, s32 slot)
{
    u8 *data;
    void **resource;
    struct ContactSwarmSprite *sprite;
    struct DisplaySequenceState *state;
    p->statModifierContext.statBOverrideEnabled = 255;
    TimedEffect_GetOrCreateManager(12);
    p->actionDisplay = ActorDebris_Create(&p->animation, &p->heading, &p->side);
    data = (u8 *)p + 0x1FC;
    AnimData_Load(data, 0x97D3);
    *(void **)((u8 *)p + 0x22C) = ScriptProc_Start(0x922E, 0xC6A0);
    resource = (void **)((u8 *)p + 0x22C);
    sprite = (struct ContactSwarmSprite *)p->auxiliarySprite;
    Sprite_Init(sprite, data, 1);
    state = (struct DisplaySequenceState *)((u8 *)p + 0x218);
    Anim_SetSequence(state, *resource, 1, 0, 0);
    StepPlayback(sprite, state);
    Player_InitSlotFromScript(p);
    Player_InitActionMode(p);
    Player_InitBodyFromScript(p, slot);
    sub_08140250(p);
    Player_InitActionRequest(p);
    Player_RefreshActionEquipment(p);
    sub_08140244(p);
    Player_InitEquipmentHook(p);
    Player_InitVitalsAndCollisionFromScript(p);
    Player_InitEffects(p);
    Player_RegisterSceneMessageReceiver(p);
    Player_InitEventProcedureFromScript(p);
    gPlayers[p->movementRequestBlocked] = p;
    gPlayerCount++;
    if (!p->movementRequestBlocked) {
        Player_SaveStateSnapshot(p);
        SceneHud_RefreshControls();
    }
    p->actionRequestEffect = PlayerBarrier_Create(slot + 1, 0);
    PlayerAttackEffect_Create(&p->animation, &p->facing, &p->side);
    *(u32 *)p->unkA88 = 0;
    return 0;
}
