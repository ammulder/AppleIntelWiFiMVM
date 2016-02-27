//
//  firmware.h
//  AppleIntelWiFiMVM
//
//  Created by Aaron Mulder on 2/13/16.
//  Copyright © 2016 Aaron Mulder. All rights reserved.
//

#ifndef firmware_h
#define firmware_h

#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>
extern "C" {
#include "iwl-config.h"
#include "iwl-agn-hw.h"
#include "iwl-fw.h"
#include "iwl-drv.h"
#include "iwl-debug.h"
}

class FirmwareParser : public OSObject {
    OSDeclareDefaultStructors(FirmwareParser);
    
public:
    bool processFirmwareData(OSData *raw, struct iwl_drv *drv);
    void releaseFirmware(struct iwl_drv *drv);
    
//private:
};
#endif /* firmware_h */
