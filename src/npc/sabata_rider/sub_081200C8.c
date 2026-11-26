#include "global.h"

/* tentative: parses n/P/s/p script keywords into actor fields */

struct ScriptPosition {
    s32 x, y, z, unused;
};
/* gParticipantActors is the player; +0x18 is its motion state (x at +0x28),
 * +0x5F4 its viewport origin X. */
struct MotionState {
    u8 unknown0[0x10];
    s32 x;
};
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
extern u8 *gParticipantActors;

static inline void Actor_SetScreenPosition(u8 *self, struct ScriptPosition *pos)
{
    u8 *player;
    struct MotionState *state;
    player = gParticipantActors;
    state = (struct MotionState *)(player + 0x18);
    *(s32 *)(self + 0x14) = pos->y << 12;
    *(s32 *)(self + 0x18) = pos->z << 12;
    if (state == NULL) {
        *(s32 *)(self + 0x5DC) = (s32)state;
        *(s32 *)(self + 0x10) = pos->x << 12;
    } else {
        s32 *offset = (s32 *)(self + 0x5DC);
        s32 d = pos->x << 12;
        s32 *viewX = (s32 *)(player + 0x5F4);
        d -= state->x + *viewX;
        *offset = d;
        *(s32 *)(self + 0x10) = state->x + *viewX + d;
        *(s32 *)(self + 0x5BC) = *offset;
        *(s32 *)(self + 0x5C4) = *(s32 *)(self + 0x18);
    }
}

void ActorScript_ParseParams(u8 *self)
{
    struct ScriptPosition position;
    *(s32 *)(self + 0x8BC) = Script_GetKeywordValue('n', 0);
    if (Script_SeekToKeyword('P')) {
        s32 a, b, c, d, e;
        a = Script_GetPc() ? Script_GetValue() + 12 : 12;
        b = Script_GetPc() ? Script_GetValue() + 18 : 18;
        c = Script_GetPc() ? Script_GetValue() + 6 : 6;
        d = Script_GetPc() ? Script_GetValue() : 0;
        e = Script_GetPc() ? Script_GetValue() : 0;
        self[0x4B] = e;
        self[0x48] = a;
        self[0x4A] = b;
        self[0x49] = c;
        self[0x4C] = d;
    }
    if (Script_SeekToKeyword('s')) {
        s32 a, b;
        a = Script_GetPc() ? Script_GetValue() : 1000;
        b = Script_GetPc() ? Script_GetValue() : 100;
        *(s16 *)(self + 0x58) = a;
        *(s16 *)(self + 0x5A) = a;
        *(s32 *)(self + 0x5C) = b;
    }
    if (Script_SeekToKeyword('p')) {
        {
            struct ScriptPosition *dst = &position;
            dst->x = Script_GetPc() ? Script_GetValue() : 0;
        }
        {
            struct ScriptPosition *dst = &position;
            dst->y = Script_GetPc() ? Script_GetValue() : 0;
        }
        {
            struct ScriptPosition *dst = &position;
            dst->z = Script_GetPc() ? Script_GetValue() : 0;
        }
        Actor_SetScreenPosition(self, &position);
    }
}
