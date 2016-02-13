//
//  FirmwareLoader.cpp
//
//  Copyright © 2016 Aaron Mulder. All rights reserved.
//
#include "AppleIntelWiFiMVM.h"

// static
void AppleIntelWiFiMVM::firmwareLoadComplete( OSKextRequestTag requestTag, OSReturn result, const void *resourceData, uint32_t resourceDataLength, void *context) {
    DEBUGLOG("%s Received firmware load callback...\n", MYNAME);
    FirmwareLoadProgress *progress = (FirmwareLoadProgress *)context;
    DEBUGLOG("%s Attempting firmware lock to prepare data\n", MYNAME);
    IOLockLock(progress->driver->firmwareLoadLock);
    if(result == kOSReturnSuccess)
        progress->firmwareData = OSData::withBytes(resourceData, resourceDataLength);
    else
        IOLog("%s firmwareLoadComplete FAILURE: %08x.\n", MYNAME, result);
    DEBUGLOG("%s Releasing firmware lock\n", MYNAME);
    IOLockUnlock(progress->driver->firmwareLoadLock);
    DEBUGLOG("%s Waking sleepers...\n", MYNAME);
    IOLockWakeup(progress->driver->firmwareLoadLock, progress->driver, true);
    DEBUGLOG("%s Callback complete\n", MYNAME);
}

OSData* AppleIntelWiFiMVM::loadFirmwareSync(const struct iwl_cfg *device) {
    DEBUGLOG("%s Loading firmware\n", MYNAME);
    IOLockLock(firmwareLoadLock);
    FirmwareLoadProgress progress = {.driver = this, .firmwareData = NULL};
    DEBUGLOG("%s Requesting firmware load...\n", MYNAME);
    OSReturn ret = OSKextRequestResource(OSKextGetCurrentIdentifier(),
                                         "iwlwifi-8000C-16.ucode",
                                         firmwareLoadComplete,
                                         &progress,
                                         NULL);
    if(ret != kOSReturnSuccess) {
        IOLog("%s Unable to load firmware file %08x\n", MYNAME, ret);
        IOLockUnlock(firmwareLoadLock);
        return NULL;
    }
    DEBUGLOG("%s Waiting for firmware load...\n", MYNAME);
    IOLockSleep(firmwareLoadLock, this, 0);
    DEBUGLOG("%s Woke up after waiting for firmware lock...\n", MYNAME);
    IOLockUnlock(firmwareLoadLock);
    
    DEBUGLOG("%s %s firmware file iwlwifi-8000C-16.ucode\n", MYNAME, progress.firmwareData ? "LOADED" : "FAILED");
    
    return progress.firmwareData;
}