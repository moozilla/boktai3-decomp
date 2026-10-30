// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "engine.h"
void SceneActorReaction_RestoreAfterTurns(struct SceneActorRegistryResetView *input, s32 inputFrame)
{
    struct SceneActorRegistryResetView *actor;
    register s32 frame asm("r4");
    u8 *group;
    register s8 *remaining asm("r5");
    register u32 countByte asm("r1");
    actor = input;
    frame = inputFrame;
    group = (u8 *)actor + 0x3D4;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        {
            register u32 mask asm("r0") = 0x400;
            register u32 offset asm("r3") = 0x2D4;
            register u16 *flags asm("r2");
            asm("" : : "r"(offset));
            flags = (u16 *)((u8 *)actor + offset);
            *flags = mask | *flags;
        }
        actor->field2C2 = 32;
    }
    remaining = (s8 *)(group + 0x206);
    countByte = *(u8 *)remaining;
    if (*remaining > 0) {
        if (frame > 60) {
            u32 state = 3;
            register u32 one asm("r2") = 1;
            register u32 zero asm("r3") = 0x2A8;
            register u8 *field asm("r0");
            u32 behaviorOffset;
            u32 encoded;
            register u8 *encodedOutput asm("r1");
            asm("" : : "r"(zero));
            field = (u8 *)actor + zero;
            zero = 0;
            *field = state;
            /* Reuse the expired frame operand for phase addresses and the saved control word. */
            frame = 0x2A9;
            field = (u8 *)actor + frame;
            *field = one;
            behaviorOffset = 0x2AA;
            field = (u8 *)actor + behaviorOffset;
            *field = zero;
            {
                u32 offset = 0x2AC;
                u32 *counter;
                counter = (u32 *)((u8 *)actor + offset);
                *counter = zero;
            }
            frame += 8;
            field = (u8 *)actor + frame;
            *field = one;
            behaviorOffset -= 0x86;
            {
                u32 *control = (u32 *)((u8 *)actor + behaviorOffset);
                frame = *control;
            }
            {
                u32 kind = 2;
                behaviorOffset += 0x8E;
                field = (u8 *)actor + behaviorOffset;
                *field = one;
                one = 0x2B0;
                *((u8 *)actor + one) = zero;
                behaviorOffset += 3;
                field = (u8 *)actor + behaviorOffset;
                *field = kind;
                {
                    register u32 offset asm("r1") = 0x21C;
                    register u32 *output asm("r0");
                    asm("" : : "r"(offset));
                    output = (u32 *)((u8 *)actor + offset);
                    *output = frame;
                }
                one += 0x18;
                {
                    register u16 *output asm("r0") = (u16 *)((u8 *)actor + one);
                    *output = zero;
                }
            }
            frame = 0x114;
            asm("" : "+r"(frame));
            {
                register u32 *flags asm("r2") = (u32 *)((u8 *)actor + frame);
                u32 mask;
                mask = ~1;
                *flags &= mask;
            }
            behaviorOffset = 0x104;
            asm("" : : "r"(behaviorOffset));
            *(u32 *)((u8 *)actor + behaviorOffset) = zero;
            if (*remaining == 1) {
                register u32 value asm("r0") = ((*(u8 *)(group + 0x205) >> 5) + 3) & 7;
                asm("" : "+r"(value));
                encoded = value;
                encodedOutput = (u8 *)actor + 0x3CD;
            } else {
                s32 prior = actor->body.direction;
                s32 tries = 0;
                const u16 *table = gRandomTable;
                u32 *randomIndex = &gRandomIndex;
                u32 wrap = 1023;
                u32 mask = 3;
                register s32 choice asm("r0");
                asm("" : "+r"(prior), "+r"(tries));
                asm("" : "+r"(table), "+r"(randomIndex));
                for (;;) {
                    u32 index = (*randomIndex + 1) & wrap;
                    *randomIndex = index;
                    choice = (table[index] & mask) << 6;
                    if (choice != prior)
                        break;
                    if (++tries > 3) {
                        choice = prior;
                        break;
                    }
                }
                choice = ((choice >> 5) + 3) & 7;
                asm("" : "+r"(choice));
                encoded = choice;
                {
                    register u32 offset asm("r3") = 0x3CD;
                    asm("" : : "r"(offset));
                    encodedOutput = (u8 *)actor + offset;
                }
            }
            *encodedOutput = encoded;
            {
                u8 *next = group + 0x206;
                *next -= 1;
            }
        }
    } else {
        u32 mask = 255;
        *(u8 *)remaining = mask | countByte;
        if (frame > 30) {
            u8 *packet = group + 0x8C;
            u32 kind = packet[7];
            void *control;
            control = *(void **)packet;
            {
                u8 *flag = &actor->field2B2;
                u32 readyOne;
                frame = 0;
                asm("" : "+r"(frame));
                readyOne = 1;
                *flag = readyOne;
                actor->field2B0 = frame;
                actor->field2B5 = kind;
                actor->target = control;
                actor->field2C8 = frame;
                {
                    u32 *flags = &actor->flags114;
                    u32 mask;
                    mask = ~1;
                    *flags &= mask;
                }
                actor->word104 = frame;

                {
                    u32 state = packet[4];
                    u32 phase = packet[5];
                    u32 behavior = packet[6];
                    actor->state2A8 = state;
                    actor->phase2A9 = phase;
                    actor->behavior2AA = behavior;
                    behavior = 0x2AC;
                    asm("" : : "r"(behavior));
                    {
                        u32 *counter = (u32 *)((u8 *)actor + behavior);
                        *counter = frame;
                    }
                    {
                        u32 offset = 0x2B1;
                        u8 *ready;
                        ready = (u8 *)actor + offset;
                        *ready = readyOne;
                    }
                    {
                        u16 *flags = &actor->flags2D4;
                        u32 mask = ~0x400;
                        u32 value;
                        asm("" : "+r"(mask) : : "r1");
                        value = *flags;
                        *flags = mask & value;
                    }
                }
            }
        }
    }
}
