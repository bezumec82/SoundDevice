#include "NetworkSend.h"

#ifdef USE_JUCE
#else
//initialize the static members of NetworkSend
bool NetworkSend::staticConstructed=false;
int NetworkSend::sleepTimeMs;
bool NetworkSend::threadIsExiting;
bool NetworkSend::threadRunning;
std::vector<NetworkSend*> NetworkSend::objAddrs(0);
AuxiliaryTask NetworkSend::sendDataTask=NULL;

void sendData(){
	NetworkSend::run();
}
void NetworkSend::staticConstructor(){
	if(staticConstructed==true)
		return;
	staticConstructed=true;
	threadIsExiting=false;
	threadRunning=false;
	sendDataTask = BeagleRT_createAuxiliaryTask(::sendData, 95, "sendDataTask"); //TODO: allow variable priority
}
void NetworkSend::sendAllData(){
	for(unsigned int n=0; n<NetworkSend::objAddrs.size(); n++){
		NetworkSend::objAddrs[n]->sendData();
	}
}
int NetworkSend::getNumInstances(){
	return objAddrs.size();
}
void NetworkSend::startThread(){
	BeagleRT_scheduleAuxiliaryTask(sendDataTask);
}
void NetworkSend::stopThread(){
	threadIsExiting=true;
}
bool NetworkSend::threadShouldExit(){
	return(gShouldStop || threadIsExiting);
}
bool NetworkSend::isThreadRunning(){
	return threadRunning;
}
#endif /* USE_JUCE */

#ifdef USE_JUCE
NetworkSend::NetworkSend(const String &threadName):
		Thread(threadName)
#else
NetworkSend::NetworkSend()
#endif /* USE_JUCE */
{
	channel.buffers=NULL;
	channel.doneOnTime=NULL;
	channel.readyToBeSent=NULL;
	channel.enabled=false;
	sleepTimeMs=2; //This should actually be initialized in the staticConstructor for non-Juce code, but doing it here makes it more portable
	channel.sampleCount=0;
}

NetworkSend::~NetworkSend(){
#ifdef USE_JUCE
	stopThread(1000);
#else
	stopThread();
	for(unsigned int n=0; n<objAddrs.size(); n++){ //keep track of deleted instances;
		if(objAddrs[n]==this){
			objAddrs.erase(objAddrs.begin()+n);
			break;
		}
	}
#endif
	dealloc();
}
void NetworkSend::dealloc(){
	channel.enabled=false;
	if(channel.buffers!=NULL){
		for(int n=0; n<channel.numBuffers; n++){
			free(channel.buffers[n]);
			channel.buffers[n]=NULL;
		}
		free(channel.buffers);
		channel.buffers=NULL;
	}
	free(channel.readyToBeSent);
	channel.readyToBeSent=NULL;
	free(channel.doneOnTime);
	channel.doneOnTime=NULL;
}
void NetworkSend::cleanup(){
	dealloc();
}

void NetworkSend::setup(float aSampleRate, int blockSize, int aChannelNumber, int aPort, const char *aServer){
#ifdef USE_JUCE
#else
	staticConstructor(); //FIXME: ideally this should be in the constructor, but this is not currently possible
						//because of limitations in BeagleRT_createAuxiliaryTask()
	//keep track of added active instances
	objAddrs.push_back(this);//TODO: this line should be in the constructor, but something weird happens if
	// an instance of NetworkSend is then declared globally: the constructor gets called,
	// and objAddrs.size()==1 but when you get to setup, objAddrs.size() has reverted back to 0, without
	// any destructor being called in between ... Have a look here
	// http://stackoverflow.com/questions/7542054/global-vector-emptying-itself-between-calls .
	// and maybe use accessor function instead of global, as was done in #1374
#endif /* USE_JUCE */
	cleanup();
	int numSamples=blockSize*4>4*channel.bufferLength ? blockSize*4 : 4*channel.bufferLength;
	channel.numBuffers= (1+numSamples/channel.bufferLength) * 3; //the +1 takes the ceil() of the division
	channel.buffers=(float**)malloc(channel.numBuffers*sizeof(float*));
	printf("NumBuffers: %d\n", channel.numBuffers);
	if(channel.buffers==NULL)
		return;
	for(int n=0; n<channel.numBuffers; n++){
		channel.buffers[n]=(float*)malloc(channel.bufferLength*sizeof(float));
		if(channel.buffers[n]==NULL)
			return;
	}
	channel.readyToBeSent=(bool*)malloc(channel.numBuffers*sizeof(bool));
	channel.doneOnTime=(bool*)malloc(channel.numBuffers*sizeof(bool));
	for(int n=0; n<channel.numBuffers; n++){
		channel.readyToBeSent[n]=false;
		channel.doneOnTime[n]=true;
	}
	if(channel.readyToBeSent==NULL || channel.doneOnTime==NULL)
		return;
	channel.writePointer=channel.headerLength;
	channel.writeBuffer=0;
	channel.readBuffer=0;
	setChannelNumber(aChannelNumber);
	setPort(aPort); //TODO: check for the return value
	setServer(aServer); //TODO: check for the return value
	printf("Channel %d is sending messages to: %s:%d at %fHz\n", getChannelNumber(), aServer, aPort, aSampleRate);
	channel.enabled=true;
}

