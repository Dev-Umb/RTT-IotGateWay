/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-01     ZeroFree     first implementation
 */


#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
//#include "my_mqtt_client.h"
#include "sever/sever.h"
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG

/**
 * MQTT URI farmat:
 * domain mode
 * tcp://iot.eclipse.org:1883
 *
 * ipv4 mode
 * tcp://192.168.10.1:1883
 * ssl://192.168.10.1:1884
 *
 * ipv6 mode
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * ssl://[fe80::20c:29ff:fe9a:a07e]:1884
 */
void start(void){
    mq_start();
    webnet_test();
}

int main(void)
{
    rt_wlan_init();
    /* 注册 wlan 回调函数 */
    rt_wlan_connect(SSID, PWD);
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, (void (*)(int, struct rt_wlan_buff *, void *))start, RT_NULL);
    /* 初始化自动连接功能 */
    rt_wlan_config_autoreconnect(1);
    /* 使能 wlan 自动连接 */
    rt_wlan_config_autoreconnect(RT_TRUE);

}
