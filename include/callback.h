#ifndef MQTT_CALLBACK_H
#define MQTT_CALLBACK_H

class Callbacks
{
public:
	/**
	 * Virtual destructor.
	 */
	virtual ~Callbacks() {}

	/**
	 * This method is called when the client is connected.
	 */
	virtual int client_connected(void *rmsg, void *arg) { return 0; }
	/**
	 * This method is called when the connection to the server is disconnected.
	 */
	virtual int client_disconnected(void *rmsg, void *arg) { return 0; }
	/**
	 * This method is called when a message arrives from the server.
	 */
	virtual int message_received(void *rmsg, void *arg) { return 0; }
	/**
	 * Called when delivery for a message has been completed, and all
	 * acknowledgments have been received.
	 */
	virtual int message_send(void *rmsg, void *arg) { return 0;}
};
#endif
