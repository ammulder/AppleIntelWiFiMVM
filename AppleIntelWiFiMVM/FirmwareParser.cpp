//
//  firmware.c
//  AppleIntelWiFiMVM
//
//  Created by Aaron Mulder on 2/13/16.
//  Copyright © 2016 Aaron Mulder. All rights reserved.
//

#include "FirmwareParser.h"
#include <libkern/libkern.h>
#include <sys/malloc.h>

#define super OSObject
OSDefineMetaClassAndStructors(FirmwareParser, OSObject);

// From iwl-drv.c iwl_req_fw_callback
bool FirmwareParser::processFirmwareData(OSData *raw, struct iwl_drv *drv) {
    //struct iwl_drv *drv = context;
    struct iwl_fw *fw = &drv->fw;
//    struct iwl_ucode_header *ucode;  ONLY FOR non-TLV
//    struct iwlwifi_opmode_table *op;
    u32 *fileVer;
    int err;
    struct iwl_firmware_pieces *pieces;
    const unsigned int api_max = drv->cfg->ucode_api_max;
    unsigned int api_ok = drv->cfg->ucode_api_ok;
    const unsigned int api_min = drv->cfg->ucode_api_min;
    size_t trigger_tlv_sz[FW_DBG_TRIGGER_MAX];
    u32 api_ver;
    int i;
//    bool load_module = false;
    
    fw->ucode_capa.max_probe_length = IWL_DEFAULT_MAX_PROBE_LENGTH;
    fw->ucode_capa.standard_phy_calibration_size =
    IWL_DEFAULT_STANDARD_PHY_CALIBRATE_TBL_SIZE;
    fw->ucode_capa.n_scan_channels = IWL_DEFAULT_SCAN_CHANNELS;
    
    if (!api_ok)
        api_ok = api_max;
    MALLOC(pieces, struct iwl_firmware_pieces *, sizeof (*pieces), M_TEMP, M_ZERO);
    //pieces = (struct iwl_firmware_pieces *)/*kzalloc*/IOMalloc(sizeof(*pieces)/*, GFP_KERNEL*/);
    if (!pieces)
        return false;
    
/*    if (!ucode_raw) {
        if (drv->fw_index <= api_ok)
            IWL_ERR(drv,
                    "request for firmware file '%s' failed.\n",
                    drv->firmware_name);
        goto try_again;
    }
*/
    IWL_DEBUG_INFO(drv, "Loaded firmware file '%s' (%zd bytes).\n",
                   drv->firmware_name, raw->getLength());
    
    /* Make sure that we got at least the API version number */
    if (raw->getLength() < 4) {
        IWL_ERR(drv, "File size way too small!\n");
        return false;
    }
    
    /* Data from ucode file:  header followed by uCode images */
//    ucode = (struct iwl_ucode_header *)raw->getBytesNoCopy();
    fileVer = (u32 *)raw->getBytesNoCopy();
    if(*fileVer) {
        IOLog("%s Firmeware error: not TLV format! %08x", "AppleIntelWiFiMVM", *fileVer);
        return false;
    }
    
///*    if (ucode->ver)
//        err = iwl_parse_v1_v2_firmware(drv, ucode_raw, pieces);
//    else */
    err = iwl_parse_tlv_firmware(drv, raw, pieces,
                                 &fw->ucode_capa);
    IOLog("%s Parsed TLV firmware Release: %s\n", "AppleIntelWiFiMVM", drv->fw.fw_version);
    if (err)
        return false;

    if (fw_has_api(&drv->fw.ucode_capa, IWL_UCODE_TLV_API_NEW_VERSION))
        api_ver = drv->fw.ucode_ver;
    else
        api_ver = IWL_UCODE_API(drv->fw.ucode_ver);
    
    /*
     * api_ver should match the api version forming part of the
     * firmware filename ... but we don't check for that and only rely
     * on the API version read from firmware header from here on forward
     */
    /* no api version check required for experimental uCode */
    if (drv->fw_index != UCODE_EXPERIMENTAL_INDEX) {
        if (api_ver < api_min || api_ver > api_max) {
            IWL_ERR(drv,
                    "Driver unable to support your firmware API. "
                    "Driver supports v%u, firmware is v%u.\n",
                    api_max, api_ver);
            return false;
        }
        
        if (api_ver < api_ok) {
            if (api_ok != api_max)
                IWL_ERR(drv, "Firmware has old API version, "
                        "expected v%u through v%u, got v%u.\n",
                        api_ok, api_max, api_ver);
            else
                IWL_ERR(drv, "Firmware has old API version, "
                        "expected v%u, got v%u.\n",
                        api_max, api_ver);
            IWL_ERR(drv, "New firmware can be obtained from "
                    "http://www.intellinuxwireless.org/.\n");
        }
    }
    
    /*
     * In mvm uCode there is no difference between data and instructions
     * sections.
     */
    if (!fw->mvm_fw) {
        IOLog("%s ERROR: this driver only supports MVM firmware", "AppleIntelWiFiMVM");
        return false;
    }

    IOLog("%s Allocate ucode buffers...\n", "AppleIntelWiFiMVM");
    /* Allocate ucode buffers for card's bus-master loading ... */
    
    /* Runtime instructions and 2 copies of data:
     * 1) unmodified from disk
     * 2) backup cache for save/restore during power-downs */
    for (i = 0; i < IWL_UCODE_TYPE_MAX; i++)
        if (iwl_alloc_ucode(drv, pieces, (enum iwl_ucode_type)i))
            goto out_free_fw;
    
    if (pieces->dbg_dest_tlv) {
        drv->fw.dbg_dest_tlv = (iwl_fw_dbg_dest_tlv *)
        kmemdup(pieces->dbg_dest_tlv,
                sizeof(*pieces->dbg_dest_tlv) +
                sizeof(pieces->dbg_dest_tlv->reg_ops[0]) *
                drv->fw.dbg_dest_reg_num, GFP_KERNEL);
        
        if (!drv->fw.dbg_dest_tlv)
            goto out_free_fw;
    }
    
    for (i = 0; i < ARRAY_SIZE(drv->fw.dbg_conf_tlv); i++) {
        if (pieces->dbg_conf_tlv[i]) {
            drv->fw.dbg_conf_tlv_len[i] =
            pieces->dbg_conf_tlv_len[i];
            drv->fw.dbg_conf_tlv[i] = (iwl_fw_dbg_conf_tlv *)
            kmemdup(pieces->dbg_conf_tlv[i],
                    drv->fw.dbg_conf_tlv_len[i],
                    GFP_KERNEL);
            if (!drv->fw.dbg_conf_tlv[i])
                goto out_free_fw;
        }
    }
    
    IOLog("%s Triggers...\n", "AppleIntelWiFiMVM");
    memset(&trigger_tlv_sz, 0xff, sizeof(trigger_tlv_sz));
    
    trigger_tlv_sz[FW_DBG_TRIGGER_MISSED_BEACONS] =
    sizeof(struct iwl_fw_dbg_trigger_missed_bcon);
    trigger_tlv_sz[FW_DBG_TRIGGER_CHANNEL_SWITCH] = 0;
    trigger_tlv_sz[FW_DBG_TRIGGER_FW_NOTIF] =
    sizeof(struct iwl_fw_dbg_trigger_cmd);
    trigger_tlv_sz[FW_DBG_TRIGGER_MLME] =
    sizeof(struct iwl_fw_dbg_trigger_mlme);
    trigger_tlv_sz[FW_DBG_TRIGGER_STATS] =
    sizeof(struct iwl_fw_dbg_trigger_stats);
    trigger_tlv_sz[FW_DBG_TRIGGER_RSSI] =
    sizeof(struct iwl_fw_dbg_trigger_low_rssi);
    trigger_tlv_sz[FW_DBG_TRIGGER_TXQ_TIMERS] =
    sizeof(struct iwl_fw_dbg_trigger_txq_timer);
    trigger_tlv_sz[FW_DBG_TRIGGER_TIME_EVENT] =
    sizeof(struct iwl_fw_dbg_trigger_time_event);
    trigger_tlv_sz[FW_DBG_TRIGGER_BA] =
    sizeof(struct iwl_fw_dbg_trigger_ba);
    
    for (i = 0; i < ARRAY_SIZE(drv->fw.dbg_trigger_tlv); i++) {
        if (pieces->dbg_trigger_tlv[i]) {
            /*
             * If the trigger isn't long enough, WARN and exit.
             * Someone is trying to debug something and he won't
             * be able to catch the bug he is trying to chase.
             * We'd better be noisy to be sure he knows what's
             * going on.
             */
            if (WARN_ON(pieces->dbg_trigger_tlv_len[i] <
                        (trigger_tlv_sz[i] +
                         sizeof(struct iwl_fw_dbg_trigger_tlv))))
                goto out_free_fw;
            drv->fw.dbg_trigger_tlv_len[i] =
            pieces->dbg_trigger_tlv_len[i];
            drv->fw.dbg_trigger_tlv[i] = (struct iwl_fw_dbg_trigger_tlv *)
            kmemdup(pieces->dbg_trigger_tlv[i],
                    drv->fw.dbg_trigger_tlv_len[i],
                    GFP_KERNEL);
            if (!drv->fw.dbg_trigger_tlv[i])
                goto out_free_fw;
        }
    }
    
    IOLog("%s Initial configuration...\n", "AppleIntelWiFiMVM");
    /* Now that we can no longer fail, copy information */
    
    /*
     * The (size - 16) / 12 formula is based on the information recorded
     * for each event, which is of mode 1 (including timestamp) for all
     * new microcodes that include this information.
     */
    fw->init_evtlog_ptr = pieces->init_evtlog_ptr;
    if (pieces->init_evtlog_size)
        fw->init_evtlog_size = (pieces->init_evtlog_size - 16)/12;
    else
        fw->init_evtlog_size =
        drv->cfg->base_params->max_event_log_size;
    fw->init_errlog_ptr = pieces->init_errlog_ptr;
    fw->inst_evtlog_ptr = pieces->inst_evtlog_ptr;
    if (pieces->inst_evtlog_size)
        fw->inst_evtlog_size = (pieces->inst_evtlog_size - 16)/12;
    else
        fw->inst_evtlog_size =
        drv->cfg->base_params->max_event_log_size;
    fw->inst_errlog_ptr = pieces->inst_errlog_ptr;
    
    /*
     * figure out the offset of chain noise reset and gain commands
     * base on the size of standard phy calibration commands table size
     */
    if (fw->ucode_capa.standard_phy_calibration_size >
        IWL_MAX_PHY_CALIBRATE_TBL_SIZE)
        fw->ucode_capa.standard_phy_calibration_size =
        IWL_MAX_STANDARD_PHY_CALIBRATE_TBL_SIZE;

    
    
    
    
    
// WANT EVERYTHING IN THIS BLOCK COMMENTED OUT
//    /* We have our copies now, allow OS release its copies */
//    release_firmware(raw);
//    
//    mutex_lock(&iwlwifi_opmode_table_mtx);
//    if (fw->mvm_fw)
//        op = &iwlwifi_opmode_table[MVM_OP_MODE];
//    else
//        op = &iwlwifi_opmode_table[DVM_OP_MODE];
//    
//    IWL_INFO(drv, "loaded firmware version %s op_mode %s\n",
//             drv->fw.fw_version, op->name);
//    
//    /* add this device to the list of devices using this op_mode */
//    list_add_tail(&drv->list, &op->drv);
//    
//    if (op->ops) {
//        drv->op_mode = _iwl_op_mode_start(drv, op);
//        
//        if (!drv->op_mode) {
//            mutex_unlock(&iwlwifi_opmode_table_mtx);
//            goto out_unbind;
//        }
//    } else {
//        load_module = true;
//    }
//    mutex_unlock(&iwlwifi_opmode_table_mtx);
//    
//    /*
//     * Complete the firmware request last so that
//     * a driver unbind (stop) doesn't run while we
//     * are doing the start() above.
//     */
//    complete(&drv->request_firmware_complete);
//    
//    /*
//     * Load the module last so we don't block anything
//     * else from proceeding if the module fails to load
//     * or hangs loading.
//     */
//    if (load_module) {
//        err = request_module("%s", op->name);
//#ifdef CONFIG_IWLWIFI_OPMODE_MODULAR
//        if (err)
//            IWL_ERR(drv,
//                    "failed to load module %s (error %d), is dynamic loading enabled?\n",
//                    op->name, err);
//#endif
//    }
    FREE(pieces, M_TEMP);
    return true;
    
//try_again:
//    /* try next, if any */
//    release_firmware(ucode_raw);
//    if (iwl_request_firmware(drv, false))
//        goto out_unbind;
//    kfree(pieces);
//    return;
//
out_free_fw:
    IWL_ERR(drv, "failed to allocate pci memory\n");
    iwl_dealloc_ucode(drv);
//    release_firmware(ucode_raw);
//out_unbind:
    FREE(pieces, M_TEMP);
//    complete(&drv->request_firmware_complete);
//    device_release_driver(drv->trans->dev);
    return false;
}

