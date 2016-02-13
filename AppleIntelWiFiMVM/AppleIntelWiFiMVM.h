#include <IOKit/IOLib.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <libkern/OSKextLib.h>
#include "linux/device-list.h"

class AppleIntelWiFiMVM : public IOService {
    OSDeclareDefaultStructors(AppleIntelWiFiMVM);
    
public:
    // --------------- IOService methods ---------------
    virtual bool init(OSDictionary *properties) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    virtual void free() override;

private:
    // --------------- Methods ---------------
    const struct iwl_cfg *identifyWiFiCard(UInt16 device, UInt16 subdevice);
    OSData* loadFirmwareSync(const struct iwl_cfg *device);
    static void firmwareLoadComplete( OSKextRequestTag requestTag, OSReturn result, const void *resourceData, uint32_t resourceDataLength, void *context);
    
    // --------------- Structs ---------------
    struct FirmwareLoadProgress {
        AppleIntelWiFiMVM *driver;
        OSData *firmwareData;
    };

    // --------------- Variables ---------------
    IOLock *firmwareLoadLock;
    IOPCIDevice *pciDevice;
    
};

#define MYNAME "AppleIntelWiFiMVM"
#define DEBUGLOG(args...) IOLog(args)
#define	RELEASE(x)	if(x){(x)->release();(x)=NULL;}
