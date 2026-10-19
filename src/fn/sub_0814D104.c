#include "global.h"
#include "player.h"
#include "scene_object.h"
#include "game_state.h"
#ifdef NONMATCHING
// fuzzy: 83.1%
// foundry draft (worker best 82.4%); what differs is mostly register allocation and instruction order
#define Player_ComputeActionScale sub_0814D104
s32 Div(s32, s32);
u32 SunClock_GetPhase(void);
static inline u32 Test(u32 *p, u32 mask) { return *p & mask; }
s32 sub_0814D104(struct Player *p, struct SceneCollider *contact)
{
    s32 scale = 64;
    if (p && Test(&p->actionFlags, 0x8000000)) scale = Div(p->derivedStats[1], 10) + 64;
    if (contact) {
        if (p && Test(&p->actionFlags, 0x10000000) && p->animationIndex == 0x81) scale += 13;
        if (p && Test(&p->actionFlags, 0x20000000) && p->animationIndex == 0x72) scale += 13;
        if (p && Test(&p->actionFlags, 0x40000000) && (contact->contactFlags & 7)) scale += 13;
    }
    if (p && Test(&p->actionStateFlags, 2) && gUnk_02000710->unk876 > 0)
        scale += gUnk_02000710->unk876;
    switch (SunClock_GetPhase()) {
    case 1: case 2: case 3:
        if (p && (s32)p->actionFlags < 0) scale += 13;
        if (p && Test(&p->actionStateFlags, 4)) scale -= 13;
        break;
    case 0: case 4: case 5:
        if (p && Test(&p->actionStateFlags, 1)) scale += 13;
        if (p && Test(&p->actionStateFlags, 8)) scale -= 13;
        break;
    }
    if (scale < 0) scale = 1;
    return scale;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0814D104);
#endif
