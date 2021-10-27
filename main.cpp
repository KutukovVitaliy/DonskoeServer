#include <iostream>
#include "mqtt/async_client.h"
#include "MqttModbus.h"

const std::string SERVER_ADDRESS("tcp://194.8.130.147:1883");
const std::string CLIENT_ID("paho_cpp_async_subcribe");
const std::string TOPIC_S("TestTopic/question");
const std::string TOPIC_A("TestTopic/answer");
const std::string USERNAME("kvit");
const std::string PASSWORD("20281174");

const int	QOS = 1;
const int	N_RETRY_ATTEMPTS = 5;

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
