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
        cout << "[Msg Arrived]: " << endl;
        nng_msg *msg = reinterpret_cast<nng_msg *>(rmsg);
        uint32_t topicsz, payloadsz;
        char *topic = (char *)nng_mqtt_msg_get_publish_topic(msg, &topicsz);
        char *payload = (char *)nng_mqtt_msg_get_publish_payload(msg, &payloadsz);
        printf("\ttopic  => %.*s\n", topicsz, topic);
        printf("\tpayload => %.*s\n", payloadsz, payload);

        property *p = nng_mqtt_msg_get_publish_property(msg);
        property_data *pa = mqtt_property_get_value(p, USER_PROPERTY);
        if (pa)
        {
            mqtt_buf_t *key = &pa->p_value.strpair.key;
            mqtt_buf_t *val = &pa->p_value.strpair.value;
            printf("\tuser property => ");
            printf("%.*s ", key->length, key->buf);
            printf("%.*s\n", val->length, val->buf);
        }

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
    QuicClient client(url, MQTT_PROTOCOL_VERSION_v5);

    Cbs cbs;
    client.set_callbacks(&cbs);

    Property cp;
    cp.append_u32(MAXIMUM_PACKET_SIZE, 120)
        .append_u32(SESSION_EXPIRY_INTERVAL, 120)
        .append_u16(RECEIVE_MAXIMUM, 120)
        .append_u32(MAXIMUM_PACKET_SIZE, 120);
    // .append_u16(TOPIC_ALIAS_MAXIMUM, 120);

    ConnMessage cmsg;
    cmsg.client_id("clientid")
        .clean_session(false)
        .keep_alive(30)
        .proto_version(5)
        .will_topic("abc")
        .will_msg((uint8_t *)"abc", 3)
        .property(cp);

    client.connect(cmsg);

    SubMessage smsg;
    SubMessage::topics topics = {
        {"topic1", 1},
        {"topic2", 1},
    };

    smsg.topic_with_qos(topics);
    client.subscribe(smsg);

    int i = 0;
    while (i < 1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        string payload = "payload" + std::to_string(i);
        PubMessage pmsg;
        i++;
        Property pp;
        pp.append_strpair(USER_PROPERTY, "key", "val");
        pp.append_u16(TOPIC_ALIAS, 10);

        pmsg.dup(false)
            .payload((uint8_t *)payload.c_str(), payload.length())
            .property(pp)
            .topic("topic1")
            .qos(2);
        client.publish(pmsg);
    }

    PubMessage _pmsg;
    Property _pp;
    string _payload = "this is a test for topic alias.";
    _pp.append_strpair(USER_PROPERTY, "key", "val");
    _pp.append_u16(TOPIC_ALIAS, 10);

    _pmsg.dup(false)
        .payload((uint8_t *)_payload.c_str(), _payload.length())
        .property(_pp)
        .qos(2);
    client.publish(_pmsg);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    cerr << "Done." << endl;
    return 0;
}