void FirmwareParser::releaseFirmware(struct iwl_drv *drv) {
    if(drv) iwl_dealloc_ucode(drv);
}


/*
 * These functions are just to extract uCode section data from the pieces
 * structure.
 */
static struct fw_sec *get_sec(struct iwl_firmware_pieces *pieces,
                              enum iwl_ucode_type type,
                              int  sec)
{
    return &pieces->img[type].sec[sec];
}

static void set_sec_data(struct iwl_firmware_pieces *pieces,
                         enum iwl_ucode_type type,
                         int sec,
                         const void *data)
{
    pieces->img[type].sec[sec].data = data;
}

static void set_sec_size(struct iwl_firmware_pieces *pieces,
                         enum iwl_ucode_type type,
                         int sec,
                         size_t size)
{
    pieces->img[type].sec[sec].size = size;
}

static size_t get_sec_size(struct iwl_firmware_pieces *pieces,
                           enum iwl_ucode_type type,
                           int sec)
{
    return pieces->img[type].sec[sec].size;
}

static void set_sec_offset(struct iwl_firmware_pieces *pieces,
                           enum iwl_ucode_type type,
                           int sec,
                           u32 offset)
{
    pieces->img[type].sec[sec].offset = offset;
}

/*
 * Gets uCode section from tlv.
 */
