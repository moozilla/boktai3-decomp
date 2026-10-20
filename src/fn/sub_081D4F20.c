#include "global.h"

s32 BestiaryDisplay_GetEntryForActor(u32 kind, s32 variant)
{
    s32 result;
    switch (kind) {
        case 1:
            switch (variant) {
                case 0:
                    result = 23;
                    break;
                case 1:
                    result = 24;
                    break;
                case 2:
                    result = 25;
                    break;
                default:
                    goto missing;
            }
            break;
        case 2:
            switch (variant) {
                case 1:
                    result = 5;
                    break;
                case 0:
                case 2:
                    result = 4;
                    break;
                default:
                    goto missing;
            }
            break;
        case 3:
            switch (variant) {
                case 0:
                    result = 44;
                    break;
                case 1:
                    result = 45;
                    break;
                case 3:
                    result = 46;
                    break;
                default:
                    goto missing;
            }
            break;
        case 4:
            switch (variant) {
                case 1:
                    result = 20;
                    break;
                case 2:
                    result = 21;
                    break;
                case 3:
                    result = 22;
                    break;
                default:
                    goto missing;
            }
            break;
        case 5:
            switch (variant) {
                case 0:
                    result = 37;
                    break;
                case 5:
                    result = 43;
                    break;
                case 12:
                    result = 40;
                    break;
                default:
                    goto missing;
            }
            break;
        case 6:
            switch (variant) {
                case 0:
                    result = 38;
                    break;
                case 12:
                    result = 41;
                    break;
                default:
                    goto missing;
            }
            break;
        case 7:
            switch (variant) {
                case 0:
                    result = 39;
                    break;
                case 12:
                    result = 42;
                    break;
                default:
                    goto missing;
            }
            break;
        case 9:
            result = 13;
            break;
        case 10:
            result = 47;
            break;
        case 11:
            switch (variant) {
                case 0:
                    result = 48;
                    break;
                case 1:
                    result = 49;
                    break;
                default:
                    goto missing;
            }
            break;
        case 12:
        case 13:
            switch (variant) {
                case 0:
                    result = 50;
                    break;
                case 1:
                    result = 51;
                    break;
                default:
                    goto missing;
            }
            break;
        case 14:
            result = 19;
            break;
        case 17:
            switch (variant) {
                case 0:
                    result = 68;
                    break;
                case 1:
                    result = 69;
                    break;
                case 2:
                    result = 70;
                    break;
                default:
                    goto missing;
            }
            break;
        case 18:
            switch (variant) {
                case 1:
                    result = 10;
                    break;
                case 0:
                case 2:
                    result = 9;
                    break;
                default:
                    goto missing;
            }
            break;
        case 19:
            switch (variant) {
                case 1:
                    result = 12;
                    break;
                case 0:
                case 2:
                    result = 11;
                    break;
                default:
                    goto missing;
            }
            break;
        case 20:
            switch (variant) {
                case 0:
                    result = 6;
                    break;
                case 1:
                    result = 8;
                    break;
                case 2:
                    result = 7;
                    break;
                default:
                    goto missing;
            }
            break;
        case 21:
            switch (variant) {
                case 0:
                    result = 14;
                    break;
                case 1:
                    result = 16;
                    break;
                case 3:
                    result = 15;
                    break;
                case 2:
                    result = 17;
                    break;
                default:
                    goto missing;
            }
            break;
        case 22:
            switch (variant) {
                case 1:
                    result = 74;
                    break;
                case 2:
                    result = 73;
                    break;
                case 4:
                    result = 71;
                    break;
                case 5:
                    result = 72;
                    break;
                default:
                    goto missing;
            }
            break;
        case 23:
            switch (variant) {
                case 0:
                    result = 75;
                    break;
                case 1:
                    result = 76;
                    break;
                default:
                    goto missing;
            }
            break;
        case 24:
            result = 18;
            break;
        case 25:
            switch (variant) {
                case 0:
                    result = 26;
                    break;
                case 2:
                    result = 27;
                    break;
                default:
                    goto missing;
            }
            break;
        case 0:
        case 8:
        case 15:
        case 16:
        default:
        missing:
            result = -1;
            break;
    }
    return result;
}
