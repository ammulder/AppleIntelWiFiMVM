#include "AppleIntelWiFiMVM.h"
extern "C" {
//#include "linux/linux-porting.h"
#include "device-list.h"
#include "iwl-csr.h"
#include "iwl-trans.h"
}

#define super IOService
OSDefineMetaClassAndStructors(AppleIntelWiFiMVM, IOService);

// ------------------------ IOService Methods ----------------------------

bool AppleIntelWiFiMVM::init(OSDictionary *dict) {
    bool res = super::init(dict);
    DEBUGLOG("%s::init\n", MYNAME);
    OSCollectionIterator *it = OSCollectionIterator::withCollection(dict);
    OSObject *key, *value;
    OSString *ks, *vs;
    va_list foo;
    IOLog("%s DICT total count %d\n", MYNAME, dict->getCount());
    while ((key = it->getNextObject())) {
        ks = OSDynamicCast(OSString, key);
        if(!ks) IOLog("%s DICT Unrecognized key class\n", MYNAME);
        else {
            value = dict->getObject(ks);
            vs = OSDynamicCast(OSString, value);
            if (!vs) IOLog("%s DICT %s = (not a string)\n", MYNAME, ks->getCStringNoCopy());
            else IOLog("%s DICT %s = %s\n", MYNAME, ks->getCStringNoCopy(), vs->getCStringNoCopy());
        }
    }
    RELEASE(it)
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
    pciDevice->retain();

    const struct iwl_cfg *card = identifyWiFiCard();
    if(!card) {
        IOLog("%s Unable to find or configure WiFi hardware.\n", MYNAME);
        return false;
    }
    IOLog("%s loading for device %s\n", MYNAME, card->name);
    
    // Create locks for synchronization
    firmwareLoadLock = IOLockAlloc();
    if (!firmwareLoadLock) {
        IOLog("%s Unable to allocate firmware load lock\n", MYNAME);
        return false;
    }
    
    IOLog("%s Starting Firmware...\n", MYNAME);
    if(!startFirmware(card, NULL)) {// TODO: PCI transport
        IOLog("%s Unable to start firmware\n", MYNAME);
        return false;
    }
    
    pciDevice->setMemoryEnable(true);
    registerService();

    return true;
}

void AppleIntelWiFiMVM::stop(IOService* provider) {
    DEBUGLOG("%s::stop\n", MYNAME);
    if(driver) stopFirmware();
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
    if(driver) {
        IOFree(driver, sizeof(*driver));
        driver = NULL;
    }
    super::free();
}

const struct iwl_cfg *AppleIntelWiFiMVM::identifyWiFiCard() {
    UInt32 i;
    const struct iwl_cfg *result = NULL;
    const struct iwl_cfg *cfg_7265d __maybe_unused = NULL;
    struct iwl_trans *iwl_trans;
    struct iwl_trans_pcie *trans_pcie;

    UInt16 vendor = pciDevice->configRead16(kIOPCIConfigVendorID);
    UInt16 device = pciDevice->configRead16(kIOPCIConfigDeviceID);
    UInt16 subsystem_vendor = pciDevice->configRead16(kIOPCIConfigSubSystemVendorID);
    UInt16 subsystem_device = pciDevice->configRead16(kIOPCIConfigSubSystemID);
    UInt8 revision = pciDevice->configRead8(kIOPCIConfigRevisionID);
//    vendor = 0x8086;
//    subsystem_vendor = 0x8086;
    // Broadwell NUC 7265
//    device = 0x095a;
//    subsystem_device = 0x9510;
    // Skylake NUC 8260
//    device = 0x24F3;
//    subsystem_device = 0x9010;
    // 7260
//    device = 0x08B1;
//    subsystem_device = 0x4070;
    // 3160
//    device = 0x08B3;
//    subsystem_device = 0x0070;
    // 3165 uses 7165D firmware
//    device = 0x3165;
//    subsystem_device = 0x4010;
    // 4165 uses 8260 firmware above, not retested here

    if(vendor != 0x8086 || subsystem_vendor != 0x8086) {
        IOLog("%s Unrecognized vendor/sub-vendor ID %#06x/%#06x; expecting 0x8086 for both; cannot load driver.\n",
                MYNAME, vendor, subsystem_vendor);
        return NULL;
    }

//    DEBUGLOG("%s Vendor %#06x Device %#06x SubVendor %#06x SubDevice %#06x Revision %#04x\n", MYNAME, vendor, device, subsystem_vendor, subsystem_device, revision);


    // STEP 1: find the configuration data based on the PCI IDs
    for(i=0; i<sizeof(iwl_hw_card_ids) / sizeof(pci_device_id); i++) {
        if(iwl_hw_card_ids[i].device == device && iwl_hw_card_ids[i].subdevice == subsystem_device) {
            result = (iwl_cfg *) iwl_hw_card_ids[i].driver_data;
            break;
        }
    }
    if(!result) {
        IOLog("%s Card has the right device ID %#06x but unmatched sub-device ID %#06x; cannot load driver.\n",
                MYNAME, device, subsystem_device);
        return NULL;
    }

    // STEP 2: configure the PCIe Transport


    // STEP 3: double-check the configuration data based on the hardware revision in the PCIe Transport
    /*
     * special-case 7265D, it has the same PCI IDs.
     *
     * Note that because we already pass the cfg to the transport above,
     * all the parameters that the transport uses must, until that is
     * changed, be identical to the ones in the 7265D configuration.
     */
    if (result == &iwl7265_2ac_cfg)
        cfg_7265d = &iwl7265d_2ac_cfg;
    else if (result == &iwl7265_2n_cfg)
        cfg_7265d = &iwl7265d_2n_cfg;
    else if (result == &iwl7265_n_cfg)
        cfg_7265d = &iwl7265d_n_cfg;
    if (cfg_7265d &&
            (iwl_trans->hw_rev & CSR_HW_REV_TYPE_MSK) == CSR_HW_REV_TYPE_7265D) {
        result = cfg_7265d;
        iwl_trans->cfg = cfg_7265d;
    }

    return result;
}