static int iwl_store_ucode_sec(struct iwl_firmware_pieces *pieces,
                               const void *data, enum iwl_ucode_type type,
                               int size)
{
    struct fw_img_parsing *img;
    struct fw_sec *sec;
    struct fw_sec_parsing *sec_parse;
    
    if (!pieces || !data || type >= IWL_UCODE_TYPE_MAX) {
        IOLog("%s iwl_store_ucode_sec error in pieces, data, or type", "AppleIntelWiFiMVM");
        return -1;
    }
    
    sec_parse = (struct fw_sec_parsing *)data;
    
    img = &pieces->img[type];
    if(img->sec_counter < 0 || img->sec_counter >= IWL_UCODE_SECTION_MAX) {
        IOLog("%s iwl_store_ucode_sec BAD COUNTER %d\n", "AppleIntelWiFiMVM", img->sec_counter);
        return -1;
    }
        
    sec = &img->sec[img->sec_counter];
    
    sec->offset = le32_to_cpu(sec_parse->offset);
    sec->data = sec_parse->data;
    sec->size = size - sizeof(sec_parse->offset);
    
    ++img->sec_counter;
    
    return 0;
}

static int iwl_store_cscheme(struct iwl_fw *fw, const u8 *data, const u32 len)
{
    int i, j;
    struct iwl_fw_cscheme_list *l = (struct iwl_fw_cscheme_list *)data;
    struct iwl_fw_cipher_scheme *fwcs;
    struct ieee80211_cipher_scheme *cs;
    u32 cipher;
    
    if (len < sizeof(*l) ||
        len < sizeof(l->size) + l->size * sizeof(l->cs[0]))
        return -EINVAL;
    
    for (i = 0, j = 0; i < IWL_UCODE_MAX_CS && i < l->size; i++) {
        fwcs = &l->cs[j];
        cipher = le32_to_cpu(fwcs->cipher);
        
        /* we skip schemes with zero cipher suite selector */
        if (!cipher)
            continue;
        
        cs = &fw->cs[j++];
        cs->cipher = cipher;
        cs->iftype = BIT(NL80211_IFTYPE_STATION);
        cs->hdr_len = fwcs->hdr_len;
        cs->pn_len = fwcs->pn_len;
        cs->pn_off = fwcs->pn_off;
        cs->key_idx_off = fwcs->key_idx_off;
        cs->key_idx_mask = fwcs->key_idx_mask;
        cs->key_idx_shift = fwcs->key_idx_shift;
        cs->mic_len = fwcs->mic_len;
    }
    
    return 0;
}

