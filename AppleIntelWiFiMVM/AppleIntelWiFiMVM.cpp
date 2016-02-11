#include "AppleIntelWiFiMVM.h"

#define super IOService
OSDefineMetaClassAndStructors(AppleIntelWiFiMVM, IOService);

// ------------------------ IOService Methods ----------------------------

bool AppleIntelWiFiMVM::init(OSDictionary *dict) {
    bool res = super::init(dict);
    IOLog("%s::init\n", getName());
    return res;
}

bool AppleIntelWiFiMVM::start(IOService* provider) {
    IOLog("%s::start\n", getName());
    if(!super::start(provider))
        return false;
    
    pciDevice = OSDynamicCast(IOPCIDevice, provider);
    if(!pciDevice)
        return false;
    
    pciDevice->retain();
    pciDevice->setMemoryEnable(true);
    registerService();
    
    return true;
}

void AppleIntelWiFiMVM::stop(IOService* provider) {
    IOLog("%s::stop\n", getName());
    super::stop(provider);
}

void AppleIntelWiFiMVM::free() {
    IOLog("%s::free\n", getName());
    RELEASE(pciDevice);
    super::free();
}
