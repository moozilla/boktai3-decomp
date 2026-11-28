#include "monsters/muspell_head.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
void TrapSpellParticles_CreateDefaultBurst(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                           u32);
void MuspellHead_RecordDefeat(struct MuspellHead *);
u32 MuspellHead_UpdateDefeatEffect(struct MuspellHead *head)
{
    if (head->initialDead == 0) {
        if (head->age <= 35) {
            struct ContactPosition position;
            u16 *y = (u16 *)&position.y;
            u32 angle;

            position = head->sprite.position;
            *y += 200;
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            angle = *(u8 *)&gRandomTable[gRandomIndex];
            TrapSpellParticles_CreateDefaultBurst(&position, 0x80, 0x20, 12, 12, 24, angle, 0x20,
                                                  0x20, 1, 2, 0x200);
        }
        if (head->age == 1)
            MuspellHead_RecordDefeat(head);
    }
    return 0;
}
