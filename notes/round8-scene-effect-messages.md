# Scene effect queue messages

Tags: #effects #projection #matching #round8

0819946C, 08199578 and 08199684 are three 268-byte routines. They post the same three-halfword message with effect type 0, 1 or 3. All 804 bytes matched before the original inventory extent was split.

Grouping the screen projection and side selection in a statement macro changes when agbcc allocates the message pointer. The pointer lands in ip and the incoming value stays in r8 in all three routines. The common implementation uses the existing packed-position union.