void NetworkSend::log(float value){ //TODO: add a vectorized version of this method
	if(channel.enabled==false)
		return;
	if(channel.writePointer==channel.bufferLength){ // when the buffer is filled ...
		channel.readyToBeSent[channel.writeBuffer]=true; // flag it as such
//		printf("Scheduling for send %d\n",(int)channel.buffers[channel.writeBuffer][channel.headerTimestampIndex]);
		channel.writePointer=channel.headerLength; //reset the writePointer
		channel.writeBuffer=(channel.writeBuffer+1); //switch buffer
		if(channel.writeBuffer==channel.numBuffers) // and wrap it
            channel.writeBuffer=0;
//		printf("WriteBuffer:%d\n", channel.writeBuffer);
		if(channel.doneOnTime[channel.writeBuffer]==false){ //check if this buffer's last sending has completed on time ...
			printf("NetworkSend buffer underrun. timestamp: %d :-{\n",
					(int)channel.buffers[channel.writeBuffer][channel.headerTimestampIndex]);
		}
		channel.doneOnTime[channel.writeBuffer]=false; // ... and then reset the flag
#ifdef USE_JUCE
		if(isThreadRunning()==false){
			startThread(10);
		}
#else
		if(isThreadRunning()==false){
			startThread();
		}
#endif /* USE_JUCE */
	}
	if(channel.writePointer==channel.headerLength){ // we are about to start writing in the buffer, let's set the header
		//set dynamic header values here. Static values are set in setup() and setChannelNumber().
		channel.buffers[channel.writeBuffer][channel.headerTimestampIndex]=(float)channel.sampleCount; //timestamp
		channel.sampleCount++;
		//add here more header fields
	}
    channel.buffers[channel.writeBuffer][channel.writePointer++]=value;
//	sampleCount++;
};

void NetworkSend::setServer(const char *aServer){
#ifdef USE_JUCE
	remoteHostname=String::fromUTF8(aServer);
#else
	udpClient.setServer(aServer);
#endif /* USE_JUCE */
}
void NetworkSend::setPort(int aPort){
#ifdef USE_JUCE
	remotePortNumber=aPort;
#else
	udpClient.setPort(aPort);
#endif /* USE_JUCE */
}

void NetworkSend::setChannelNumber(int aChannelNumber){
	channel.channelNumber=aChannelNumber;
	for(int n=0; n < channel.numBuffers; n++){ //initialize the header
		channel.buffers[n][channel.headerChannelIndex]=channel.channelNumber;
		//add here more static header fields
	}
};
int NetworkSend::getChannelNumber(){
	return channel.channelNumber;
};

int NetworkSend::getTimestamp(){
	return channel.buffers[channel.readBuffer][channel.headerTimestampIndex];
}

void NetworkSend::sendData(){
	if(channel.enabled==false)
		return;
	while(channel.readyToBeSent[channel.readBuffer]==true){
		channel.readyToBeSent[channel.readBuffer]=false;
		void* sourceBuffer=channel.buffers[channel.readBuffer];
//		printf("Trying to send timestamp %d\n",(int)((float*)sourceBuffer)[channel.headerTimestampIndex]);
//		printf("ReadBuffer:%d\n", channel.readBuffer);
		unsigned int numBytesToSend=NETWORK_AUDIO_BUFFER_SIZE*sizeof(float);
		//TODO: call waitUntilReady before trying to write/send, to avoid blocks! (OR NOT?)
#ifdef USE_JUCE
		if(1==udpClient.waitUntilReady(0, 5)){
			udpClient.write(remoteHostname, remotePortNumber, sourceBuffer, numBytesToSend);
			channel.doneOnTime[channel.readBuffer]=true;
			//  printf ("Sent timestamp: %d\n", (int)((float*)sourceBuffer)[1]);
		} else {
			//  printf ("Not ready timestamp: %d\n", (int)((float*)sourceBuffer)[1]);
		}
#else
		udpClient.send(sourceBuffer, numBytesToSend);
//		printf("sent sourceBuffer: %d, channel: %f, timestamp: %f\n", channel.readBuffer, channel.buffers[channel.readBuffer][0],
//				channel.buffers[channel.readBuffer][1]);
		channel.doneOnTime[channel.readBuffer]=true;
#endif /* USE_JUCE */
		channel.readBuffer++;
		if(channel.readBuffer==channel.numBuffers)
			channel.readBuffer=0;
	}
}

void NetworkSend::run(){
#ifdef USE_JUCE
	//  std::chrono::high_resolution_clock::time_point t1;
    //  std::chrono::high_resolution_clock::time_point t2;
    //  std::chrono::high_resolution_clock::time_point t3;
	while(threadShouldExit()==false){
		//  t3 = std::chrono::high_resolution_clock::now();
		//  t1 = std::chrono::high_resolution_clock::now();
		sendData();
		//  t2 = std::chrono::high_resolution_clock::now();
		//  auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t1 ).count();
		//  auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
		//  if(duration2>0)
			//  std::cout << "Duration is: " << duration2 <<". Whole loop is: " << duration1 << "\n"; 
		usleep(1000);
	}
#else
	threadRunning=true;
	while(threadShouldExit()==false){
		sendAllData();
		usleep(sleepTimeMs*1000);
	}
	threadRunning=false;
#endif
}
