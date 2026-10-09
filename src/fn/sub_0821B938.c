#include "global.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 sub_0821B938(u32 operation, s32 left, s32 right)
{
    switch (operation) {
    case 1: return -right;
    case 2: return !right;
    case 3: return ~right;
    case 4: return left + right;
    case 5: return left - right;
    case 6: return left * right;
    case 7: return Div(left, right);
    case 8: return Mod(left, right);
    case 9: left <<= right; return left;
    case 10: left = (u32)left >> right; return left;
    case 11: return left == right;
    case 12: return left != right;
    case 13: return left < right;
    case 14: return left <= right;
    case 15: return left > right;
    case 16: return left >= right;
    case 17: left |= right; return left;
    case 18: left &= right; return left;
    case 19: left ^= right; return left;
    case 20: return left || right;
    case 21: return left && right;
    case 23: return right;
    default: return 0;
    }
}
