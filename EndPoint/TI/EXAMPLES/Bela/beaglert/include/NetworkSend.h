//scope.cpp
#ifndef SCOPE_H_
#define SCOPE_H_

#ifdef USE_JUCE
#include <JuceHeader.h>
#else
#include <BeagleRT.h> 
#include <rtdk.h>
#include <cmath>
#include <UdpClient.h>
#include <vector>
#include <string>
extern int gShouldStop;
#endif /* USE_JUCE */

#define NETWORK_AUDIO_BUFFER_SIZE 302
#define UDP_BUFFER_HEADER_CHANNEL_INDEX 0
#define UDP_BUFFER_HEADER_TIMESTAMP_INDEX 1
#define UDP_BUFFER_HEADER_LENGTH 2

struct NetworkBuffer{
	int channelNumber;
	int numBuffers;
	int writeBuffer;
	int readBuffer;
	int writePointer;
	float** buffers;
	bool* doneOnTime;
	bool* readyToBeSent;
	bool enabled;
	int sampleCount;
	static const int bufferLength=NETWORK_AUDIO_BUFFER_SIZE;
	static const int headerLength=UDP_BUFFER_HEADER_LENGTH;
	static const int headerChannelIndex=UDP_BUFFER_HEADER_CHANNEL_INDEX;
	static const int headerTimestampIndex=UDP_BUFFER_HEADER_TIMESTAMP_INDEX;
};

#ifdef USE_JUCE
class NetworkSend: public Thread {
#else
class NetworkSend {
#endif /* USE_JUCE */
	float sampleRate;
#ifdef USE_JUCE
	DatagramSocket udpClient;
	int sleepTimeMs;
	String remoteHostname;
	int remotePortNumber;
#else
	UdpClient udpClient;
	bool isThreadRunning();
	static int sleepTimeMs;
	static bool threadShouldExit();
	static bool threadIsExiting;
	static bool threadRunning;
	static bool staticConstructed;
	static void staticConstructor();
	static AuxiliaryTask sendDataTask; //TODO: allow different AuxiliaryTasks for different priorities (e.g.: audio vs scope)
	static std::vector<NetworkSend *> objAddrs;
#endif /* USE_JUCE */
	void dealloc();
public:
	NetworkBuffer channel;
#ifdef USE_JUCE
	NetworkSend(const String &threadName);
#else
	NetworkSend();
#endif
	~NetworkSend();
	void setup(float aSampleRate, int blockSize, int aChannelNumber, int aPort, const char *aServer);
	void cleanup();
	void sendData();
	void log(float value);
	void setPort(int aPort);
	void setServer(const char* aServer);
	void setChannelNumber(int aChannelNumber);
	int getChannelNumber();
	int getTimestamp();
#ifdef USE_JUCE
	void run();
#else
	static int getNumInstances();
	static void sendAllData();
    static void startThread();
    static void stopThread();
	static void run();
#endif /* USE_JUCE */
};

#ifdef USE_JUCE
#else
/**
 * An array of NetworkSend objects with some default parameters
 *
 * All sending on the same port (defaults to 9999)
 * All sending to the same server (defaults to 127.0.0.1)
*/
class Scope {
	std::vector<NetworkSend> channels;
	void deallocate();
public:
	Scope(int aNumChannels);
	~Scope();
	void log(int channel, float value);
    void setup();
    void setup(float sampleRate, int aPort, const char* aServer);
    void sendData();
    void setPort(int port);
    void setPort(int channel, int port);
    int getNumChannels();
};
#endif /* USE_JUCE */

#endif /* SCOPE_H */
