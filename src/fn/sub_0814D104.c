#include "global.h"
#include "player.h"
#include "scene_object.h"
#include "game_state.h"
s32 Div(s32, s32);
u32 SunClock_GetPhase(void);
static inline u32 Test(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 FlagAt(void *object, u32 offset, u32 mask)
{
    u8 *p = object;
    u32 *field;
    u32 bits;
    u32 fieldOffset;
    if (!p)
        return 0;
    fieldOffset = offset;
    field = (u32 *)(p + fieldOffset);
    bits = mask;
    return *field & bits;
}
static inline u32 ReadHalf(struct Player *p, u32 offset)
{
    u32 fieldOffset = offset;
    u16 *field;
    field = (u16 *)((u8 *)p + fieldOffset);
    return *field;
}
static inline s32 ReadBonus(void)
{
    u8 *global = (u8 *)gGameState;
    u32 offset;
    s16 *field;
    offset = 0x876;
    field = (s16 *)(global + offset);
    return *field;
}
static inline s32 SignedActionFlags(struct Player *p)
{
    if (!p)
        return 0;
    return p->actionFlags;
}
#define ActionFlag(p, mask) FlagAt(p, 0x448, mask)
#define StateFlag(p, mask)  FlagAt(p, 0x44C, mask)
s32 Player_ComputeActionScale(struct Player *p, struct SceneCollider *contact)
{
    s32 scale = 64;
    if (ActionFlag(p, 0x8000000))
        scale = Div(ReadHalf(p, 0x41E), 10) + 64;
    if (contact) {
        if (ActionFlag(p, 0x10000000) && ReadHalf(p, 0x3A0) == 0x81)
            scale += 13;
        if (ActionFlag(p, 0x20000000) && ReadHalf(p, 0x3A0) == 0x72)
            scale += 13;
        if (ActionFlag(p, 0x40000000) && (contact->contactFlags & 7))
            scale += 13;
    }
    if (StateFlag(p, 2)) {
        s32 bonus = ReadBonus();
        if (bonus > 0)
            scale += bonus;
    }
    switch (SunClock_GetPhase()) {
        case 1:
        case 2:
        case 3:
            if (SignedActionFlags(p) < 0)
                scale += 13;
            if (StateFlag(p, 4))
                scale -= 13;
            break;
        case 0:
        case 4:
        case 5:
            if (StateFlag(p, 1))
                scale += 13;
            if (StateFlag(p, 8))
                scale -= 13;
            break;
    }
    if (scale < 0)
        scale = 1;
    return scale;
}
