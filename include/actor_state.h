#ifndef FOUNDRY_ACTOR_STATE_H
#define FOUNDRY_ACTOR_STATE_H
/* Tentative actor state prefix shared by the adjacent state callbacks. */
struct ActorAnimationPair {
    u32 first, second;
};
struct ActorState {
    u8 unknown0[0x94];
    s16 vitality;
    u8 unknown96[4];
    u8 checking;
    u8 unknown9B;
    u32 flags;
    u8 unknownA0[0xA];
    u8 state;
    u8 unknownAB;
    u8 active, phase, variant, pending, triggered;
    u8 unknownB1[0x13];
    u32 timer;
    u8 unknownC8[0x3A9];
    u8 direction;
    u8 unknown472[6];
    struct ActorAnimationPair selectedAnimation;
    u8 unknown480[0x60];
    u8 animationState, unknown4E1, animationPhase, animationIndex, unknown4E4, mode, unknown4E6[2];
    u8 request, requestedVariant, requestMode, requestPhase;
    u8 unknown4EC[0x734];
    struct ActorAnimationPair animation;
};
/* The owner retains this actor prefix at offset zero. */
struct ActorStateOwner {
    struct ActorState actor;
};
#endif