static int iwl_set_default_calib(struct iwl_drv *drv, const u8 *data)
{
    struct iwl_tlv_calib_data *def_calib =
    (struct iwl_tlv_calib_data *)data;
    u32 ucode_type = le32_to_cpu(def_calib->ucode_type);
    if (ucode_type >= IWL_UCODE_TYPE_MAX) {
        IWL_ERR(drv, "Wrong ucode_type %u for default calibration.\n",
                ucode_type);
        return -EINVAL;
    }
    drv->fw.default_calib[ucode_type].flow_trigger =
    def_calib->calib.flow_trigger;
    drv->fw.default_calib[ucode_type].event_trigger =
    def_calib->calib.event_trigger;
    
    return 0;
}

static int iwl_set_ucode_api_flags(struct iwl_drv *drv, const u8 *data,
                                   struct iwl_ucode_capabilities *capa)
{
    const struct iwl_ucode_api *ucode_api = (iwl_ucode_api *)data;
    u32 api_index = le32_to_cpu(ucode_api->api_index);
    u32 api_flags = le32_to_cpu(ucode_api->api_flags);
    int i;
    
    if (api_index >= DIV_ROUND_UP(NUM_IWL_UCODE_TLV_API, 32)) {
        IWL_ERR(drv, "api_index larger than supported by driver\n");
        /* don't return an error so we can load FW that has more bits */
        return 0;
    }
    
    for (i = 0; i < 32; i++) {
        if (api_flags & BIT(i))
            set_bit(i + 32 * api_index, capa->_api);
    }
    
    return 0;
}

static int iwl_set_ucode_capabilities(struct iwl_drv *drv, const u8 *data,
                                      struct iwl_ucode_capabilities *capa)
{
    const struct iwl_ucode_capa *ucode_capa = (iwl_ucode_capa *)data;
    u32 api_index = le32_to_cpu(ucode_capa->api_index);
    u32 api_flags = le32_to_cpu(ucode_capa->api_capa);
    int i;
    
    if (api_index >= DIV_ROUND_UP(NUM_IWL_UCODE_TLV_CAPA, 32)) {
        IWL_ERR(drv, "api_index larger than supported by driver\n");
        /* don't return an error so we can load FW that has more bits */
        return 0;
    }
    
    for (i = 0; i < 32; i++) {
        if (api_flags & BIT(i))
            set_bit(i + 32 * api_index, capa->_capa);
    }
    
    return 0;
}

static int iwl_store_gscan_capa(struct iwl_fw *fw, const u8 *data,
                                const u32 len)
{
    struct iwl_fw_gscan_capabilities *fw_capa = (struct iwl_fw_gscan_capabilities *)data;
    struct iwl_gscan_capabilities *capa = &fw->gscan_capa;
    
    if (len < sizeof(*fw_capa))
        return -EINVAL;
    
    capa->max_scan_cache_size = le32_to_cpu(fw_capa->max_scan_cache_size);
    capa->max_scan_buckets = le32_to_cpu(fw_capa->max_scan_buckets);
    capa->max_ap_cache_per_scan =
    le32_to_cpu(fw_capa->max_ap_cache_per_scan);
    capa->max_rssi_sample_size = le32_to_cpu(fw_capa->max_rssi_sample_size);
    capa->max_scan_reporting_threshold =
    le32_to_cpu(fw_capa->max_scan_reporting_threshold);
    capa->max_hotlist_aps = le32_to_cpu(fw_capa->max_hotlist_aps);
    capa->max_significant_change_aps =
    le32_to_cpu(fw_capa->max_significant_change_aps);
    capa->max_bssid_history_entries =
    le32_to_cpu(fw_capa->max_bssid_history_entries);
    return 0;
}

#if DISABLED_CODE  // just for debugging my own load failures
static void check_pieces(struct iwl_firmware_pieces *pieces) {
    IOLog("%s Checking pieces\n", "AppleIntelWiFiMVM");
    IOLog("%s REGULAR sections %d\n", "AppleIntelWiFiMVM", pieces->img[IWL_UCODE_REGULAR].sec_counter);
    IOLog("%s INIT sections %d\n", "AppleIntelWiFiMVM", pieces->img[IWL_UCODE_INIT].sec_counter);
    IOLog("%s WOWLAN sections %d\n", "AppleIntelWiFiMVM", pieces->img[IWL_UCODE_WOWLAN].sec_counter);
    IOLog("%s USNIFFER sections %d\n", "AppleIntelWiFiMVM", pieces->img[IWL_UCODE_REGULAR_USNIFFER].sec_counter);
}
#endif // DISABLED_CODE

static int iwl_parse_tlv_firmware(struct iwl_drv *drv,
                                  OSData *ucode_raw,
                                  struct iwl_firmware_pieces *pieces,
                                  struct iwl_ucode_capabilities *capa)
{
    struct iwl_tlv_ucode_header *ucode = (struct iwl_tlv_ucode_header *)ucode_raw->getBytesNoCopy();
    struct iwl_ucode_tlv *tlv;
    size_t len = ucode_raw->getLength();
    const u8 *data;
    u32 tlv_len;
    u32 usniffer_img;
    enum iwl_ucode_tlv_type tlv_type;
    const u8 *tlv_data;
    char buildstr[25];
    u32 build, paging_mem_size;
    int num_of_cpus;
    bool usniffer_images = false;
    bool usniffer_req = false;
    bool gscan_capa = false;


//    if (len < sizeof(*ucode)) {
//        IWL_ERR(drv, "uCode has invalid length: %zd\n", len);
//        return -EINVAL;
//    }
    
