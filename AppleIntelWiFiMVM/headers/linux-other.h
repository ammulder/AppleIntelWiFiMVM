/**
 * Other Linux definitions needed to build this, where for whatever reason importing
 * entire files doesn't seem reasonable.
 *
 * All covered by the Linux kernel license (GPL v2)
 */
#ifndef __LINUX_OTHER_H
#define __LINUX_OTHER_H

#import "linux-porting.h"

// =================== FROM include/linux/pci_ids.h ==================
#define PCI_VENDOR_ID_INTEL             0x8086

// =================== FROM /include/linux/mod_devicetable.h ==================
#define PCI_ANY_ID (~0)
typedef unsigned long kernel_ulong_t;
struct pci_device_id {
    __u32 vendor, device;           /* Vendor and device ID or PCI_ANY_ID*/
    __u32 subvendor, subdevice;     /* Subsystem ID's or PCI_ANY_ID */
    __u32 pci_class, class_mask;        /* (class,subclass,prog-if) triplet */
    kernel_ulong_t driver_data;      /* Data private to the driver */
};

#endif // LINUX_OTHER_H