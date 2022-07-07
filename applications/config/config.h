/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-07     Umb       the first version
 */
#ifndef APPLICATIONS_CONFIG_CONFIG_H_
#define APPLICATIONS_CONFIG_CONFIG_H_

/** MQTT config*/
#define MQTT_URI "tcp://"
//默认的订阅主题
#define MQTT_DEFAULT_SUBTOPIC ""
//遗言推送主题
#define MQTT_WILL_PUBTOPIC ""
static rt_thread_t tid1 = RT_NULL;

//用户专属的订阅主题，用来收取用户发来的检测信息
#define MQTT_SUBTOPIC ""
//用户专属的发布主题，用来将检测的结果发布到指定主题，供医生查看
#define MQTT_PUBTOPIC ""
#define MQTT_WILLMSG  ""
#define MQTT_USERNAME ""                        //代理服务器用户名
#define MQTT_PASSWORD ""                    //代理服务器密码

/** wifi config*/
#define SSID ""
#define PWD ""




#endif /* APPLICATIONS_CONFIG_CONFIG_H_ */