struct iwl_trans *AppleIntelWiFiMVM::allocatePCIeTransport(const struct iwl_cfg *cfg) {
    struct iwl_trans_pcie *trans_pcie = NULL;
    struct iwl_trans *trans = NULL;
    u16 pci_cmd;
    int ret;
#if DISABLED_CODE
    trans = iwl_trans_alloc(sizeof(struct iwl_trans_pcie),
            &pdev->dev, cfg, &trans_ops_pcie, 0);
    if (!trans)
        return ERR_PTR(-ENOMEM);

    trans->max_skb_frags = IWL_PCIE_MAX_FRAGS;

    trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

    trans_pcie->trans = trans;
    spin_lock_init(&trans_pcie->irq_lock);
    spin_lock_init(&trans_pcie->reg_lock);
    spin_lock_init(&trans_pcie->ref_lock);
    mutex_init(&trans_pcie->mutex);
    init_waitqueue_head(&trans_pcie->ucode_write_waitq);

    ret = pci_enable_device(pdev);
    if (ret)
        goto out_no_pci;

    if (!cfg->base_params->pcie_l1_allowed) {
        /*
         * W/A - seems to solve weird behavior. We need to remove this
         * if we don't want to stay in L1 all the time. This wastes a
         * lot of power.
         */
        pci_disable_link_state(pdev, PCIE_LINK_STATE_L0S |
                PCIE_LINK_STATE_L1 |
                PCIE_LINK_STATE_CLKPM);
    }

    pci_set_master(pdev);

