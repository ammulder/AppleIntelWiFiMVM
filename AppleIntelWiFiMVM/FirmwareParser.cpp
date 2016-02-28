//
//  firmware.c
//  AppleIntelWiFiMVM
//
//  Created by Aaron Mulder on 2/13/16.
//  Copyright © 2016 Aaron Mulder. All rights reserved.
//

#include "FirmwareParser.h"

#define super OSObject
OSDefineMetaClassAndStructors(FirmwareParser, OSObject);

bool FirmwareParser::processFirmwareData(OSData *raw, struct iwl_drv *drv) {
    return iwl_req_fw_callback((void *)raw->getBytesNoCopy(), (size_t)raw->getLength(), (void *)drv);
}

void FirmwareParser::releaseFirmware(struct iwl_drv *drv) {
    if(drv) iwl_dealloc_ucode(drv);
}
