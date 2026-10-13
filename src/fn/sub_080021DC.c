#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "background_controller.h"

void Background_QueueCommand(u32 layer, u32 type, s32 length, s32 *args)
{
    u32 commandType = type;
    register s32 lengthLimit asm("r5") = length;
    register s32 *input asm("r12") = args;
    struct Background *p = gUnk_02000000[layer];
    if (p && p->queue.count <= 7) {
        s32 j;
        p->queue.types[p->queue.count] = commandType;
        j = 0;
        if (j < lengthLimit) {
            /* Retain the original count reload on each argument copy. The byte offset
             * is commands[count].args[j]; this operand order keeps agbcc's index order. */
            volatile u32 *count = &p->queue.count;
            struct BackgroundCommand *commands = p->queue.commands;
            s32 *source = input;
            do {
                *(s16 *)((u8 *)commands + ((j << 1) + (*count << 4))) = *source++;
                j++;
            } while (j < lengthLimit);
        }
        p->queue.count++;
    }
}
