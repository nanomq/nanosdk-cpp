#include "client.h"
#include "message.h"
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
    QuicClient client(url);

    Cbs cbs;
    client.set_callbacks(&cbs);

    ConnMessage cmsg;
    cmsg.client_id("clientid")
        .clean_session(false)
        .keep_alive(30);

    client.connect(cmsg);

    SubMessage smsg;
    SubMessage::topics topics = {
        {"topic1", 1},
        {"topic2", 1},
    };

    smsg.topic_with_qos(topics);
    client.subscribe(smsg);

    while (true)
    {
        PubMessage pmsg;
        pmsg.dup(false)
            .payload((uint8_t *)"aaaaa", strlen("aaaaa"))
            .topic("topic1")
            .retain(true)
            .qos(1);
        client.publish(pmsg);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    cerr << "Done." << endl;
    return 0;
}