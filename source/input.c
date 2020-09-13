#include "input.h"

static inline u32 convertHidKeys(u32 keys)
{
    // Nothing to do yet
    return keys;
}

u32 waitInputWithTimeout(s32 msec)
{
    s32 n = 0;
    u32 keys;

    do
    {
        svcSleepThread(1 * 1000 * 1000LL);
        n++;

        hidScanInput();
        keys = convertHidKeys(hidKeysDown()) | (convertHidKeys(hidKeysDownRepeat()) & DIRECTIONAL_KEYS);
    } while (keys == 0 && (msec < 0 || n < msec));

    return keys;
}

u32 waitInput(void)
{
    return waitInputWithTimeout(-1);
}

static u32 scanHeldKeys(void)
{
    u32 keys;
    hidScanInput();
    keys = convertHidKeys(hidKeysHeld());
    return keys;
}

u32 waitComboWithTimeout(s32 msec)
{
    s32 n = 0;
    u32 keys = 0;
    u32 tempKeys = 0;

    // Wait for nothing to be pressed
    while (scanHeldKeys() != 0 && (msec < 0 || n < msec))
    {
        svcSleepThread(1 * 1000 * 1000LL);
        n++;
    }

    if (!(msec < 0 || n < msec))
        return 0;

    do
    {
        svcSleepThread(1 * 1000 * 1000LL);
        n++;

        tempKeys = scanHeldKeys();

        for (u32 i = 0x10000; i > 0; i--)
        {
            if (tempKeys != scanHeldKeys()) break;
            if (i == 1) keys = tempKeys;
        }
    }
    while((keys == 0 || scanHeldKeys() != 0) && (msec < 0 || n < msec));

    return keys;
}

u32 waitCombo(void)
{
    return waitComboWithTimeout(-1);
}