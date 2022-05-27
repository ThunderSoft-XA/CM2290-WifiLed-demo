
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "mqtt_control.hpp"

using namespace mosqtt;


static const struct option long_options[] = {
    {"host",required_argument,NULL,'h'},
    {"port",required_argument,NULL,'p'},
    {"help",no_argument,NULL,'H'}, 
    {NULL,     0                ,NULL, 0}
};

void help() {
    printf("mqtt-led-client usage:\n \
                    \t --host/-h = 127.0.0.1\n\
                    \t --port/-p = 1883\n\
                    \t --help/-H\n");
}


static MqttClientConfig led_config = {
    .server_ip="10.4.12.90",
    .user_name="",
    .password = "",
    .server_port = 1883,
    .keep_alive = 30,
    .qos = 1,
    .timeout = 10,
    .reconnect_maxcnt = -1,
    .enable_ssl = FALSE
};

//parse params
int parseParams(int argc, char *argv[]) {
    if( 5 > argc) {
        help();
        return -1;
    }
    int opt=0;
	while(-1 != (opt = getopt_long(argc,argv,"h:p:H",long_options,NULL))) {
		//optarg is global
		switch(opt) {
			case 'h':
				led_config.server_ip =  std::string(optarg);
				break;
			case 'p':
				led_config.server_port = atoi(optarg);
				break;
            case 'H':
            default:
                help();
                return -1;
		}
	}
    return 0;
}

int main(int argc, char **argv)
{
    if( -1 == parseParams(argc,argv)) {
        return -1;
    }
    MqttHwClient led_client;
    led_client.config = led_config;

    led_client.Create();

    led_client.Start();

    return 0;
}
