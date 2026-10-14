// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
extern u8 *gUnk_02000488;
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_BeginDirectedState(struct ContactPosition *position)
{
    struct TetherActor *actor = (struct TetherActor *)gUnk_02000488;
    if (actor) {
        struct TetherPlayer *player;
        struct TetherActor *savedActor;
        s32 dx;
        s32 dz;
        actor->directedTarget = *position;
        TetherActor_SetState(actor, 15);
        savedActor = (struct TetherActor *)gUnk_02000488;
        savedActor->directedStep = 16;
        player = gUnk_02000580[0];
        {
            s32 playerX = player->position.x;
            struct ContactPosition *targetPosition = &savedActor->directedTarget;
            s32 actorX = targetPosition->x;
            dx = playerX - actorX;
        }
        dz = player->position.z - savedActor->directedTarget.z;
        if (dx || dz) {
            register s32 angle asm("r0") = Math_Atan2(dx, dz);
            struct TetherActor *target;
            angle = (angle + 16) & 255;
            asm("" : "+r"(angle));
            angle >>= 5;
            target = (struct TetherActor *)gUnk_02000488;
            angle = (angle + 7) & 7;
            {
                register u32 offset asm("r3") = 0x46C;
                *(u16 *)((u8 *)target + offset) = angle;
            }
        } else {
            u32 index = (gUnk_03005308 + 1) & 1023;
            gUnk_03005308 = index;
            {
                u32 byteOffset = index << 1;
                const u16 *table;
                register u32 sample asm("r0");
                register u16 *heading asm("r1");
                table = (const u16 *)RANDOM_TABLE_ADDR;
                sample = *(const u16 *)(byteOffset + (u32)table);
                sample &= 7;
                {
                    register u32 offset asm("r2") = 0x46C;
                    asm("" : "+r"(offset));
                    heading = (u16 *)((u8 *)savedActor + offset);
                }
                *heading = sample;
            }
        }
        {
            register u8 *target asm("r0") = gUnk_02000488;
            register u32 offset asm("r3");
            offset = 0x46A;
            asm("" : "+r"(offset));
            *(u16 *)(target + offset) = 29;
        }
    }
}
