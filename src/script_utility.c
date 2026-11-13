#include "script_utility.h"
#include "script_virtual_machine.h"

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
    args.count = count;
    args.values = values;
    if (Script_SeekToKeyword('e')) {
        u32 script = Script_GetValue();
        if (Script_SeekToKeyword('d')) {
            u8 *pc = Script_GetPc();
            s32 current = 0;
            if (current < repeats)
                do {
                    s32 next;
                    s32 *destination;
                    current++;
                    next = current;
                    if (count > 0) {
                        destination = values;
                        current = count;
                        do {
                            *destination++ = Script_ReadValue(pc);
                            pc = Script_GetPc();
                        } while (--current != 0);
                    }
                    Script_RunBlock(script, &args);
                    current = next;
                } while (current < repeats);
        } else {
            if (repeats > 0) {
                s32 index = repeats;
                do {
                    Script_RunBlock(script, &args);
                } while (--index != 0);
            }
        }
    }
    return 0;
}

s32 ScriptUtility_GetRandom(void)
{
    if (Script_SeekToKeyword('p')) {
        s32 bound = Script_GetValue();
        if (bound > 0) {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                u32 offset = gRandomIndex * sizeof(gRandomTable[0]);
                s32 sample = *(u16 *)(offset + (u32)gRandomTable);
                return Mod(sample >> 3, bound);
            }
        }
    }
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * sizeof(gRandomTable[0]);
        return *(u16 *)(offset + (u32)gRandomTable);
    }
}

u32 ScriptUtility_GetKeys(void)
{
    if (Script_SeekToKeyword('p')) {
        u32 keys = gPadState[1];
        return keys;
    }
    if (Script_SeekToKeyword('s'))
        return gPadState[0];
    {
        u32 keys;
        if (Script_SeekToKeyword('r') == 0)
            keys = gPadState[1];
        else
            keys = gPadState[2];
        return keys;
    }
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
    {
        s32 offset;
        if (size != 1)
            offset = index * 4;
        else
            offset = index * 2;
        return *(s16 *)((u32)base + offset);
    }
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

/* ScriptUtility_SetFlag800: Sets bit 0x800 in the shared control flags and preserves the incoming return value; pairs with ScriptUtility_WriteFlag800ToReference. */

u32 ScriptUtility_SetFlag800(u32 result)
{
    u32 *flags;
    do {
        do {
            do {
                flags = &gEngineFlags;
            } while (0);
        } while (0);
    } while (0);
    *flags |= 0x800;
    return result;
}

void ScriptUtility_WriteFlag800ToReference(void)
{
    struct ScriptUtilityReference reference;
    Script_ReadReference(&reference);
    if (gEngineFlags & 0x800)
        Script_WriteReferenceValue(&reference, 0, 1);
    else
        Script_WriteReferenceValue(&reference, 0, 0);
}
