/**
 * These are the Linux config flags I've noticed in poking around the code
 */

#define IS_ENABLED(x) defined(x)

// ------------ LINUX CORE OPTIONS -------------
#define CONFIG_ACPI 1
#define __UAPI_DEF_IN6_ADDR 1
//#define CONFIG_IPV6
//#define CONFIG_PM_SLEEP


// ------------ DRIVER OPTIONS -------------
//#define CONFIG_IWLDVM
#define CONFIG_IWLMVM 1
//#define CONFIG_IWLWIFI_DEBUG
//#define CONFIG_IWLWIFI_DEBUGFS
//#define CONFIG_IWLWIFI_BCAST_FILTERING
//#define CONFIG_IWLWIFI_LEDS
//#define CONFIG_IWLWIFI_OPMODE_MODULAR
//#define CONFIG_NL80211_TESTMODE
