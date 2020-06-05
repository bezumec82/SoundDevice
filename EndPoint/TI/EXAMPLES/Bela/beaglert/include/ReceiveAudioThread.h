#ifndef RECEIVEAUDIOTHREAD_H_INCLUDED
#define RECEIVEAUDIOTHREAD_H_INCLUDED

#ifdef USE_JUCE
#include <JuceHeader.h>
#else
#include <BeagleRT.h>
#include <UdpServer.h>
#include <vector>
#include <iostream>
#include <native/task.h>
#include <native/timer.h>
#include <math.h>

#endif /*USE_JUCE*/

#ifdef USE_JUCE
class ReceiveAudioThread : public Thread {
#else
class ReceiveAudioThread{
#endif /* USE_JUCE */
private:
    //  FILE *fd; //DEBUG
    //  FILE *fd2; //DEBUG
#ifdef USE_JUCE
	DatagramSocket socket;
#else
    UdpServer socket;
#endif /* USE_JUCE */
    bool listening;
    bool bufferReady;
#ifdef USE_JUCE
    bool threadRunning; //do we really need this ?
#else
    static bool threadRunning;
    static bool threadIsExiting;
#endif
    float *buffer;
    float *stackBuffer;
    int bufferLength;
    float readPointer;
    int writePointer;
    int lastValidPointer;
#ifdef USE_JUCE
    int sleepTime;
#else
    static int sleepTime;
#endif
    int waitForSocketTime;
    int payloadLength; //size of the payload of each datagram
    int headerLength; //size of the header of each datagram
    int bytesToRead;
    int threadPriority;
    int channel;
    int timestamp;
    void dealloc();
    void wrapWritePointer();
    void pushPayload(int startIndex);
    void popPayload(int startIndex);
    int readUdpToBuffer();
#ifdef USE_JUCE
#else
    RTIME lastTime; // Used for clock synchronization
    static bool threadShouldExit();
	static bool staticConstructed;
    static void staticConstructor();
    static AuxiliaryTask receiveDataTask; //TODO: allow different AuxiliaryTasks for different priorities (e.g.: audio vs scope)
    static std::vector<ReceiveAudioThread *> objAddrs;
#endif
public:
#ifdef USE_JUCE
    ReceiveAudioThread(const String &threadName);
#else
    ReceiveAudioThread();
#endif
    ~ReceiveAudioThread();
    void init(int port, int aSamplesPerBlock, int channel);
    void bindToPort(int aPort);
    bool isListening();
    float* getCurrentBuffer(int length);
    /**
     * Copies the samples to a non-interleaved buffer.
     */
    int getSamplesSrc(float *destination, int length, float samplingRateRatio);
    /**
     * Copies the samples to an interleaved buffer.
     */
    int getSamplesSrc(float *destination, int length,
    		float samplingRateRatio, int numChannelsInDestination,
    		int channelToWriteTo);
    bool isBufferReady();
    int getTimestamp();
#ifdef USE_JUCE // if we are in Juce, then we run a separate thread for each receiver
    		// (as each of them are typically receiving on a mono or stereo track)
    void run();
#else
    RTIME getLastTime();
    void static run(); //while in BeagleRT we have a single thread that receives for all the instances.
    //TODO: make run() private in BeagleRT
    static void startThread();
    static void stopThread();
    static int getNumInstances();
#endif // USE_JUCE
};
#endif  // RECEIVEAUDIOTHREAD_H_INCLUDED
