/* Solar bank panel layout inferred from static animation and amount accesses.
 * Balance/transfer interpretations remain tentative. */
struct SolarBankDigit {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0x54];
};
struct SolarBankPanel {
    u8 unk0[0x18];
    u8 labelData[0x20];
    u8 animData[0x20];
    struct SolarBankDigit label;
    struct SolarBankDigit prompt;
    struct SolarBankDigit controls[2];
    u8 unk1D8[0x60];
    struct SolarBankDigit balanceDigits[4];
    struct SolarBankDigit transferDigits[4];
    u8 unk538[0x1b0];
    void *layerMap;
    u8 unk6EC[8];
    s32 transferAmount;
    s32 balance;
    u8 unk6FC[0xe];
    u16 selection;
};
