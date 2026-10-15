#include "burst_emitter.h"
#define BurstEmitter_CreateFromScript sub_08069DB8
static inline s32 BurstEmitter_GetScriptValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline s32 BurstEmitter_GetScriptShort(void)
{
    if (Script_GetPc())
        return (s16)Script_GetValue();
    return 0;
}
void BurstEmitter_CreateFromScript(void)
{
    struct BurstEmitterPool *pool = gUnk_02000160;
    struct BurstEmitter *emitter;
    s32 a, b, c, d;
    if (!pool) {
        pool = sub_08069F4C();
        if (!pool)
            return;
    }
    emitter = sub_08069D0C(pool);
    if (!emitter)
        return;
    if (Script_SeekToKeyword('p')) {
        emitter->position.x = BurstEmitter_GetScriptValue();
        {
            s32 y = BurstEmitter_GetScriptValue();
            emitter->position.y = y;
        }
        emitter->position.z = BurstEmitter_GetScriptValue();
    }
    if (Script_SeekToKeyword('P')) {
        a = BurstEmitter_GetScriptShort();
        b = BurstEmitter_GetScriptShort();
        c = BurstEmitter_GetScriptShort();
        d = BurstEmitter_GetScriptShort();
    } else {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
    }
    emitter->context = Script_GetKeywordValue('c', 0);
    {
        u8 *state = &emitter->state;
        u32 zero;
        zero = 0;
        *state = 1;
        emitter->frame = zero;
        emitter->mode = 2;
    }
    sub_08069BC8(emitter, a, b, c, d);
}
