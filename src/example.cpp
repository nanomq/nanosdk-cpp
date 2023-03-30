#include "client.h"
#include <iostream>
#include <thread>
using namespace std;

class Cbs : public virtual Callbacks
{
public:
    int client_connected(void *rmsg, void *arg) override
    {
        cout << "[Connected]...\n";
        return 0;
    }
    int client_disconnected(void *rmsg, void *arg) override
    {
        cout << "[Disconnected]...\n";
        return 0;
    }
    int message_received(void *rmsg, void *arg) override
    {
        nng_msg *msg = reinterpret_cast<nng_msg *>(rmsg);
        uint32_t topicsz, payloadsz;
        char *topic = (char *)nng_mqtt_msg_get_publish_topic(msg, &topicsz);
        char *payload = (char *)nng_mqtt_msg_get_publish_payload(msg, &payloadsz);
        cout << "[Msg Arrived]: ";
        printf("topic  => %.*s\npayload => %.*s\n", topicsz, topic, payloadsz, payload);
        return 0;
    }
    int message_send(void *rmsg, void *arg) override
    {
        cout << "[Msg Sent]...\n";
        return 0;
    }
};

int main()
{
    char url[] = "mqtt-quic://13.49.223.253:14567";
    char topic[] = "topic";
    char payload[] = "payload";
    int keepalive = 30;
    bool clean_session = false;
    Cbs cbs;
    int qos = 1;

    QuicClient client(url);
    client.set_callbacks(&cbs);
    client.connect(keepalive, clean_session);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.subscribe(topic, qos);

    while (true)
    {
        cout << "Publish message" << endl;
        client.publish(topic, payload, 0, true, false);
        cout << "Publish message done" << endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    cerr << "Done." << endl;
    return 0;
}