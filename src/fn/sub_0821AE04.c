#include "global.h"
struct ScriptContext { u8 *table; u32 count; u8 *body; u8 *tail; };
extern struct ScriptContext gUnk_02000438;
extern u8 *gUnk_02000448[4];
extern u32 gUnk_0200060C;
u8 *Script_ReadProcTable(u8 *, u32 *);
static inline u32 read_word(u8 *p)
{
    return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
}
s32 sub_0821AE04(u8 *p)
{
    u32 count;
    struct ScriptContext *ctx = &gUnk_02000438;
    u8 *base;
    gUnk_0200060C = read_word(p);
    p += 4;
    base = Script_ReadProcTable(p, &count);
    ctx->table = p;
    ctx->count = count;
    gUnk_02000448[0] = base;
    gUnk_02000448[1] = base + read_word(base + 4);
    gUnk_02000448[2] = base + read_word(base + 8);
    gUnk_02000448[3] = base + read_word(base + 12);
    base += read_word(base);
    ctx->body = base + 4;
    {
        u32 size = read_word(base);
        size += (u32)base;
        size += 8;
        ctx->tail = (u8 *)size;
    }
    return 0;
}
