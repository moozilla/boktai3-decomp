#include "global.h"
void sub_08033778(int);void sub_082195E0(void*);
#define MessageMenu_Destroy sub_080113E0
int MessageMenu_Destroy(u8*m){int *handles=(int*)(m+0x444);int i=2;do{sub_08033778(*handles++);}while(--i>=0);sub_082195E0(m+0x478);return 0;}
