#include "global.h"
s32 Div(s32,s32);s32 sub_08228548(s32,s32);
static inline s32 yearDays(s32 year)
{
 s32 a=Div(year,4),b=Div(year,100);
 return 365*year+a-b+Div(year,400);
}
s32 sub_082285C4(s32 year,s32 month,s32 day,s32 otherYear,s32 otherMonth,s32 otherDay)
{
 s32 total=yearDays(year);
 s32 otherTotal,i;
 i=1;
 while(i<month) {total+=sub_08228548(year,i);i++;}
 total+=day;
 otherTotal=yearDays(otherYear);
 i=1;
 while(i<otherMonth) {otherTotal+=sub_08228548(otherYear,i);i++;}
 otherTotal+=otherDay;
 return total-otherTotal;
}
