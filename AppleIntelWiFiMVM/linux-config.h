/**
 * These are the Linux config flags I've noticed in poking around the code
 */

#define __ARG_PLACEHOLDER_1 0,
#define config_enabled(cfg) _config_enabled(cfg)
#define _config_enabled(value) __config_enabled(__ARG_PLACEHOLDER_##value)
#define __config_enabled(arg1_or_junk) ___config_enabled(arg1_or_junk 1, 0)
#define ___config_enabled(__ignored, val, ...) val
#define IS_ENABLED(option) config_enabled(option)

// ------------ LINUX CORE OPTIONS -------------
#define CONFIG_ACPI
#define __UAPI_DEF_IN6_ADDR
//#define CONFIG_IPV6
//#define CONFIG_PM_SLEEP


// ------------ DRIVER OPTIONS -------------
//#define CONFIG_IWLDVM
#define CONFIG_IWLMVM
//#define CONFIG_IWLWIFI_DEBUG
//#define CONFIG_IWLWIFI_DEBUGFS
//#define CONFIG_IWLWIFI_BCAST_FILTERING
//#define CONFIG_IWLWIFI_LEDS
//#define CONFIG_IWLWIFI_OPMODE_MODULAR
//#define CONFIG_NL80211_TESTMODE
