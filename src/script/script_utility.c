#include "script/script_utility.h"
#include "script/script_virtual_machine.h"
#include "link/wireless/random.h"

s32 ScriptUtility_WriteValuesToReference(void)
{
    struct ScriptUtilityReference reference;
    s32 index;
    Script_ReadReference(&reference);
    for (index = 0; Script_GetPc() != 0; index++)
        Script_WriteReferenceValue(&reference, index, Script_GetValue());
    return 0;
}

s32 ScriptUtility_CallRepeatedFromScript(void)
{
    struct ScriptUtilityArguments args;
    s32 count = Script_GetKeywordValue('a', 0);
    s32 repeats = Script_GetKeywordValue('r', 0);
    s32 values[count];
    s32 i, j;
    args.count = count;
    args.values = values;
    if (Script_SeekToKeyword('e')) {
        u32 script = Script_GetValue();
        if (Script_SeekToKeyword('d')) {
            u8 *pc = Script_GetPc();
            for (i = 0; i < repeats; i++) {
                for (j = 0; j < count; j++) {
                    values[j] = Script_ReadValue(pc);
                    pc = Script_GetPc();
                }
                Script_RunBlock(script, &args);
            }
        } else {
            for (i = 0; i < repeats; i++)
                Script_RunBlock(script, &args);
        }
    }
    return 0;
}

s32 ScriptUtility_GetRandom(void)
{
    if (Script_SeekToKeyword('p')) {
        s32 bound = Script_GetValue();
        if (bound > 0) {
            s32 sample = RANDOM_NEXT();
            return Mod(sample >> 3, bound);
        }
    }
    return RANDOM_NEXT();
}

u32 ScriptUtility_GetKeys(void)
{
    if (Script_SeekToKeyword('p'))
        return gPadState[1];
    if (Script_SeekToKeyword('s'))
        return gPadState[0];
    return Script_SeekToKeyword('r') == 0 ? gPadState[1] : gPadState[2];
}

s32 ScriptUtility_ReadIndexed(void)
{
    u8 *base;
    s32 index, size;
    if (!Script_SeekToKeyword('r'))
        return -1;
    base = (u8 *)Script_GetValueSafe();
    if (!Script_SeekToKeyword('i'))
        return -1;
    index = Script_GetValue();
    if (!Script_SeekToKeyword('s'))
        return -1;
    size = Script_GetValue();
    if (!size)
        return base[index];
    return ((s16 *)base)[size != 1 ? index * 2 : index];
}

void Sound_SuspendPlayer30Song(void);

void ScriptUtility_SetControlFlag(s32 stopSong)
{
    gEngineFlags |= 0x200;
    if (stopSong && !(gEnginePauseFlags & 8)) {
        Sound_SuspendPlayer30Song();
        gEnginePauseFlags |= 8;
    }
}

void ScriptUtility_SetControlFlagFromScript(void)
{
    s32 value;
    if (Script_SeekToKeyword('p'))
        value = Script_GetValue();
    else
        value = 0;
    ScriptUtility_SetControlFlag(value);
}

void Sound_RestartPlayer30Song(void);

void ScriptUtility_ClearControlFlag(void)
{
    gEngineFlags &= ~0x200;
    if (gEnginePauseFlags & 8) {
        Sound_RestartPlayer30Song();
        {
            u32 mask = ~8;
            gEnginePauseFlags &= mask;
        }
    }
}

void ScriptUtility_ClearControlFlagFromScript(void)
{
    ScriptUtility_ClearControlFlag();
}

u32 ScriptUtility_IsControlFlagSet(void)
{
    if (gEngineFlags & 0x200)
        return TRUE;
    return FALSE;
}

/* ScriptUtility_SetPauseLatch: Sets bit 0x800 in the shared control flags; pairs with ScriptUtility_WritePauseLatchToReference. */

u32 ScriptUtility_SetPauseLatch(void)
{
    gEngineFlags |= 0x800;
#ifdef UBFIX
    // falls off the end: the result is whatever the caller left in r0
    return 0;
#endif
}

void ScriptUtility_WritePauseLatchToReference(void)
{
    struct ScriptUtilityReference reference;
    Script_ReadReference(&reference);
    if (gEngineFlags & 0x800)
        Script_WriteReferenceValue(&reference, 0, 1);
    else
        Script_WriteReferenceValue(&reference, 0, 0);
}
