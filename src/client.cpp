// Copyright (C) 2014-2019 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <thread>
#include <cstdarg>
#include <cstring>

using namespace std;

#ifndef _WIN32
#include <unistd.h>
#endif

#include <nng/nng.h>
#include <nng/supplemental/tls/tls.h>
#include <nng/supplemental/util/options.h>
#include <nng/supplemental/util/platform.h>
#include "nng/mqtt/mqtt_client.h"

static const char *sub_topic = "topic/sub";
static const char *pub_topic = "topic/pub";
static const char *address = "mqtt-tcp://localhost:1885";
static const char *username = "username";
static const char *password = "passwd";
static const char *clientid = "vsomeip_gateway";
static bool clean_start = true;
static int sub_qos = 0;
static int proto_ver = 4;
static int keepalive = 60;
static int parallel = 10;
static nng_socket *gsock = NULL;


typedef enum { INIT, RECV, WAIT, SEND } work_state;

struct work {
	work_state state;
	nng_aio   *aio;
	nng_msg   *msg;
	nng_ctx    ctx;
};


static void
fatal(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

void
nng_fatal(const char *msg, int rv)
{
	fatal("%s: %s\n", msg, nng_strerror(rv));
}


struct work *
alloc_work(nng_socket sock, void cb(void *))
{
	struct work *w;
	int          rv;

	if ((w = (work*) nng_alloc(sizeof(*w))) == NULL) {
		nng_fatal("nng_alloc", NNG_ENOMEM);
	}
	if ((rv = nng_aio_alloc(&w->aio, cb, w)) != 0) {
		nng_fatal("nng_aio_alloc", rv);
	}
	if ((rv = nng_ctx_open(&w->ctx, sock)) != 0) {
		nng_fatal("nng_ctx_open", rv);
	}
	w->state = INIT;
	return (w);
}

int
client_publish(nng_socket sock, const char *topic, uint8_t *payload,
    uint32_t payload_len, uint8_t qos, bool verbose)
{
	cout << "Send publish: '" << payload << "' to '" << topic << "'";
	int rv;

	// create a PUBLISH message
	nng_msg *pubmsg;
	nng_mqtt_msg_alloc(&pubmsg, 0);
	nng_mqtt_msg_set_packet_type(pubmsg, NNG_MQTT_PUBLISH);
	nng_mqtt_msg_set_publish_dup(pubmsg, 0);
	nng_mqtt_msg_set_publish_qos(pubmsg, qos);
	nng_mqtt_msg_set_publish_retain(pubmsg, 0);
	nng_mqtt_msg_set_publish_payload(
	    pubmsg, (uint8_t *) payload, payload_len);
	nng_mqtt_msg_set_publish_topic(pubmsg, topic);

	if ((rv = nng_sendmsg(sock, pubmsg, NNG_FLAG_NONBLOCK)) != 0) {
		cerr << "nng_sendmsg" << rv;
	}

	return rv;
}

void
set_sub_topic(nng_mqtt_topic_qos topic_qos[], int qos, const char *topic)
{
	topic_qos[0].qos = qos;
	cout << "topic: " << topic[0];
	topic_qos[0].topic.buf    = (uint8_t *) topic;
	topic_qos[0].topic.length = strlen(topic);
	return;
}

void
disconnect_cb(nng_pipe p, nng_pipe_ev ev, void *arg)
{
	int reason = 0;
	// get connect reason
	nng_pipe_get_int(p, NNG_OPT_MQTT_DISCONNECT_REASON, &reason);
	cout << __FUNCTION__ << ": disconnected RC [" <<  reason <<"] !";
}

void
connect_cb(nng_pipe p, nng_pipe_ev ev, void *arg)
{
	int reason;
	// get connect reason
	nng_pipe_get_int(p, NNG_OPT_MQTT_CONNECT_REASON, &reason);

	cout << __FUNCTION__ << ": connected RC [" << reason << "] !" ;

	nng_socket sock = *(nng_socket *) arg;

	nng_mqtt_topic_qos topic_qos[1];

	set_sub_topic(topic_qos, sub_qos, sub_topic);
	cout << "topic: " << sub_topic;
	 topic_qos[0].qos          = 0;
	 topic_qos[0].topic.buf    = (uint8_t *)sub_topic;
	 topic_qos[0].topic.length = strlen(sub_topic);

	 size_t topic_qos_count =
	     sizeof(topic_qos) / sizeof(nng_mqtt_topic_qos);

	 nng_msg *msg;
	 nng_mqtt_msg_alloc(&msg, 0);
	 nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_SUBSCRIBE);
	 nng_mqtt_msg_set_subscribe_topics(msg, topic_qos, topic_qos_count);

	 // Send subscribe message
	 int rv = 0;
	 rv     = nng_sendmsg(sock, msg, NNG_FLAG_NONBLOCK);
	 if (rv != 0) {
	 	cerr << "nng_sendmsg" << rv;
	 }
}