    if (ucode->magic != cpu_to_le32(IWL_TLV_UCODE_MAGIC)) {
        IWL_ERR(drv, "invalid uCode magic: 0X%x\n",
                le32_to_cpu(ucode->magic));
        return -EINVAL;
    }
    
    drv->fw.ucode_ver = le32_to_cpu(ucode->ver);
    memcpy(drv->fw.human_readable, ucode->human_readable,
           sizeof(drv->fw.human_readable));
    build = le32_to_cpu(ucode->build);
    
    if (build)
        sprintf(buildstr, " build %u%s", build,
                (drv->fw_index == UCODE_EXPERIMENTAL_INDEX)
                ? " (EXP)" : "");
    else
        buildstr[0] = '\0';
    
    snprintf(drv->fw.fw_version,
             sizeof(drv->fw.fw_version),
             "%u.%u.%u.%u%s",
             IWL_UCODE_MAJOR(drv->fw.ucode_ver),
             IWL_UCODE_MINOR(drv->fw.ucode_ver),
             IWL_UCODE_API(drv->fw.ucode_ver),
             IWL_UCODE_SERIAL(drv->fw.ucode_ver),
             buildstr);
    
    data = ucode->data;
    
    len -= sizeof(*ucode);
    
    const u8 *start = data;
    size_t delta;
    
