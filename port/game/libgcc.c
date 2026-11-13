/* C counterparts of handwritten assembly helpers (symbols/progress_asm.csv):
 * GCC lib1funcs division, the MP2K umul3232H32 veneer and the RFU bx-register
 * trampolines. Division by zero follows the GBA build (__div0 returns). */
#include <stdint.h>

int32_t __divsi3(int32_t a, int32_t b)
{
    if (b == 0) return 0;
    if (b == -1) return (int32_t)(0u - (uint32_t)a);
    return a / b;
}
int32_t sub_08249350(int32_t a, int32_t b) /* __modsi3 */
{
    if (b == 0 || b == -1) return b == 0 ? a : 0;
    return a % b;
}
uint32_t sub_08249420(uint32_t a, uint32_t b) { return b ? a / b : 0; }  /* __udivsi3 */
uint32_t sub_08249498(uint32_t a, uint32_t b) { return b ? a % b : a; }  /* __umodsi3 */

uint32_t umul3232H32(uint32_t a, uint32_t b) { return (uint32_t)(((uint64_t)a * b) >> 32); }

/* librfu Callback_Dummy_M/S/ID: bx r2, bx r1, bx r0. */
void sub_0824538C(int command, int error, void (*callback)(int, int)) { callback(command, error); }
void sub_08245390(uint16_t command, void (*callback)(uint16_t)) { callback(command); }
void sub_08245394(void (*callback)(void)) { callback(); }