int
check_recv(nng_msg *msg)
{

	// Get PUBLISH payload and topic from msg;
	uint32_t p_len;
	uint32_t t_len;

	uint8_t    *p = nng_mqtt_msg_get_publish_payload(msg, &p_len);
	const char *t = nng_mqtt_msg_get_publish_topic(msg, &t_len);

	// TODO replacement
	std::string payload(p, p + p_len);
	std::string topic(t, t + t_len);
	cout << "Recv message: '" << payload << "' from '" << topic << "'";

	nng_msg_free(msg);

	return 0;
}

void
vsomeip_gateway_sub_cb(void *arg)
{
	struct work *work = reinterpret_cast<struct work *>(arg);
	nng_msg     *msg;
	int          rv;

	switch (work->state) {
	case INIT:
		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;
	case RECV:
		if ((rv = nng_aio_result(work->aio)) != 0) {
			// nng_msg_free(work->msg);
			cerr << "nng_send_aio" << rv;
		}
		msg = nng_aio_get_msg(work->aio);

		if (-1 == check_recv(msg)) {
			abort();
		}

		work->state = RECV;
		nng_ctx_recv(work->ctx, work->aio);
		break;
	default:
		cerr << "bad state!" << NNG_ESTATE ;
		break;
	}
}

struct work *
proxy_alloc_work(nng_socket sock)
{
	struct work *w;
	int          rv;

	if ((w = reinterpret_cast<struct work *>(nng_alloc(sizeof(*w)))) ==
	    NULL) {
		cerr << "nng_alloc" << NNG_ENOMEM;
	}
	if ((rv = nng_aio_alloc(&w->aio, vsomeip_gateway_sub_cb, w)) != 0) {
		cerr << "nng_aio_alloc" << rv;
	}
	if ((rv = nng_ctx_open(&w->ctx, sock)) != 0) {
		cerr << "nng_ctx_open" << rv;
	}
	w->state = INIT;
	return (w);
}

int
client(const char *url, nng_socket *sock_ret)
{
	nng_socket   sock;
	nng_dialer   dialer;
	int          rv;
	struct work *works[parallel];

	if ((rv = nng_mqtt_client_open(&sock)) != 0) {
		cerr << "nng_socket" << rv;
		return rv;
	}

	*sock_ret = sock;

	for (int i = 0; i < parallel; i++) {
		works[i] = proxy_alloc_work(sock);
	}

	// Mqtt connect message
	nng_msg *msg;
	nng_mqtt_msg_alloc(&msg, 0);
	nng_mqtt_msg_set_packet_type(msg, NNG_MQTT_CONNECT);
	nng_mqtt_msg_set_connect_proto_version(msg, proto_ver);
	nng_mqtt_msg_set_connect_keep_alive(msg, keepalive);
	nng_mqtt_msg_set_connect_clean_session(msg, clean_start);
	if (username) {
		nng_mqtt_msg_set_connect_user_name(msg, username);
	}

	if (password) {
		nng_mqtt_msg_set_connect_password(msg, password);
	}

	if (clientid) {
		nng_mqtt_msg_set_connect_client_id(msg, clientid);
	}

	nng_mqtt_set_connect_cb(sock, connect_cb, sock_ret);
	nng_mqtt_set_disconnect_cb(sock, disconnect_cb, NULL);

	if ((rv = nng_dialer_create(&dialer, sock, url)) != 0) {
		cerr << "nng_dialer_create" << rv;
	}

	nng_dialer_set_ptr(dialer, NNG_OPT_MQTT_CONNMSG, msg);
	nng_dialer_start(dialer, NNG_FLAG_NONBLOCK);

	for (int i = 0; i < parallel; i++) {
		vsomeip_gateway_sub_cb(works[i]);
	}

	return 0;
}



int main() {

	nng_socket sock;
	client(address, &sock);
	gsock = &sock;

    while (true) {

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}