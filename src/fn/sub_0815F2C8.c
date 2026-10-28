// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "contact_swarm.h"
#include "ram.h"
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
    const volatile u16 *command;
    u32 duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r4");
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                } else {
                    state->index = index - 1;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                } else {
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}
s32 Player_InitializeFromScript(struct Player *p, s32 slot)
{
    u8 *data;
    void **resource;
    register struct ContactSwarmSprite *sprite asm("r6");
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
