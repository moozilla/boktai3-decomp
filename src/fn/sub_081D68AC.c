#include "global.h"

void MonsterBook_RemoveActiveAnimation(u8 *p);

s32 sub_081D68AC(u8 *p)
{
    s32 i;
    MonsterBook_RemoveActiveAnimation(p + 0xCC);
    MonsterBook_RemoveActiveAnimation(p + 0x6C);
    MonsterBook_RemoveActiveAnimation(p + 0xCC);
    MonsterBook_RemoveActiveAnimation(p + 0x12C);
    MonsterBook_RemoveActiveAnimation(p + 0x18C);
    MonsterBook_RemoveActiveAnimation(p + 0x1EC);
    MonsterBook_RemoveActiveAnimation(p + 0x24C);
    for (i = 0; i < 7; i++) {
        MonsterBook_RemoveActiveAnimation(p + 0x2AC + i * 0x60);
        MonsterBook_RemoveActiveAnimation(p + 0x54C + i * 0xC0);
        MonsterBook_RemoveActiveAnimation(p + 0x5AC + i * 0xC0);
    }
    for (i = 0; i < 2; i++) {
        MonsterBook_RemoveActiveAnimation(p + 0xA8C + i * 0x60);
        MonsterBook_RemoveActiveAnimation(p + 0xBAC + i * 0x60);
    }
    MonsterBook_RemoveActiveAnimation(p + 0xB4C);
    MonsterBook_RemoveActiveAnimation(p + 0xC6C);
    MonsterBook_RemoveActiveAnimation(p + 0xCCC);
    return 0;
}