    while (len >= sizeof(*tlv)) {
        len -= sizeof(*tlv);
        tlv = (iwl_ucode_tlv *)data;
        delta = 88+(data-start);
        
        tlv_len = le32_to_cpu(tlv->length);
        tlv_type = (enum iwl_ucode_tlv_type)le32_to_cpu(tlv->type);
        tlv_data = tlv->data;
        
        if (len < tlv_len) {
            IWL_ERR(drv, "invalid TLV len: %zd/%u\n",
                    len, tlv_len);
            return -EINVAL;
        }
        len -= ALIGN(tlv_len, 4);
        data += sizeof(*tlv) + ALIGN(tlv_len, 4);
        
        switch (tlv_type) {
            case IWL_UCODE_TLV_INST:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_INST\n", "AppleIntelWiFiMVM", delta, tlv_type);
                set_sec_data(pieces, IWL_UCODE_REGULAR,
                             IWL_UCODE_SECTION_INST, tlv_data);
                set_sec_size(pieces, IWL_UCODE_REGULAR,
                             IWL_UCODE_SECTION_INST, tlv_len);
                set_sec_offset(pieces, IWL_UCODE_REGULAR,
                               IWL_UCODE_SECTION_INST,
                               IWLAGN_RTC_INST_LOWER_BOUND);
                break;
            case IWL_UCODE_TLV_DATA:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_DATA\n", "AppleIntelWiFiMVM", delta, tlv_type);
                set_sec_data(pieces, IWL_UCODE_REGULAR,
                             IWL_UCODE_SECTION_DATA, tlv_data);
                set_sec_size(pieces, IWL_UCODE_REGULAR,
                             IWL_UCODE_SECTION_DATA, tlv_len);
                set_sec_offset(pieces, IWL_UCODE_REGULAR,
                               IWL_UCODE_SECTION_DATA,
                               IWLAGN_RTC_DATA_LOWER_BOUND);
                break;
            case IWL_UCODE_TLV_INIT:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_INIT\n", "AppleIntelWiFiMVM", delta, tlv_type);
                set_sec_data(pieces, IWL_UCODE_INIT,
                             IWL_UCODE_SECTION_INST, tlv_data);
                set_sec_size(pieces, IWL_UCODE_INIT,
                             IWL_UCODE_SECTION_INST, tlv_len);
                set_sec_offset(pieces, IWL_UCODE_INIT,
                               IWL_UCODE_SECTION_INST,
                               IWLAGN_RTC_INST_LOWER_BOUND);
                break;
            case IWL_UCODE_TLV_INIT_DATA:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_INIT_DATA\n", "AppleIntelWiFiMVM", delta, tlv_type);
                set_sec_data(pieces, IWL_UCODE_INIT,
                             IWL_UCODE_SECTION_DATA, tlv_data);
                set_sec_size(pieces, IWL_UCODE_INIT,
                             IWL_UCODE_SECTION_DATA, tlv_len);
                set_sec_offset(pieces, IWL_UCODE_INIT,
                               IWL_UCODE_SECTION_DATA,
                               IWLAGN_RTC_DATA_LOWER_BOUND);
                break;
            case IWL_UCODE_TLV_BOOT:
                IWL_ERR(drv, "Found unexpected BOOT ucode\n");
                break;
            case IWL_UCODE_TLV_PROBE_MAX_LEN:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_PROBE_MAX_LEN\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                capa->max_probe_length = le32_to_cpu(*(UInt32 *)tlv_data);
                //le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_PAN:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_PAN\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len)
                    goto invalid_tlv_len;
                capa->flags |= IWL_UCODE_TLV_FLAGS_PAN;
                break;
            case IWL_UCODE_TLV_FLAGS:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_FLAGS\n", "AppleIntelWiFiMVM", delta, tlv_type);
                /* must be at least one u32 */
                if (tlv_len < sizeof(u32))
                    goto invalid_tlv_len;
                /* and a proper number of u32s */
                if (tlv_len % sizeof(u32))
                    goto invalid_tlv_len;
                /*
                 * This driver only reads the first u32 as
                 * right now no more features are defined,
                 * if that changes then either the driver
                 * will not work with the new firmware, or
                 * it'll not take advantage of new features.
                 */
                capa->flags = le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_API_CHANGES_SET:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_API_CHANGES_SET\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(struct iwl_ucode_api))
                    goto invalid_tlv_len;
                if (iwl_set_ucode_api_flags(drv, tlv_data, capa))
                    goto tlv_error;
                break;
            case IWL_UCODE_TLV_ENABLED_CAPABILITIES:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_ENABLED_CAPABILITIES\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(struct iwl_ucode_capa))
                    goto invalid_tlv_len;
                if (iwl_set_ucode_capabilities(drv, tlv_data, capa))
                    goto tlv_error;
                break;
            case IWL_UCODE_TLV_INIT_EVTLOG_PTR:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_INIT_EVTLOG_PTR\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                pieces->init_evtlog_ptr =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_INIT_EVTLOG_SIZE:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_INIT_EVTLOG_SIZE\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                pieces->init_evtlog_size =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_INIT_ERRLOG_PTR:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_INIT_ERRLOG_PTR\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                pieces->init_errlog_ptr =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_RUNT_EVTLOG_PTR:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_RUNT_EVTLOG_PTR\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                pieces->inst_evtlog_ptr =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_RUNT_EVTLOG_SIZE:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_RUNT_EVTLOG_SIZE\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                pieces->inst_evtlog_size =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_RUNT_ERRLOG_PTR:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_RUNT_ERRLOG_PTR\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                pieces->inst_errlog_ptr =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_ENHANCE_SENS_TBL:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_ENHANCE_SENS_TBL\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len)
                    goto invalid_tlv_len;
                drv->fw.enhance_sensitivity_table = true;
                break;
            case IWL_UCODE_TLV_WOWLAN_INST:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_WOWLAN_INST\n", "AppleIntelWiFiMVM", delta, tlv_type);
                set_sec_data(pieces, IWL_UCODE_WOWLAN,
                             IWL_UCODE_SECTION_INST, tlv_data);
                set_sec_size(pieces, IWL_UCODE_WOWLAN,
                             IWL_UCODE_SECTION_INST, tlv_len);
                set_sec_offset(pieces, IWL_UCODE_WOWLAN,
                               IWL_UCODE_SECTION_INST,
                               IWLAGN_RTC_INST_LOWER_BOUND);
                break;
            case IWL_UCODE_TLV_WOWLAN_DATA:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_WOWLAN_DATA\n", "AppleIntelWiFiMVM", delta, tlv_type);
                set_sec_data(pieces, IWL_UCODE_WOWLAN,
                             IWL_UCODE_SECTION_DATA, tlv_data);
                set_sec_size(pieces, IWL_UCODE_WOWLAN,
                             IWL_UCODE_SECTION_DATA, tlv_len);
                set_sec_offset(pieces, IWL_UCODE_WOWLAN,
                               IWL_UCODE_SECTION_DATA,
                               IWLAGN_RTC_DATA_LOWER_BOUND);
                break;
            case IWL_UCODE_TLV_PHY_CALIBRATION_SIZE:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_PHY_CALIBRATION_SIZE\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                capa->standard_phy_calibration_size =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_SEC_RT:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SEC_RT\n", "AppleIntelWiFiMVM", delta, tlv_type);
                iwl_store_ucode_sec(pieces, tlv_data, IWL_UCODE_REGULAR,
                                    tlv_len);
                drv->fw.mvm_fw = true;
                break;
            case IWL_UCODE_TLV_SEC_INIT:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SEC_INIT\n", "AppleIntelWiFiMVM", delta, tlv_type);
                iwl_store_ucode_sec(pieces, tlv_data, IWL_UCODE_INIT,
                                    tlv_len);
                drv->fw.mvm_fw = true;
                break;
            case IWL_UCODE_TLV_SEC_WOWLAN:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SEC_WOWLAN\n", "AppleIntelWiFiMVM", delta, tlv_type);
                iwl_store_ucode_sec(pieces, tlv_data, IWL_UCODE_WOWLAN,
                                    tlv_len);
                drv->fw.mvm_fw = true;
                break;
            case IWL_UCODE_TLV_DEF_CALIB:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_DEF_CALIB\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(struct iwl_tlv_calib_data))
                    goto invalid_tlv_len;
                if (iwl_set_default_calib(drv, tlv_data))
                    goto tlv_error;
                break;
            case IWL_UCODE_TLV_PHY_SKU:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_PHY_SKU\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                drv->fw.phy_config = le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                drv->fw.valid_tx_ant = (drv->fw.phy_config &
                                        FW_PHY_CFG_TX_CHAIN) >>
                FW_PHY_CFG_TX_CHAIN_POS;
                drv->fw.valid_rx_ant = (drv->fw.phy_config &
                                        FW_PHY_CFG_RX_CHAIN) >>
                FW_PHY_CFG_RX_CHAIN_POS;
                break;
            case IWL_UCODE_TLV_SECURE_SEC_RT:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SECURE_SEC_RT\n", "AppleIntelWiFiMVM", delta, tlv_type);
                iwl_store_ucode_sec(pieces, tlv_data, IWL_UCODE_REGULAR,
                                    tlv_len);
                drv->fw.mvm_fw = true;
                break;
            case IWL_UCODE_TLV_SECURE_SEC_INIT:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SECURE_SEC_INIT\n", "AppleIntelWiFiMVM", delta, tlv_type);
                iwl_store_ucode_sec(pieces, tlv_data, IWL_UCODE_INIT,
                                    tlv_len);
                drv->fw.mvm_fw = true;
                break;
            case IWL_UCODE_TLV_SECURE_SEC_WOWLAN:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SECURE_SEC_WOWLAN\n", "AppleIntelWiFiMVM", delta, tlv_type);
                iwl_store_ucode_sec(pieces, tlv_data, IWL_UCODE_WOWLAN,
                                    tlv_len);
                drv->fw.mvm_fw = true;
                break;
            case IWL_UCODE_TLV_NUM_OF_CPU:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_NUM_OF_CPU\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                num_of_cpus =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                
                if (num_of_cpus == 2) {
                    drv->fw.img[IWL_UCODE_REGULAR].is_dual_cpus =
                    true;
                    drv->fw.img[IWL_UCODE_INIT].is_dual_cpus =
                    true;
                    drv->fw.img[IWL_UCODE_WOWLAN].is_dual_cpus =
                    true;
                } else if ((num_of_cpus > 2) || (num_of_cpus < 1)) {
                    IWL_ERR(drv, "Driver support upto 2 CPUs\n");
                    return -EINVAL;
                }
                break;
            case IWL_UCODE_TLV_CSCHEME:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_CSCHEME\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (iwl_store_cscheme(&drv->fw, tlv_data, tlv_len))
                    goto invalid_tlv_len;
                break;
            case IWL_UCODE_TLV_N_SCAN_CHANNELS:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_N_SCAN_CHANNELS\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                capa->n_scan_channels =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_FW_VERSION: {
                IOLog("%s %#10zx %d IWL_UCODE_TLV_FW_VERSION\n", "AppleIntelWiFiMVM", delta, tlv_type);
                __le32 *ptr = (__le32 *)tlv_data;
                u32 major, minor;
                u8 local_comp;
                
                if (tlv_len != sizeof(u32) * 3)
                    goto invalid_tlv_len;
                
                major = le32_to_cpu(*(UInt32 *)ptr++);//le32_to_cpup(ptr++);
                minor = le32_to_cpu(*(UInt32 *)ptr++);//le32_to_cpup(ptr++);
                local_comp = le32_to_cpu(*(UInt32 *)ptr);//le32_to_cpup(ptr);
                
                snprintf(drv->fw.fw_version,
                         sizeof(drv->fw.fw_version), "%u.%u.%u",
                         major, minor, local_comp);
                break;
            }
            case IWL_UCODE_TLV_FW_DBG_DEST: {
                IOLog("%s %#10zx %d IWL_UCODE_TLV_FW_DBG_DEST\n", "AppleIntelWiFiMVM", delta, tlv_type);
                struct iwl_fw_dbg_dest_tlv *dest = (struct iwl_fw_dbg_dest_tlv *)tlv_data;
                
                if (pieces->dbg_dest_tlv) {
                    IWL_ERR(drv,
                            "dbg destination ignored, already exists\n");
                    break;
                }
                
                pieces->dbg_dest_tlv = dest;
                IWL_INFO(drv, "Found debug destination: %s\n",
                         get_fw_dbg_mode_string(dest->monitor_mode));
                
                drv->fw.dbg_dest_reg_num =
                tlv_len - offsetof(struct iwl_fw_dbg_dest_tlv,
                                   reg_ops);
                drv->fw.dbg_dest_reg_num /=
                sizeof(drv->fw.dbg_dest_tlv->reg_ops[0]);
                
                break;
            }
            case IWL_UCODE_TLV_FW_DBG_CONF: {
                IOLog("%s %#10zx %d IWL_UCODE_TLV_FW_DBG_CONF\n", "AppleIntelWiFiMVM", delta, tlv_type);
                struct iwl_fw_dbg_conf_tlv *conf = (struct iwl_fw_dbg_conf_tlv *)tlv_data;
                
                if (!pieces->dbg_dest_tlv) {
                    IWL_ERR(drv,
                            "Ignore dbg config %d - no destination configured\n",
                            conf->id);
                    break;
                }
                
                if (conf->id >= ARRAY_SIZE(drv->fw.dbg_conf_tlv)) {
                    IWL_ERR(drv,
                            "Skip unknown configuration: %d\n",
                            conf->id);
                    break;
                }
                
                if (pieces->dbg_conf_tlv[conf->id]) {
                    IWL_ERR(drv,
                            "Ignore duplicate dbg config %d\n",
                            conf->id);
                    break;
                }
                
                if (conf->usniffer)
                    usniffer_req = true;
                
                IWL_INFO(drv, "Found debug configuration: %d\n",
                         conf->id);
                
                pieces->dbg_conf_tlv[conf->id] = conf;
                pieces->dbg_conf_tlv_len[conf->id] = tlv_len;
                break;
            }
            case IWL_UCODE_TLV_FW_DBG_TRIGGER: {
                IOLog("%s %#10zx %d IWL_UCODE_TLV_FW_DBG_TRIGGER\n", "AppleIntelWiFiMVM", delta, tlv_type);
                struct iwl_fw_dbg_trigger_tlv *trigger =
                (struct iwl_fw_dbg_trigger_tlv *)tlv_data;
                u32 trigger_id = le32_to_cpu(trigger->id);
                
                if (trigger_id >= ARRAY_SIZE(drv->fw.dbg_trigger_tlv)) {
                    IWL_ERR(drv,
                            "Skip unknown trigger: %u\n",
                            trigger->id);
                    break;
                }
                
                if (pieces->dbg_trigger_tlv[trigger_id]) {
                    IWL_ERR(drv,
                            "Ignore duplicate dbg trigger %u\n",
                            trigger->id);
                    break;
                }
                
                IWL_INFO(drv, "Found debug trigger: %u\n", trigger->id);
                
                pieces->dbg_trigger_tlv[trigger_id] = trigger;
                pieces->dbg_trigger_tlv_len[trigger_id] = tlv_len;
                break;
            }
            case IWL_UCODE_TLV_SEC_RT_USNIFFER:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SEC_RT_USNIFFER\n", "AppleIntelWiFiMVM", delta, tlv_type);
                usniffer_images = true;
                iwl_store_ucode_sec(pieces, tlv_data,
                                    IWL_UCODE_REGULAR_USNIFFER,
                                    tlv_len);
                break;
            case IWL_UCODE_TLV_PAGING:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_PAGING\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                paging_mem_size = le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                
                IWL_DEBUG_FW(drv,
                             "Paging: paging enabled (size = %u bytes)\n",
                             paging_mem_size);
                
                if (paging_mem_size > MAX_PAGING_IMAGE_SIZE) {
                    IWL_ERR(drv,
                            "Paging: driver supports up to %lu bytes for paging image\n",
                            MAX_PAGING_IMAGE_SIZE);
                    return -EINVAL;
                }
                
                if (paging_mem_size & (FW_PAGING_SIZE - 1)) {
                    IWL_ERR(drv,
                            "Paging: image isn't multiple %lu\n",
                            FW_PAGING_SIZE);
                    return -EINVAL;
                }
                
                drv->fw.img[IWL_UCODE_REGULAR].paging_mem_size =
                paging_mem_size;
                usniffer_img = IWL_UCODE_REGULAR_USNIFFER;
                drv->fw.img[usniffer_img].paging_mem_size =
                paging_mem_size;
                break;
            case IWL_UCODE_TLV_SDIO_ADMA_ADDR:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_SDIO_ADMA_ADDR\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (tlv_len != sizeof(u32))
                    goto invalid_tlv_len;
                drv->fw.sdio_adma_addr =
                le32_to_cpu(*(UInt32 *)tlv_data);//le32_to_cpup((__le32 *)tlv_data);
                break;
            case IWL_UCODE_TLV_FW_GSCAN_CAPA:
                IOLog("%s %#10zx %d IWL_UCODE_TLV_FW_GSCAN_CAPA\n", "AppleIntelWiFiMVM", delta, tlv_type);
                if (iwl_store_gscan_capa(&drv->fw, tlv_data, tlv_len))
                    goto invalid_tlv_len;
                gscan_capa = true;
                break;
            default:
                IWL_DEBUG_INFO(drv, "%s unknown TLV at %#10zx: %d\n", "AppleIntelWiFiMVM", delta, tlv_type);
                break;
        }
    }
    
    if (usniffer_req && !usniffer_images) {
        IWL_ERR(drv,
                "user selected to work with usniffer but usniffer image isn't available in ucode package\n");
        return -EINVAL;
    }
    
    if (len) {
        IWL_ERR(drv, "invalid TLV after parsing: %zd\n", len);
        iwl_print_hex_dump(drv, IWL_DL_FW, (u8 *)data, len);
        return -EINVAL;
    }
    
    /*
     * If ucode advertises that it supports GSCAN but GSCAN
     * capabilities TLV is not present, warn and continue without GSCAN.
     */
    if (fw_has_capa(capa, IWL_UCODE_TLV_CAPA_GSCAN_SUPPORT) &&
        WARN(!gscan_capa,
             "GSCAN is supported but capabilities TLV is unavailable\n"))
        clear_bit((/*__force */long)IWL_UCODE_TLV_CAPA_GSCAN_SUPPORT,
                    capa->_capa);
    
    return 0;
    
