#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Position {
    s16 x, y, z;
};
struct Trigger {
    s16 id, kind;
    s8 actor;
    u8 type;
    u8 pad06[9];
    u8 shake;
    s16 baseX, baseZ;
    u8 pad14[24];
    struct Position *position;
};
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
#define Trigger_UpdateShake sub_08007060
void Trigger_UpdateShake(struct Trigger *p)
{
    if (p->shake) {
        p->shake--;
        if (!p->shake) {
            p->position->x = p->baseX;
            p->position->z = p->baseZ;
        } else {
            register const u16 *table asm("r6") = gUnk_0203B400;
            register u32 random asm("r0");
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            random = table[gUnk_03005308];
            {
                struct Position *position = p->position;
                random &= 15;
                asm("" : : "r"(random));
                position->x = random + p->baseX - 4;
            }
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            random = table[gUnk_03005308];
            {
                struct Position *position = p->position;
                random &= 7;
                position->z = random + ({
                                  register u32 base asm("r7") = (u16)p->baseZ;
                                  base;
                              }) -
                              4;
            }
        }
    }
}
