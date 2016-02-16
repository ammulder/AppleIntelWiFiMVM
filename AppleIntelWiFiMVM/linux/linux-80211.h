//
//  linux-80211.h
//
//  Various definitions from Linux 80211 header files used by the included Intel header files
//
//  Copyright © 2016 Aaron Mulder. All rights reserved.
//

#ifndef linux_80211_h
#define linux_80211_h

/**
 * enum nl80211_band - Frequency band
 * @NL80211_BAND_2GHZ: 2.4 GHz ISM band
 * @NL80211_BAND_5GHZ: around 5 GHz band (4.9 - 5.7 GHz)
 * @NL80211_BAND_60GHZ: around 60 GHz band (58.32 - 64.80 GHz)
 */
enum nl80211_band {
    NL80211_BAND_2GHZ,
    NL80211_BAND_5GHZ,
    NL80211_BAND_60GHZ,
};

/**
 * enum ieee80211_band - supported frequency bands
 *
 * The bands are assigned this way because the supported
 * bitrates differ in these bands.
 *
 * @IEEE80211_BAND_2GHZ: 2.4GHz ISM band
 * @IEEE80211_BAND_5GHZ: around 5GHz band (4.9-5.7)
 * @IEEE80211_BAND_60GHZ: around 60 GHz band (58.32 - 64.80 GHz)
 * @IEEE80211_NUM_BANDS: number of defined bands
 */
enum ieee80211_band {
    IEEE80211_BAND_2GHZ = NL80211_BAND_2GHZ,
    IEEE80211_BAND_5GHZ = NL80211_BAND_5GHZ,
    IEEE80211_BAND_60GHZ = NL80211_BAND_60GHZ,
    
    /* keep last */
    IEEE80211_NUM_BANDS
};


/**
 * enum ieee80211_smps_mode - spatial multiplexing power save mode
 *
 * @IEEE80211_SMPS_AUTOMATIC: automatic
 * @IEEE80211_SMPS_OFF: off
 * @IEEE80211_SMPS_STATIC: static
 * @IEEE80211_SMPS_DYNAMIC: dynamic
 * @IEEE80211_SMPS_NUM_MODES: internal, don't use
 */
enum ieee80211_smps_mode {
    IEEE80211_SMPS_AUTOMATIC,
    IEEE80211_SMPS_OFF,
    IEEE80211_SMPS_STATIC,
    IEEE80211_SMPS_DYNAMIC,
    
    /* keep last */
    IEEE80211_SMPS_NUM_MODES,
};

/**
 * struct ieee80211_cipher_scheme - cipher scheme
 *
 * This structure contains a cipher scheme information defining
 * the secure packet crypto handling.
 *
 * @cipher: a cipher suite selector
 * @iftype: a cipher iftype bit mask indicating an allowed cipher usage
 * @hdr_len: a length of a security header used the cipher
 * @pn_len: a length of a packet number in the security header
 * @pn_off: an offset of pn from the beginning of the security header
 * @key_idx_off: an offset of key index byte in the security header
 * @key_idx_mask: a bit mask of key_idx bits
 * @key_idx_shift: a bit shift needed to get key_idx
 *     key_idx value calculation:
 *      (sec_header_base[key_idx_off] & key_idx_mask) >> key_idx_shift
 * @mic_len: a mic length in bytes
 */
struct ieee80211_cipher_scheme {
    u32 cipher;
    u16 iftype;
    u8 hdr_len;
    u8 pn_len;
    u8 pn_off;
    u8 key_idx_off;
    u8 key_idx_mask;
    u8 key_idx_shift;
    u8 mic_len;
};

/**
 * enum nl80211_iftype - (virtual) interface types
 *
 * @NL80211_IFTYPE_UNSPECIFIED: unspecified type, driver decides
 * @NL80211_IFTYPE_ADHOC: independent BSS member
 * @NL80211_IFTYPE_STATION: managed BSS member
 * @NL80211_IFTYPE_AP: access point
 * @NL80211_IFTYPE_AP_VLAN: VLAN interface for access points; VLAN interfaces
 *      are a bit special in that they must always be tied to a pre-existing
 *      AP type interface.
 * @NL80211_IFTYPE_WDS: wireless distribution interface
 * @NL80211_IFTYPE_MONITOR: monitor interface receiving all frames
 * @NL80211_IFTYPE_MESH_POINT: mesh point
 * @NL80211_IFTYPE_P2P_CLIENT: P2P client
 * @NL80211_IFTYPE_P2P_GO: P2P group owner
 * @NL80211_IFTYPE_P2P_DEVICE: P2P device interface type, this is not a netdev
 *      and therefore can't be created in the normal ways, use the
 *      %NL80211_CMD_START_P2P_DEVICE and %NL80211_CMD_STOP_P2P_DEVICE
 *      commands to create and destroy one
 * @NL80211_IF_TYPE_OCB: Outside Context of a BSS
 *      This mode corresponds to the MIB variable dot11OCBActivated=true
 * @NL80211_IFTYPE_MAX: highest interface type number currently defined
 * @NUM_NL80211_IFTYPES: number of defined interface types
 *
 * These values are used with the %NL80211_ATTR_IFTYPE
 * to set the type of an interface.
 *
 */
enum nl80211_iftype {
    NL80211_IFTYPE_UNSPECIFIED,
    NL80211_IFTYPE_ADHOC,
    NL80211_IFTYPE_STATION,
    NL80211_IFTYPE_AP,
    NL80211_IFTYPE_AP_VLAN,
    NL80211_IFTYPE_WDS,
    NL80211_IFTYPE_MONITOR,
    NL80211_IFTYPE_MESH_POINT,
    NL80211_IFTYPE_P2P_CLIENT,
    NL80211_IFTYPE_P2P_GO,
    NL80211_IFTYPE_P2P_DEVICE,
    NL80211_IFTYPE_OCB,
    
    /* keep last */
    NUM_NL80211_IFTYPES,
    NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};

#endif /* linux_80211_h */
