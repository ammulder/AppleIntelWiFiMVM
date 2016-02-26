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

static int iwl_parse_tlv_firmware(struct iwl_drv *drv,
                                  OSData *ucode_raw,
                                  struct iwl_firmware_pieces *pieces,
                                  struct iwl_ucode_capabilities *capa);
static int iwl_alloc_ucode(struct iwl_drv *drv,
                           struct iwl_firmware_pieces *pieces,
                           enum iwl_ucode_type type);
static void iwl_dealloc_ucode(struct iwl_drv *drv);

#endif /* firmware_h */
