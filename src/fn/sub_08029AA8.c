#include "global.h"
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void GridAttackActor_SpawnGridContact(void *, int, int);
int sub_08249350(int, int);
static inline int randomColumn(u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return sub_08249350(table[gUnk_03005308], 3);
}
void GridAttackActor_SpawnGridPattern(u8 *p)
{
    if ((*(s16 **)(p + 0x110))[2] < 0xc00) {
        GridAttackActor_SpawnGridContact(p, 0, 0);
        GridAttackActor_SpawnGridContact(p, 2, 0);
        GridAttackActor_SpawnGridContact(p, 0, 1);
        GridAttackActor_SpawnGridContact(p, 1, 1);
        GridAttackActor_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gUnk_0203B400;
            GridAttackActor_SpawnGridContact(p, randomColumn(table), 2);
            GridAttackActor_SpawnGridContact(p, randomColumn(table), 3);
            GridAttackActor_SpawnGridContact(p, randomColumn(table), 4);
        }
    } else {
        GridAttackActor_SpawnGridContact(p, 0, 1);
        GridAttackActor_SpawnGridContact(p, 1, 1);
        GridAttackActor_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gUnk_0203B400;
            GridAttackActor_SpawnGridContact(p, randomColumn(table), 2);
            GridAttackActor_SpawnGridContact(p, randomColumn(table), 3);
            GridAttackActor_SpawnGridContact(p, randomColumn(table), 4);
        }
    }
}
