#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <webnet.h>
#include <wn_module.h>
#include"../my_mqtt_client.h"
#include"../fake_info_data.h"
static void cgi_hello_handler(struct webnet_session* session)
{

     struct webnet_request* request;
     session->request->result_code = 200;
     LOG_D("success get");
     RT_ASSERT(session != RT_NULL);
     request = session->request;
     RT_ASSERT(request != RT_NULL);
     LOG_D("success request");
     cJSON *res = cJSON_CreateObject();;
     LOG_D("success res %s",res);
     if (request->query_counter)
     {
             const char *id,*data;
             id = webnet_request_get_query(request, "id");
             data = webnet_request_get_query(request, "data");
             cJSON_AddItemToObject(res,"id",cJSON_CreateString(id));
             cJSON_AddItemToObject(res,"data",cJSON_CreateString(data));
     }

     mq_publish(cJSON_Print(res));
     webnet_session_printf(session, "success");
     rt_free(&res);
}

void webnet_test(void)
{
#ifdef WEBNET_USING_CGI
    webnet_cgi_register("device/post", cgi_hello_handler);
#endif
    webnet_init();
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(webnet_test, wenbet test);
#endif
