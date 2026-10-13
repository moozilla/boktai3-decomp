#include "global.h"
#include "background_controller.h"

s32 Background_LoadTilesFromScript(void)
{
    u16 entries[64];
    struct BackgroundResource resource;
    u16 layer = sub_0821ABA8('t', 0);
    u16 data = sub_0821ABA8('d', 0);
    u16 size = sub_0821ABA8('s', 0);
    void *base = sub_0821A520(0xCEEF, data);
    s32 count = 0;
    if (Script_SeekToKeyword('c')) {
        s32 i;
        u16 *out;
        i = 0;
        out = entries;
        for (;; i++) {
            u8 *p;
            if (i > 63)
                goto done;
            p = Script_GetPc();
            if (!p)
                goto done;
            *out = sub_0821AA00(p);
            count++;
            out++;
        }
    } else {
        s32 zero = 0;
        u16 *out = entries + 63;
        do {
            *out = zero;
            out--;
        } while ((s32)out >= (s32)entries);
    }
done:
    if (Script_SeekToKeyword('m')) {
        struct BackgroundResource *loaded =
            (struct BackgroundResource *)sub_08216E54(Script_GetValue());
        resource = *loaded;
        size = resource.size;
    }
    sub_08215D40(layer, base, count, entries, size);
    return 0;
}
