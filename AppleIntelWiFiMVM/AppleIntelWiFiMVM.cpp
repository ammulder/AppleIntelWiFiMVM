#include "AppleIntelWiFiMVM.h"
extern "C" {
#include "linux/linux-porting.h"
#include "linux/device-list.h"
}

#define super IOService
OSDefineMetaClassAndStructors(AppleIntelWiFiMVM, IOService);

// ------------------------ IOService Methods ----------------------------

bool AppleIntelWiFiMVM::init(OSDictionary *dict) {
    bool res = super::init(dict);
    DEBUGLOG("%s::init\n", MYNAME);
    return res;
}

bool AppleIntelWiFiMVM::start(IOService* provider) {
    DEBUGLOG("%s::start\n", MYNAME);
    if(!super::start(provider)) {
        IOLog("%s Super start failed\n", MYNAME);
        return false;
    }

    // Ensure we have a PCI device provider
    pciDevice = OSDynamicCast(IOPCIDevice, provider);
    if(!pciDevice) {
        IOLog("%s Provider not a PCIDevice\n", MYNAME);
        return false;
    }

    UInt16 vendor = pciDevice->configRead16(kIOPCIConfigVendorID);
    UInt16 device = pciDevice->configRead16(kIOPCIConfigDeviceID);
    UInt16 subsystem_vendor = pciDevice->configRead16(kIOPCIConfigSubSystemVendorID);
    UInt16 subsystem_device = pciDevice->configRead16(kIOPCIConfigSubSystemID);
    UInt8 revision = pciDevice->configRead8(kIOPCIConfigRevisionID);
    vendor = 0x8086;
    subsystem_vendor = 0x8086;
    device = 0x095a;
    subsystem_device = 0x9510;
    
    if(vendor != 0x8086 || subsystem_vendor != 0x8086) {
        IOLog("%s Unrecognized vendor/sub-vendor ID %#06x/%#06x; expecting 0x8086 for both; cannot load driver.\n",
              MYNAME, vendor, subsystem_vendor);
        return false;
    }

//    DEBUGLOG("%s Vendor %#06x Device %#06x SubVendor %#06x SubDevice %#06x Revision %#04x\n", MYNAME, vendor, device, subsystem_vendor, subsystem_device, revision);
    const struct iwl_cfg *card = identifyWiFiCard(device, subsystem_device);
    if(!card) {
        IOLog("%s Card has the right device ID %#06x but unmatched sub-device ID %#06x; cannot load driver.\n",
              MYNAME, device, subsystem_device);
        return false;
    }
    IOLog("%s loading for device %s\n", MYNAME, card->name);
    
    // Create locks for synchronization
    firmwareLoadLock = IOLockAlloc();
    if (!firmwareLoadLock) {
        IOLog("%s Unable to allocate firmware load lock\n", MYNAME);
        return false;
    }
    
    pciDevice->retain();

    loadFirmwareSync(card);
    
    pciDevice->setMemoryEnable(true);
    registerService();

    return true;
}

void AppleIntelWiFiMVM::stop(IOService* provider) {
    DEBUGLOG("%s::stop\n", MYNAME);
    if (firmwareLoadLock)
    {
        IOLockFree(firmwareLoadLock);
        firmwareLoadLock = NULL;
    }
    super::stop(provider);
}

void AppleIntelWiFiMVM::free() {
    DEBUGLOG("%s::free\n", MYNAME);
    RELEASE(pciDevice);
    super::free();
}

const struct iwl_cfg *AppleIntelWiFiMVM::identifyWiFiCard(UInt16 device, UInt16 subdevice) {
    UInt32 i;
    for(i=0; i<sizeof(wifi_card_ids) / sizeof(wifi_card); i++) {
        if(wifi_card_ids[i].device == device && wifi_card_ids[i].subdevice == subdevice)
            return wifi_card_ids[i].config;
    }
    
    return NULL;
}
