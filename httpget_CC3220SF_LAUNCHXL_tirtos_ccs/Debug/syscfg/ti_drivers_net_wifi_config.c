/*
 *  ======== ti_drivers_net_wifi_Config.c ========
 *  Configured SimpleLink Wifi module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC32XX
 *  by the SysConfig tool.
 */

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>

/*
 *  =============================== SimpleLink Wifi ===============================
 */

/*
 *  ======== SimpleLinkWifi_config ========
 */
const SlWifiCC32XXConfig_t SimpleLinkWifiCC32XX_config = {

        .Mode = ROLE_STA,
        .Ipv4Mode = SL_NETCFG_IPV4_STA_ADDR_MODE,
        .ConnectionPolicy = SL_WLAN_CONNECTION_POLICY(1,0,0,0),
        .PMPolicy = SL_WLAN_NORMAL_POLICY,
        .MaxSleepTimeMS = 0,
        .ScanPolicy = SL_WLAN_SCAN_POLICY(0,0),
        .ScanIntervalInSeconds = 0,
        .Ipv4Config = SL_NETCFG_ADDR_STATIC,
        .Ipv4 = (_u32)SL_IPV4_VAL(192,168,137,201),
        .IpMask = (_u32)SL_IPV4_VAL(255,255,255,0),
        .IpGateway = (_u32)SL_IPV4_VAL(192,168,137,1),
        .IpDnsServer = (_u32)SL_IPV4_VAL(8,8,8,8),
        .DHCPServer = 0,
        .StartAddress = 0,
        .LastAddress = 0,
        .LeaseTime = 0,
        .ProvisioningStop = 1,
        .DeleteAllProfile = 0
};
