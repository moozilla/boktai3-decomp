#include "password.h"
s32 sub_08011794(u8 *src, s32 tableRef, s32 script, u8 *output)
{
    struct PasswordEntry *e = Heap_Alloc(0x90);
    s32 result;
    if (!e)
        return -1;
    Memory_Clear(e, 0x90);
    e->table = Text_LookupString(Script_ParseStringRef((void *)tableRef));
    e->script = script;
    do {
        if (Text_CopyWithoutTags(e->text, src, 0x30) != 0x30) {
            result = -1;
            break;
        }
        result = Password_EncodeSymbols(e);
        if (result < 0)
            break;
        result = NameEntry_DecodeAndValidate(e);
        if (result < 0)
            break;
        Password_ApplyDecoded(e, output);
    } while (0);
    Heap_Free(e);
    return result;
}
