#ifndef _TS_MQTT_CONTROLLER_H
#define _TS_MQTT_CONTROLLER_H

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <mosquitto.h>

#define FALSE false
#define TRUE true

namespace mosqtt {

/**
 * @brief  sotre mqtt client configure,such as , server ip/ username/ password/ .etc.
 * 
 */
typedef struct _MqttClientConfig
{
    std::string server_ip;
    std::string user_name;
    std::string password;
    int         server_port;
    int         keep_alive; 
    int         qos;     // Quality of Service
    int         timeout; // unit:ms
    int         reconnect_maxcnt;
    bool    enable_ssl;
} MqttClientConfig;

/**
 * @brief mqtt client controller class for init,connect/disconnect,receive and response message, 
 * 
 */
class MqttHwClient
{
private:
	std::vector<std::string> topic_set ;

public:
    bool lib_inited;
    bool running;
    bool exited;
    bool connected;
    int  reconnect_count;

    struct mosquitto* mosquitto;
    MqttClientConfig config;

public:
	/**
	 * @brief Construct a new Mqtt Hw Client object
	 * 
	 */
    MqttHwClient (void);
    bool Create(void);
    bool Start (void);

	inline uint getTopicSize() {
		return topic_set.size();
	}

	inline std::string getTopicName(uint topic_index) {
		return topic_set[topic_index];
	}

	/**
	 * @brief Subscribe to one/more topic immediately when the plug-in is formed and
	 * can only be unsubscribed when the plug-in is unsubscribed
	 * 
	 * @param topic 
	 * @return int 
	 */
	int subscribeTopic(std::string topic);

   ~MqttHwClient(void);
};

}

#endif //_TS_MQTT_CONTROLLER_H
