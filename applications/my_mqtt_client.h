
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include "paho_mqtt.h"
#include <wlan_mgnt.h>
#include <rtdbg.h>
#include "./config/config.h"
#include "fake_info_data.h"
// wifi join ziroom203 ziroom4001001111
/* define MQTT client context */
static MQTTClient client;
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       4096
#define THREAD_TIMESLICE        5
static void mq_start(void);
static void mq_publish_log(const char *send_str,const char* log);
static void mq_publish(const char *send_str);
char sup_pub_topic[48] = {0};
static int is_started = 0;
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
//    LOG_D("Topic: %.*s receive a message:%.*s",
//          msg_data->topicName->lenstring.len,
//          msg_data->topicName->lenstring.data,
//          msg_data->message->payloadlen,
//          (char *)msg_data->message->payload);

    return;
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
    return;
}
/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
    LOG_D("thread publish start!");
    rt_uint32_t count = 0;

    while (1)
    {
        /* 线程1采用低优先级运行，一直打印计数值 */
        if(is_started){
            cJSON *res = getTemperature();
            char* log = cJSON_GetObjectItemCaseSensitive(res,"name")->valuestring;
            mq_publish_log(cJSON_Print(res),log);
            rt_free(log);
            rt_free(res);
        }

        rt_thread_mdelay(2000);
    }
}
static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_I("Start to connect mqtt server");

}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("Connect mqtt server success");
    LOG_D("Publish message: Hello,RT-Thread! to topic: %s", sup_pub_topic);
    mq_publish("Hello,RT-Thread!");
    LOG_D("start thread");
    tid1 = rt_thread_create("thread1",thread1_entry, RT_NULL,THREAD_STACK_SIZE,THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
            rt_thread_startup(tid1);
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_I("Disconnect from mqtt server");
}

/* 创建与配置 mqtt 客户端 */
static void mq_start(void)
{
    /* 初始 condata 参数 */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = {0};


    if (is_started)
    {
        return;
    }
    /* 配置 MQTT 文本参数 */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* 生成随机客户端 ID */
        rt_snprintf(cid, sizeof(cid), "/%d", rt_tick_get());
        rt_snprintf(sup_pub_topic, sizeof(sup_pub_topic), "%s/%s", MQTT_PUBTOPIC, "1");
        /* 配置连接参数 */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 60;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* 配置 mqtt 参数 */
        client.condata.willFlag = 0;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = sup_pub_topic;

        client.buf_size = client.readbuf_size = 1024;
        client.buf = malloc(client.buf_size);
        client.readbuf = malloc(client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            goto _exit;
        }

        /* 设置事件回调 */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;
        /* 设置要订阅的 topic 和 topic 对应的回调函数 */
        client.messageHandlers[0].topicFilter = sup_pub_topic;
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* 设置默认订阅回调函数 */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    /* 启动 MQTT 客户端 */
    LOG_D("Start mqtt client and subscribe topic:%s", sup_pub_topic);
    paho_mqtt_start(&client);
    is_started = 1;

_exit:
    return;
}

/* MQTT 消息发布函数 */
static void mq_publish_log(const char *send_str,const char* log)
{
    MQTTMessage message;

    const char *msg_str = send_str;
    const char *topic = sup_pub_topic;
    message.qos = QOS1;
    message.retained = 0;
    message.payload = (void *)msg_str;
    message.payloadlen = strlen(message.payload);

    MQTTPublish(&client, topic, &message);

    LOG_D("publish message:%s",log);

    return;
}
static void mq_publish(const char *send_str)
{
    MQTTMessage message;

    const char *msg_str = send_str;
    const char *topic = sup_pub_topic;
    message.qos = QOS1;
    message.retained = 0;
    message.payload = (void *)msg_str;
    message.payloadlen = strlen(message.payload);

    MQTTPublish(&client, topic, &message);
    rt_free(&message);
    LOG_D("publish message:%s",send_str);

    return;
}

