/* C counterparts of the SDK's bx r0..r3 interworking trampolines. These adapt
 * ARM/Thumb calls on the GBA; the portable build calls the C function pointer. */
int sub_08249238(int (*function)(void)) { return function(); }
int Callback_InvokeActor(void *argument, int (*function)(void *))
{
    return function(argument);
}
int sub_08249240(void *first, void *second, int (*function)(void *, void *))
{
    return function(first, second);
}
int sub_08249244(void *first, void *second, void *third,
                 int (*function)(void *, void *, void *))
{
    return function(first, second, third);
}