invalid_tlv_len:
    IWL_ERR(drv, "TLV %d has invalid size: %u\n", tlv_type, tlv_len);
tlv_error:
    iwl_print_hex_dump(drv, IWL_DL_FW, tlv_data, tlv_len);
    
    return -EINVAL;
}


static int iwl_alloc_fw_desc(struct iwl_drv *drv, struct fw_desc *desc,
                             struct fw_sec *sec)
{
    void *data;
    
    desc->data = NULL;
    
    if (!sec || !sec->size)
        return -EINVAL;

    MALLOC(data, void *, sec->size, M_TEMP, M_WAITOK);
    if (!data)
        return -ENOMEM;
    
    desc->len = sec->size;
    desc->offset = sec->offset;
    memcpy(data, sec->data, desc->len);
    desc->data = data;
    
    return 0;
}

static int iwl_alloc_ucode(struct iwl_drv *drv,
                           struct iwl_firmware_pieces *pieces,
                           enum iwl_ucode_type type)
{
    int i;
    for (i = 0;
         i < IWL_UCODE_SECTION_MAX && get_sec_size(pieces, type, i);
         i++)
        if (iwl_alloc_fw_desc(drv, &(drv->fw.img[type].sec[i]),
                              get_sec(pieces, type, i)))
            return -ENOMEM;
    return 0;
}

static void iwl_free_fw_desc(struct iwl_drv *drv, struct fw_desc *desc)
{
    FREE(desc->data, M_TEMP);
    desc->data = NULL;
    desc->len = 0;
}

static void iwl_free_fw_img(struct iwl_drv *drv, struct fw_img *img)
{
    int i;
    for (i = 0; i < IWL_UCODE_SECTION_MAX; i++)
        iwl_free_fw_desc(drv, &img->sec[i]);
}

static void iwl_dealloc_ucode(struct iwl_drv *drv)
{
    int i;
    
    if(drv->fw.dbg_dest_tlv) FREE(drv->fw.dbg_dest_tlv, M_TEMP);
    for (i = 0; i < ARRAY_SIZE(drv->fw.dbg_conf_tlv); i++)
        if(drv->fw.dbg_conf_tlv[i])
            FREE(drv->fw.dbg_conf_tlv[i], M_TEMP);
    for (i = 0; i < ARRAY_SIZE(drv->fw.dbg_trigger_tlv); i++)
        FREE(drv->fw.dbg_trigger_tlv[i], M_TEMP);
    
    for (i = 0; i < IWL_UCODE_TYPE_MAX; i++)
        iwl_free_fw_img(drv, drv->fw.img + i);
}
