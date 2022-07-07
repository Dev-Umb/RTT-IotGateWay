/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-05     Umb       the first version
 */
#ifndef APPLICATIONS_FAKE_INFO_DATA_H_
#define APPLICATIONS_FAKE_INFO_DATA_H_
// 创造用于测试的假数据


#include <string.h>
#include"cjson.h"
#include <stdlib.h>
#include <time.h>
#define SENSOR "SENSOR"
char* T = "36";
char* H = "2%";
char* windySpeed = "50";
int getId(){
    srand((unsigned)time(NULL));
    int a = rand();
    return a;
}
cJSON *getJson(int id,char* data){
    cJSON *cvalue = cJSON_CreateObject();
    cJSON *cid= cJSON_CreateNumber(id);
    cJSON_AddItemToObject(cvalue, "id", cid);
    cJSON_AddItemToObject(cvalue, "data", cJSON_CreateString(data));
    return cvalue;
}
cJSON *getTemperature(){
//    cJSON *tem = cJSON_CreateObject();
//    cJSON_AddItemToObject(tem,"data",cJSON_CreateString(T));
//    cJSON *res = getJson(SENSOR, getId(), "Temperature", tem);

    return getJson(getId(), T);
}

cJSON* getHumidity(){
//       cJSON *tem = cJSON_CreateObject();
//       cJSON_AddItemToObject(tem,"type",cJSON_CreateString("湿度传感器"));
//       cJSON_AddItemToObject(tem,"data",cJSON_CreateString(H));
//       cJSON *res = getJson(SENSOR, getId(), "Humidity", tem);
       return getJson(getId(), H);;
}



#endif /* APPLICATIONS_FAKE_INFO_DATA_H_ */
