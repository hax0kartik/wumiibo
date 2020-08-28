#include "input.h"

u32 waitInputWithTimeout(u32 msec) {
    bool pressedKey = false;
    u32 key = 0;
    u32 n = 0;

    //Wait for no keys to be pressed
    while(HID_PAD && (msec == 0 || n < msec)) {
        svcSleepThread(1 * 1000 * 1000LL);
        n++;
    }

    if(msec != 0 && n >= msec)
        return 0;

    do {
        //Wait for a key to be pressed
        while (!HID_PAD && (msec == 0 || n < msec)) {
            svcSleepThread(1 * 1000 * 1000LL);
            n++;
        }

        if((msec != 0 && n >= msec))
            return 0;

        key = HID_PAD;

        //Make sure it's pressed
        for(u32 i = 0x26000; i > 0; i --) {
            if(key != HID_PAD) break;
            if(i == 1) pressedKey = true;
        }
    } while(!pressedKey);

    return key;
}

u32 getKeysPressed() {
    return HID_PAD;
}

u32 waitInput() {
    return waitInputWithTimeout(0);
}

u32 waitComboWithTimeout(u32 msec) {
    u32 key = 0;
    u32 n = 0;

    //Wait for no keys to be pressed
    while(HID_PAD && (msec == 0 || n < msec)) {
        svcSleepThread(1 * 1000 * 1000LL);
        n++;
    }

    if((msec != 0 && n >= msec))
        return 0;

    do {
        svcSleepThread(1 * 1000 * 1000LL);
        n++;

        u32 tempKey = HID_PAD;

        for(u32 i = 0x26000; i > 0; i--) {
            if(tempKey != HID_PAD) break;
            if(i == 1) key = tempKey;
        }
    } while((!key || HID_PAD) && (msec == 0 || n < msec));

    if((msec != 0 && n >= msec))
        return 0;

    return key;
}

u32 waitCombo(void) {
    return waitComboWithTimeout(0);
}
