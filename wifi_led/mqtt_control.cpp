/**
 * @file MqttHwClient.cpp
 * @author zhanglei (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-05-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <string>
#include <ostream>
#include <sstream>
#include <fstream>

//
// sys headers
//
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// net
#include <netdb.h>
#include <ifaddrs.h>
#include <netinet/in.h>   
#include <arpa/inet.h>

// file IO
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//mqtt header
#include "mqtt_control.hpp"

#define RED_LED_NODE "/sys/class/leds/red/brightness"
#define GREEN_LED_NODE "/sys/class/leds/green/brightness"

namespace mosqtt {
/**
 * @brief  subscribe status topic
 * 
 * @param user_data 
 * @return true 
 * @return false 
 */
static bool MqttStatus(void* user_data)
{
    return true;
}

//
// OnConnected
//
static void OnConnected (
    struct mosquitto* client,
    void* userData,
    int rc)
{
    MqttHwClient* mmc = static_cast<MqttHwClient*> (userData);

    if (rc) {      
        if (mmc->config.reconnect_maxcnt > 0 && ++mmc->reconnect_count >= 
            mmc->config.reconnect_maxcnt) {
            return;
        }

        if (mmc->exited) {
            return;
        }

        if (MOSQ_ERR_SUCCESS != (rc= mosquitto_connect_async (mmc->mosquitto,
            mmc->config.server_ip.c_str(), mmc->config.server_port,
            mmc->config.keep_alive))) {
        }
    } else {
        mmc->reconnect_count = 0;
        mmc->connected = TRUE;

        if (mmc->exited) return;

        mmc->running = TRUE;
        for (int topic_index = 0, topic_size = mmc->getTopicSize(); topic_index < topic_size; topic_index++) {
            if (mosquitto_subscribe(mmc->mosquitto,  NULL, mmc->getTopicName(topic_index).c_str(), mmc->config.qos)){
            }
        }
    }
}

/**
 * @brief callback function is that handle event when mqtt disconnection. Reset MQTT config parmas
 * 
 * @param client 
 * @param userData 
 * @param rc 
 */
static void OnDisconnected (
    struct mosquitto* client,
    void* userData,
    int rc)
{
    MqttHwClient* mmc = static_cast<MqttHwClient*> (userData);

    mmc->reconnect_count = 0;
    mmc->connected = FALSE;
    mmc->running = FALSE;

    if (mmc->exited) return;

    if (MOSQ_ERR_SUCCESS != (rc = mosquitto_connect_async (mmc->mosquitto,
        mmc->config.server_ip.c_str(), mmc->config.server_port,
        mmc->config.keep_alive))) {

    }
}

/**
 * @brief callback function is that handle event when mqtt message arrives
 * 
 * @param mosq 
 * @param userData 
 * @param msg 
 */
void OnMessage(struct mosquitto *mosq, void* userData, const struct mosquitto_message *msg)
{
    MqttHwClient* mmc = (MqttHwClient*) userData;
    //handle the event, different event
    std::cout << "topic :" << msg->topic << ",msg: " << (const char*)msg->payload << std::endl;
    if("topic/LED" == std::string(msg->topic)) {
        if ("led on" == std::string((char*)msg->payload)) {
            int fd;
            char on[] = "255";
            if((fd = open(RED_LED_NODE,O_RDWR | O_NONBLOCK)) != -1) {
                write(fd,on,sizeof(on));
            }
            std::cout << "turn on LED ...." << std::endl;
        } else if ("led off" == std::string((char*)msg->payload)) {
            int fd;
            char off[] = "0";
            if((fd = open(RED_LED_NODE,O_RDWR | O_NONBLOCK)) != -1) {
                write(fd,off,sizeof(off));
            }
            std::cout << "turn off LED ...." << std::endl;
        } else {
            std::cout << "The control commond of led topic is invild" << std::endl;
        }
    } 

    return;
}

/**
 * @brief Construct a new Mqtt Controller:: Mqtt Controller object and finish process of init
 * 
 */
MqttHwClient::MqttHwClient (void)
{
    std::string topic = "topic/LED";
    this->topic_set.push_back(topic);
    lib_inited = FALSE;
    running = FALSE;
    exited = FALSE;
    connected = FALSE;
    reconnect_count = 0;
    this->running = false;
}

/**
 * @brief Destroy the Mqtt Controller:: Mqtt Controller object
 * 
 */
MqttHwClient::~MqttHwClient (void)
{
    this->running = FALSE;
    this->connected = FALSE;
    mosquitto_lib_cleanup();
    this->lib_inited = FALSE;
    this->exited = TRUE;
}

/**
 * @brief mqtt env init and add the callback of connect,disconnect,message
 * 
 * @return true 
 * @return false 
 */
bool MqttHwClient::Create (void)
{
    int rc;

    if (MOSQ_ERR_SUCCESS != (rc = mosquitto_lib_init ())) {
        return FALSE;
    } else {
        this->lib_inited = TRUE;
    }
    this->lib_inited = TRUE;

    if (!(this->mosquitto = mosquitto_new (NULL, TRUE, this))) {
        return FALSE;
    }
    //username and password
    if(!this->config.user_name.empty() && !this->config.password.empty()) {
        if (mosquitto_username_pw_set(this->mosquitto, this->config.user_name.c_str(), this->config.password.c_str())){
            return FALSE;
        }
        std::cout << "mqtt username and password set successfully!!" << std::endl; 
    }

    mosquitto_connect_callback_set(this->mosquitto, OnConnected);
    mosquitto_disconnect_callback_set(this->mosquitto, OnDisconnected);
    mosquitto_message_callback_set(this->mosquitto, OnMessage);
    
    return TRUE;
}

/**
 * @brief build a mqtt connection  on async
 * 
 * @return true 
 * @return false 
 */
bool MqttHwClient::Start (void)
{
    int rc;

    if (MOSQ_ERR_SUCCESS != (rc = mosquitto_connect(this->mosquitto,
        this->config.server_ip.c_str(), this->config.server_port, 
        this->config.keep_alive))) {
            std::cout << "mqtt connection failed !!" << std::endl; 
            return FALSE;
    }
    std::cout << "mqtt connection successfully !!" << std::endl; 
    while (1) {
        rc = mosquitto_loop_start (this->mosquitto);
    }

    return TRUE;
}

} // end of namespace mosquitto