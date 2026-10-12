#include "global.h"
#define GlyphTable_FindPair sub_08011578
int GlyphTable_FindPair(u8 *table,int first,int second){int i;for(i=0;i<64;i++,table+=2)if(table[0]==first && table[1]==second)return i;return -1;}
