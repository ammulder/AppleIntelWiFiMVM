/******************************************************************************
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2007 - 2014 Intel Corporation. All rights reserved.
 * Copyright(c) 2013 - 2015 Intel Mobile Communications GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110,
 * USA
 *
 * The full GNU General Public License is included in this distribution
 * in the file called COPYING.
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 * BSD LICENSE
 *
 * Copyright(c) 2005 - 2014 Intel Corporation. All rights reserved.
 * Copyright(c) 2013 - 2015 Intel Mobile Communications GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

/** See iwlwifi/pcie/drv.c */

#ifndef __MVM_DEVICE_LIST_H
#define __MVM_DEVICE_LIST_H

#include "iwl-7000.h"
#include "iwl-8000.h"

#define IWL_PCI_DEVICE(dev, subdev, cfg) \
.device = (dev), \
.subdevice = (subdev), \
.config = &(cfg)

struct wifi_card {
    UInt16 device;
    UInt16 subdevice;
    const struct iwl_cfg *config;
};

static const struct wifi_card wifi_card_ids[] = {
    /* 7260 Series */
    {IWL_PCI_DEVICE(0x08B1, 0x4070, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4072, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4170, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4C60, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4C70, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4060, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x406A, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4160, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4062, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4162, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x4270, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x4272, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x4260, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x426A, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x4262, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4470, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4472, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4460, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x446A, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4462, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4870, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x486E, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4A70, iwl7260_2ac_cfg_high_temp)},
    {IWL_PCI_DEVICE(0x08B1, 0x4A6E, iwl7260_2ac_cfg_high_temp)},
    {IWL_PCI_DEVICE(0x08B1, 0x4A6C, iwl7260_2ac_cfg_high_temp)},
    {IWL_PCI_DEVICE(0x08B1, 0x4570, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4560, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x4370, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x4360, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x5070, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x5072, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x5170, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x5770, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4020, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x402A, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0x4220, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0x4420, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC070, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC072, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC170, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC060, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC06A, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC160, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC062, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC162, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC770, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC760, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0xC270, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xCC70, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xCC60, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0xC272, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0xC260, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0xC26A, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0xC262, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC470, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC472, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC460, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC462, iwl7260_n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC570, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC560, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0xC370, iwl7260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC360, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC020, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC02A, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B2, 0xC220, iwl7260_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B1, 0xC420, iwl7260_2n_cfg)},
    
    /* 3160 Series */
    {IWL_PCI_DEVICE(0x08B3, 0x0070, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x0072, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x0170, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x0172, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x0060, iwl3160_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x0062, iwl3160_n_cfg)},
    {IWL_PCI_DEVICE(0x08B4, 0x0270, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B4, 0x0272, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x0470, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x0472, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B4, 0x0370, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8070, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8072, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8170, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8172, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8060, iwl3160_2n_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8062, iwl3160_n_cfg)},
    {IWL_PCI_DEVICE(0x08B4, 0x8270, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B4, 0x8370, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B4, 0x8272, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8470, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x8570, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x1070, iwl3160_2ac_cfg)},
    {IWL_PCI_DEVICE(0x08B3, 0x1170, iwl3160_2ac_cfg)},
    
    /* 3165 Series */
    {IWL_PCI_DEVICE(0x3165, 0x4010, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3165, 0x4012, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3166, 0x4212, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3165, 0x4410, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3165, 0x4510, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3165, 0x4110, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3166, 0x4310, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3166, 0x4210, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3165, 0x8010, iwl3165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x3165, 0x8110, iwl3165_2ac_cfg)},
    
    /* 7265 Series */
    {IWL_PCI_DEVICE(0x095A, 0x5010, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5110, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5100, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x5310, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x5302, iwl7265_n_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x5210, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5012, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5412, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5410, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5510, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5400, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x1010, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5000, iwl7265_2n_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x500A, iwl7265_2n_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x5200, iwl7265_2n_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5002, iwl7265_n_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5102, iwl7265_n_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x5202, iwl7265_n_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9010, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9012, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x900A, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9110, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9112, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9210, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x9200, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9510, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9310, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9410, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5020, iwl7265_2n_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x502A, iwl7265_2n_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5420, iwl7265_2n_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5090, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5190, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5590, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x5290, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5490, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x5F10, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x5212, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095B, 0x520A, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9000, iwl7265_2ac_cfg)},
    {IWL_PCI_DEVICE(0x095A, 0x9400, iwl7265_2ac_cfg)},
    
    /* 8000 Series */
    {IWL_PCI_DEVICE(0x24F3, 0x0010, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x1010, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0130, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x1130, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0132, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x1132, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0110, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x01F0, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0012, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x1012, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x1110, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0050, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0250, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x1050, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0150, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x1150, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F4, 0x0030, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F4, 0x1030, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0xC010, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0xC110, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0xD010, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0xC050, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0xD050, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x8010, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x8110, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x9010, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x9110, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F4, 0x8030, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F4, 0x9030, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x8130, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x9130, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x8132, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x9132, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x8050, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x8150, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x9050, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x9150, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0004, iwl8260_2n_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0044, iwl8260_2n_cfg)},
    {IWL_PCI_DEVICE(0x24F5, 0x0010, iwl4165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F6, 0x0030, iwl4165_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0810, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0910, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0850, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0950, iwl8260_2ac_cfg)},
    {IWL_PCI_DEVICE(0x24F3, 0x0930, iwl8260_2ac_cfg)}
};

#endif