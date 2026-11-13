/* Thumb-to-IWRAM ARM call veneers become ordinary C calls in the source port. */
void sub_08000E90(void);
void sub_08001038(void);
void sub_08001120(void);
void sub_080012E0(void);
void sub_08219624(void) { sub_08000E90(); }
void sub_0821964C(void) { sub_08001038(); }
void sub_08219674(void) { sub_08001120(); }
void sub_0821969C(void) { sub_080012E0(); }
