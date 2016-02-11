#include <IOKit/IOLib.h>
#include <IOKit/pci/IOPCIDevice.h>


class AppleIntelWiFiMVM : public IOService {
    OSDeclareDefaultStructors(AppleIntelWiFiMVM);
    
public:
    // IOService methods
    virtual bool init(OSDictionary *properties) override;
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
    virtual void free() override;

private:
    IOPCIDevice*    pciDevice;
    
};


#define	RELEASE(x)	if(x){(x)->release();(x)=NULL;}
