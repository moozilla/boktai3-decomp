#include "global.h"
#include "engine.h"
void ResourceRegistry_RegisterResource(u32, void *, u32);

// confidence=low: loads the animation resources of the scene object manager and allocates its slot pool.
s32 SceneObjects_Init(u8 *p, u16 arg, s32 unused)
{
    s32 flag;
    s32 i;
    u8 *q;
    u32 zero = 0;
    *(u16 *)(p + 0x18) = arg;
    *(u16 *)(p + 0x2e) = zero;
    *(u16 *)(p + 0x1a) = zero;
    flag = Script_GetKeywordValue(0x65, 1);
    *(s32 *)(p + 0x34) = flag;
    if (flag) {
        u8 **t0, **t1, **t2, **t3, **t4, **t5, **t6;
        *(t0 = (u8 **)(p + 0x410)) = Heap_Alloc(0x8c);
        *(t1 = (u8 **)(p + 0x414)) = Heap_Alloc(0x8c);
        *(t2 = (u8 **)(p + 0x418)) = Heap_Alloc(0x8c);
        *(t3 = (u8 **)(p + 0x41c)) = Heap_Alloc(0x8c);
        *(t4 = (u8 **)(p + 0x420)) = Heap_Alloc(0x8c);
        *(t5 = (u8 **)(p + 0x424)) = Heap_Alloc(0x8c);
        *(t6 = (u8 **)(p + 0x428)) = Heap_Alloc(0x8c);
        if (!*t0 || !*t1 || !*t2 || !*t3 || !*t4 || !*t5 || !*t6) {
            Task_Kill(p);
            return -1;
        }
        Memory_Clear(*t0, 0x8c);
        Memory_Clear(*t1, 0x8c);
        Memory_Clear(*t3, 0x8c);
        Memory_Clear(*t4, 0x8c);
        Memory_Clear(*t5, 0x8c);
        Memory_Clear(*t6, 0x8c);
    } else {
        *(s32 *)(p + 0x410) = flag;
        *(s32 *)(p + 0x414) = flag;
        *(s32 *)(p + 0x418) = flag;
        *(s32 *)(p + 0x41c) = flag;
        *(s32 *)(p + 0x420) = flag;
        *(s32 *)(p + 0x424) = flag;
        *(s32 *)(p + 0x428) = flag;
    }
    if (Script_SeekToKeyword(0x64)) {
        i = 0;
        q = p + 0x3c;
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7ACD;
            if (!AnimData_Load(q, id))
                return -1;
            q += 0x1c;
        }
    } else {
        i = 0;
        q = p + 0x3c;
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7ACD))
                return -1;
            q += 0x1c;
        }
    }
    if (Script_SeekToKeyword(0x6b)) {
        u8 *b;
        u32 off;
        i = 0;
        b = p + 0x154;
        off = 0;
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AD3;
            if (!AnimData_Load(b, id))
                return -1;
            if (*(s32 *)(p + 0x34)) {
                AnimData_Load(*(u8 **)(p + 0x410) + off, id);
                AnimData_Load(*(u8 **)(p + 0x414) + off, id);
                AnimData_Load(*(u8 **)(p + 0x418) + off, id);
                AnimData_Load(*(u8 **)(p + 0x41c) + off, id);
                AnimData_Load(*(u8 **)(p + 0x420) + off, id);
                AnimData_Load(*(u8 **)(p + 0x424) + off, id);
                AnimData_Load(*(u8 **)(p + 0x428) + off, id);
            }
            b += 0x1c;
            off += 0x1c;
        }
    } else {
        i = 0;
        q = p + 0x154;
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AD3))
                return -1;
            q += 0x1c;
        }
    }
    if (Script_SeekToKeyword(0x53)) {
        u32 id;
        if (Script_GetPc())
            id = (u16)Script_GetValue();
        else
            id = 0x7AD2;
        i = 0;
        q = p + 0xc8;
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, id))
                return -1;
            q += 0x1c;
        }
    } else {
        i = 0;
        q = p + 0xc8;
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AD2))
                return -1;
            q += 0x1c;
        }
    }
    if (Script_SeekToKeyword(0x62)) {
        u8 *b;
        u8 *c;
        i = 0;
        b = p + 0x26c;
        c = p + 0x1e0;
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AEE;
            if (!AnimData_Load(c, id))
                return -1;
            if (!AnimData_Load(b, id))
                return -1;
            b += 0x1c;
            c += 0x1c;
        }
    } else {
        u8 *b;
        u8 *c;
        i = 0;
        b = p + 0x26c;
        c = p + 0x1e0;
        for (; i <= 4; i++) {
            if (!AnimData_Load(c, 0x7AD3))
                return -1;
            if (!AnimData_Load(b, 0x7AD3))
                return -1;
            b += 0x1c;
            c += 0x1c;
        }
    }
    if (Script_SeekToKeyword(0x73)) {
        i = 0;
        q = p + 0x2f8;
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AF3;
            if (!AnimData_Load(q, id))
                return -1;
            q += 0x1c;
        }
    } else {
        i = 0;
        q = p + 0x2f8;
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AF3))
                return -1;
            q += 0x1c;
        }
    }
    if (Script_SeekToKeyword(0x75)) {
        i = 0;
        q = p + 0x384;
        for (; i <= 4; i++) {
            u32 id;
            if (Script_GetPc())
                id = (u16)Script_GetValue();
            else
                id = 0x7AF3;
            if (!AnimData_Load(q, id))
                return -1;
            q += 0x1c;
        }
    } else {
        i = 0;
        q = p + 0x384;
        for (; i <= 4; i++) {
            if (!AnimData_Load(q, 0x7AF3))
                return -1;
            q += 0x1c;
        }
    }
    {
        s32 count = Script_GetKeywordValue(0x6d, 0x10);
        void *buf;
        s32 sz;
        *(s32 *)(p + 0x30) = count;
        sz = 0x8c;
        buf = Heap_Alloc(count * sz);
        *(void **)(p + 0x42c) = buf;
        Memory_Clear(buf, *(s32 *)(p + 0x30) * sz);
    }
    {
        s32 end = (s32)(p + 0x1e);
        u16 z = 0;
        s32 h = (s32)(p + 0x24);
        do {
            *(u16 *)h = z;
            h -= 2;
        } while (h >= end);
    }
    ResourceRegistry_RegisterResource(0x85E6, p, 0);
    return 0;
}
