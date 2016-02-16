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
//#include <IOKit/pci/IOPCIDevice.h>
//#include <libkern/OSKextLib.h>
#include "firmware-defs.h"
#include "iwl-config.h"
#include "iwl-agn-hw.h"


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
//static int validate_sec_sizes(struct iwl_drv *drv,
//                              struct iwl_firmware_pieces *pieces,
//                              const struct iwl_cfg *cfg);
static int iwl_alloc_ucode(struct iwl_drv *drv,
                           struct iwl_firmware_pieces *pieces,
                           enum iwl_ucode_type type);
static void iwl_dealloc_ucode(struct iwl_drv *drv);

/**
 * struct iwl_fw - variables associated with the firmware
 *
 * @ucode_ver: ucode version from the ucode file
 * @fw_version: firmware version string
 * @img: ucode image like ucode_rt, ucode_init, ucode_wowlan.
 * @ucode_capa: capabilities parsed from the ucode file.
 * @enhance_sensitivity_table: device can do enhanced sensitivity.
 * @init_evtlog_ptr: event log offset for init ucode.
 * @init_evtlog_size: event log size for init ucode.
 * @init_errlog_ptr: error log offfset for init ucode.
 * @inst_evtlog_ptr: event log offset for runtime ucode.
 * @inst_evtlog_size: event log size for runtime ucode.
 * @inst_errlog_ptr: error log offfset for runtime ucode.
 * @mvm_fw: indicates this is MVM firmware
 * @cipher_scheme: optional external cipher scheme.
 * @human_readable: human readable version
 * @sdio_adma_addr: the default address to set for the ADMA in SDIO mode until
 *	we get the ALIVE from the uCode
 * @dbg_dest_tlv: points to the destination TLV for debug
 * @dbg_conf_tlv: array of pointers to configuration TLVs for debug
 * @dbg_conf_tlv_len: lengths of the @dbg_conf_tlv entries
 * @dbg_trigger_tlv: array of pointers to triggers TLVs
 * @dbg_trigger_tlv_len: lengths of the @dbg_trigger_tlv entries
 * @dbg_dest_reg_num: num of reg_ops in %dbg_dest_tlv
 */
struct iwl_fw {
    u32 ucode_ver;
    
    char fw_version[ETHTOOL_FWVERS_LEN];
    
    /* ucode images */
    struct fw_img img[IWL_UCODE_TYPE_MAX];
    
    struct iwl_ucode_capabilities ucode_capa;
    bool enhance_sensitivity_table;
    
    u32 init_evtlog_ptr, init_evtlog_size, init_errlog_ptr;
    u32 inst_evtlog_ptr, inst_evtlog_size, inst_errlog_ptr;
    
    struct iwl_tlv_calib_ctrl default_calib[IWL_UCODE_TYPE_MAX];
    u32 phy_config;
    u8 valid_tx_ant;
    u8 valid_rx_ant;
    
    bool mvm_fw;
    
    struct ieee80211_cipher_scheme cs[IWL_UCODE_MAX_CS];
    u8 human_readable[FW_VER_HUMAN_READABLE_SZ];
    
    u32 sdio_adma_addr;
    
    struct iwl_fw_dbg_dest_tlv *dbg_dest_tlv;
    struct iwl_fw_dbg_conf_tlv *dbg_conf_tlv[FW_DBG_CONF_MAX];
    size_t dbg_conf_tlv_len[FW_DBG_CONF_MAX];
    struct iwl_fw_dbg_trigger_tlv *dbg_trigger_tlv[FW_DBG_TRIGGER_MAX];
    size_t dbg_trigger_tlv_len[FW_DBG_TRIGGER_MAX];
    u8 dbg_dest_reg_num;
    struct iwl_gscan_capabilities gscan_capa;
};

/**
 * struct iwl_drv - drv common data
 * @list: list of drv structures using this opmode
 * @fw: the iwl_fw structure
 * @op_mode: the running op_mode
 * @trans: transport layer
 * @dev: for debug prints only
 * @cfg: configuration struct
 * @fw_index: firmware revision to try loading
 * @firmware_name: composite filename of ucode file to load
 * @request_firmware_complete: the firmware has been obtained from user space
 */
struct iwl_drv {
//    struct list_head list;
    struct iwl_fw fw;
    
    struct iwl_op_mode *op_mode;
    struct iwl_trans *trans;
//    struct device *dev;
    const struct iwl_cfg *cfg;
    
    int fw_index;                   /* firmware we're trying to load */
    char firmware_name[32];         /* name of firmware file to load */
    
    struct {
        int dbg_dest_tlv;
        int dbg_conf_tlv[FW_DBG_CONF_MAX];
        int dbg_trigger_tlv[FW_DBG_TRIGGER_MAX];
    } sizes;
};



#endif /* firmware_h */