    ret = pci_set_dma_mask(pdev, DMA_BIT_MASK(36));
    if (!ret)
        ret = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(36));
    if (ret) {
        ret = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
        if (!ret)
            ret = pci_set_consistent_dma_mask(pdev,
                    DMA_BIT_MASK(32));
        /* both attempts failed: */
        if (ret) {
            dev_err(&pdev->dev, "No suitable DMA available\n");
            goto out_pci_disable_device;
        }
    }

    ret = pci_request_regions(pdev, DRV_NAME);
    if (ret) {
        dev_err(&pdev->dev, "pci_request_regions failed\n");
        goto out_pci_disable_device;
    }

    trans_pcie->hw_base = pci_ioremap_bar(pdev, 0);
    if (!trans_pcie->hw_base) {
        dev_err(&pdev->dev, "pci_ioremap_bar failed\n");
        ret = -ENODEV;
        goto out_pci_release_regions;
    }

    /* We disable the RETRY_TIMEOUT register (0x41) to keep
     * PCI Tx retries from interfering with C3 CPU state */
    pci_write_config_byte(pdev, PCI_CFG_RETRY_TIMEOUT, 0x00);

    trans->dev = &pdev->dev;
    trans_pcie->pci_dev = pdev;
    iwl_disable_interrupts(trans);

    ret = pci_enable_msi(pdev);
    if (ret) {
        dev_err(&pdev->dev, "pci_enable_msi failed(0X%x)\n", ret);
        /* enable rfkill interrupt: hw bug w/a */
        pci_read_config_word(pdev, PCI_COMMAND, &pci_cmd);
        if (pci_cmd & PCI_COMMAND_INTX_DISABLE) {
            pci_cmd &= ~PCI_COMMAND_INTX_DISABLE;
            pci_write_config_word(pdev, PCI_COMMAND, pci_cmd);
        }
    }

    trans->hw_rev = iwl_read32(trans, CSR_HW_REV);
    /*
     * In the 8000 HW family the format of the 4 bytes of CSR_HW_REV have
     * changed, and now the revision step also includes bit 0-1 (no more
     * "dash" value). To keep hw_rev backwards compatible - we'll store it
     * in the old format.
     */
    if (trans->cfg->device_family == IWL_DEVICE_FAMILY_8000) {
        unsigned long flags;

        trans->hw_rev = (trans->hw_rev & 0xfff0) |
                (CSR_HW_REV_STEP(trans->hw_rev << 2) << 2);

        ret = iwl_pcie_prepare_card_hw(trans);
        if (ret) {
            IWL_WARN(trans, "Exit HW not ready\n");
            goto out_pci_disable_msi;
        }

        /*
         * in-order to recognize C step driver should read chip version
         * id located at the AUX bus MISC address space.
         */
        iwl_set_bit(trans, CSR_GP_CNTRL,
                CSR_GP_CNTRL_REG_FLAG_INIT_DONE);
        udelay(2);

        ret = iwl_poll_bit(trans, CSR_GP_CNTRL,
                CSR_GP_CNTRL_REG_FLAG_MAC_CLOCK_READY,
                CSR_GP_CNTRL_REG_FLAG_MAC_CLOCK_READY,
                25000);
        if (ret < 0) {
            IWL_DEBUG_INFO(trans, "Failed to wake up the nic\n");
            goto out_pci_disable_msi;
        }

        if (iwl_trans_grab_nic_access(trans, false, &flags)) {
            u32 hw_step;

            hw_step = __iwl_read_prph(trans, WFPM_CTRL_REG);
            hw_step |= ENABLE_WFPM;
            __iwl_write_prph(trans, WFPM_CTRL_REG, hw_step);
            hw_step = __iwl_read_prph(trans, AUX_MISC_REG);
            hw_step = (hw_step >> HW_STEP_LOCATION_BITS) & 0xF;
            if (hw_step == 0x3)
                trans->hw_rev = (trans->hw_rev & 0xFFFFFFF3) |
                        (SILICON_C_STEP << 2);
            iwl_trans_release_nic_access(trans, &flags);
        }
    }

    trans->hw_id = (pdev->device << 16) + pdev->subsystem_device;
    snprintf(trans->hw_id_str, sizeof(trans->hw_id_str),
            "PCI ID: 0x%04X:0x%04X", pdev->device, pdev->subsystem_device);

    /* Initialize the wait queue for commands */
    init_waitqueue_head(&trans_pcie->wait_command_queue);

    ret = iwl_pcie_alloc_ict(trans);
    if (ret)
        goto out_pci_disable_msi;

    ret = request_threaded_irq(pdev->irq, iwl_pcie_isr,
            iwl_pcie_irq_handler,
            IRQF_SHARED, DRV_NAME, trans);
    if (ret) {
        IWL_ERR(trans, "Error allocating IRQ %d\n", pdev->irq);
        goto out_free_ict;
    }

    trans_pcie->inta_mask = CSR_INI_SET_MASK;
    trans->d0i3_mode = IWL_D0I3_MODE_ON_SUSPEND;
#endif //DISABLED_CODE
    return trans;
#if DISABLED_CODE
    out_free_ict:
    iwl_pcie_free_ict(trans);
    out_pci_disable_msi:
    pci_disable_msi(pdev);
    out_pci_release_regions:
    pci_release_regions(pdev);
    out_pci_disable_device:
    pci_disable_device(pdev);
    out_no_pci:
    iwl_trans_free(trans);
    return ERR_PTR(ret);
#endif // DISABLED_CODE
}
