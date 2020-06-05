#include <ReceiveAudioThread.h>

#ifdef USE_JUCE
#else
//initialise static members
bool ReceiveAudioThread::staticConstructed=false;
AuxiliaryTask ReceiveAudioThread::receiveDataTask=NULL;
std::vector<ReceiveAudioThread *> ReceiveAudioThread::objAddrs(0);
bool ReceiveAudioThread::threadRunning;
bool ReceiveAudioThread::threadIsExiting;
int ReceiveAudioThread::sleepTime;

void receiveData(){
	ReceiveAudioThread::run();
}
void ReceiveAudioThread::staticConstructor(){
	if(staticConstructed==true)
		return;
	staticConstructed=true;
    threadIsExiting=false;
	receiveDataTask=BeagleRT_createAuxiliaryTask(receiveData, 90, "receiveDataTask"); //TODO: allow different priorities
}
#endif /* USE_JUCE */

void ReceiveAudioThread::dealloc(){
    free(buffer);
    buffer=NULL;
    free(stackBuffer);
    stackBuffer=NULL;
}
void ReceiveAudioThread::wrapWritePointer(){
    //this is not quite a simple wrapping as you would do in a circular buffer,
    //as there is no guarantee the buffer will be full at all times, given that there must alwas be enough space at the end of it
    //to hold a full payload
    // lastValidPointer indicates the last pointer in the buffer containing valid data
    //
    if(writePointer+payloadLength+headerLength>bufferLength){ //if we are going to exceed the length of the buffer with the next reading
        //  lastValidPointer=writePointer+headerLength; //remember where the last valid data are
        //  for(int n=headerLength;n<lastValidPointer; n++){
            //  fprintf(fd2, "%f\n",buffer[n]); //DEBUG
        //  }
    	writePointer=0; //and reset to beginning of the buffer
    }
}
void ReceiveAudioThread::pushPayload(int startIndex){ //backup the payload samples that will be overwritten by the new header
    for(int n=0; n<headerLength; n++){
        stackBuffer[n]=buffer[startIndex+n];
    }
}
void ReceiveAudioThread::popPayload(int startIndex){
    for(int n=0; n<headerLength; n++){
        buffer[startIndex+n]=stackBuffer[n];
    }        
}

int ReceiveAudioThread::readUdpToBuffer(){

	if(listening==false || bufferReady==false)
        return 0;
    if(writePointer<0)
        return 0;
    if(socket.waitUntilReady(true, waitForSocketTime)){// TODO: if waitForSocketTime here is >>5, the
    	// destructor (always or sometimes) never actually gets called, despite run() returns ...see issue #1381
#ifdef USE_JUCE
#else
		lastTime=rt_timer_read();
//        rt_printf("lastTimeread= %llu\n", lastTime);
#endif /* USE_JUCE */
        pushPayload(writePointer); //backup headerLength samples. This could be skipped if writePointer==0
        //read header+payload
        int numBytes=socket.read(buffer+writePointer, bytesToRead, true); //read without waiting.
            //TODO: (if using variable-length payload) validate the actual numBytes read against the size declared in the header
        if(numBytes<0){
            printf("error numBytes1\n");
            return -3; //TODO: something went wrong, you have to discard the rest of the packet!
        }
        if(numBytes==0){//TODO: this should not happen unless you actually receive a packet of size zero (is it at all possible?)
//        	printf("received 0 bytes\n");
        	return 0;
        }
        if(numBytes != bytesToRead){ //this is equivalent to (numBytes<bytesToRead)
            printf("error numBytes2: %d\n", numBytes);
            return -4; //TODO: something went wrong, less bytes than expected in the payload.
        }
        if(channel!=(int)buffer[writePointer]){
//        	printf("I am channel %d, but I received data for channel %d\n", channel, (int)buffer[writePointer]);
        	return -5;
        }
        if(buffer[writePointer+1]!=timestamp+1)
        	printf("missing a timestamp: %d\n",timestamp+1);
        timestamp=buffer[writePointer+1];
//        rt_printf("Received a message of length %d, it was on channel %d and timestamp %d\n", numBytes, (int)buffer[writePointer], (int)buffer[writePointer+1]);

        popPayload(writePointer); //restore headerLength payload samples. This could be skipped if writePointer==0
        //even though we just wrote (payloadLength+headerLength) samples in the buffer,
        //we only increment by payloadLength. This way, next time a socket.read is performed, we will
        //backup the last headerLength samples that we just wrote and we will overwrite them with
        //the header from the new read. After parsing the header we will then restore the backed up samples.
        //This way we guarantee that, apart from the first headerLength samples, buffer is a circular buffer!
        writePointer+=payloadLength;
        wrapWritePointer();
        return numBytes;
    }
    return 0; //timeout occurred
}
//USE_JUCE    Thread(threadName),
#ifdef USE_JUCE
ReceiveAudioThread::ReceiveAudioThread(const String &threadName) :
			Thread(threadName),
#else
ReceiveAudioThread::ReceiveAudioThread() :
#endif /* USE_JUCE */
	socket(0),
    listening(false),
    bufferReady(false),
    buffer(NULL),
    stackBuffer(NULL),
    bufferLength(0),
    lastValidPointer(0),
    waitForSocketTime(5),
#ifdef USE_JUCE
    threadPriority(5)
#else
    threadPriority(88)
#endif /* USE_JUCE */
{};
ReceiveAudioThread::~ReceiveAudioThread(){
#ifdef USE_JUCE
	stopThread(1000);
#else
	stopThread();
	while(threadRunning){
		usleep(sleepTime*2);	//wait for thread to stop
		std::cout<< "Waiting for receiveAudioTask to stop" << std::endl;
	}
#endif /* USE_JUCE */
	//TODO: check if thread stopped, otherwise kill it before dealloc
    dealloc();
}
void ReceiveAudioThread::init(int aPort, int aSamplesPerBlock, int aChannel){
  dealloc();
#ifdef USE_JUCE
#else
  staticConstructor();
  objAddrs.push_back(this);//TODO: this line should be in the constructor
#endif /* USE_JUCE */
  bindToPort(aPort);
  channel=aChannel;
  printf("Channel %d is receiving on port %d\n",aChannel, aPort);
  //  fd=fopen("output.m","w"); //DEBUG
  //  fprintf(fd,"var=["); //DEBUG
  headerLength=2;
  payloadLength=300; //TODO: make sure that payloadLength and headerLength are the same as the client is sending.
  bufferLength=10 * std::max(headerLength+(payloadLength*4), headerLength+(aSamplesPerBlock*4)); //there are many considerations that can be done here ...
                      //We keep a headerLength padding at the beginning of the array to allow full reads from the socket
  buffer=(float*)malloc(sizeof(float)*bufferLength);
  if(buffer==NULL) // something wrong
    return;
  lastValidPointer=headerLength+ ((bufferLength-headerLength)/payloadLength)*payloadLength;
  memset(buffer,0,bufferLength*sizeof(float));
  stackBuffer=(float*)malloc(sizeof(float)*headerLength);
  if(stackBuffer==NULL) // something wrong
      return;
  bufferReady=true;
  bytesToRead=sizeof(float)*(payloadLength + headerLength);
  writePointer=-1;
  readPointer=0;
  sleepTime=payloadLength/(float)44100 /4.0; //set sleepTime so that you do not check too often or too infrequently
  timestamp=0;
#ifdef USE_JUCE
  startThread(threadPriority);
#else
  //TODO: the thread cannot be started here at the moment because init() is called in setup(), where tasks cannot be scheduled
#endif /* USE_JUCE */
}

void ReceiveAudioThread::bindToPort(int aPort){
    listening=socket.bindToPort(aPort);
#ifdef USE_JUCE
#else
    if(listening==false) //this condition is valid also for USE_JUCE, but we do not printf in USE_JUCE
    	printf("Could not bind to port %d\n",aPort);
#endif /* USE_JUCE */
}
bool ReceiveAudioThread::isListening(){
    return listening;
}
float* ReceiveAudioThread::getCurrentBuffer(int length){ // NOTE: this cannot work all the time unless samplesPerBuffer and payloadLength are multiples
    //TODO: make it return the number of samples actually available at the specified location
    if(isListening()==false || length>bufferLength)
        return NULL;
    readPointer+=length;
    if(readPointer>lastValidPointer){
        readPointer=headerLength;
    }
    return buffer+(int)readPointer;
};
int ReceiveAudioThread::getSamplesSrc(float *destination, int length,
		float samplingRateRatio, int numChannelsInDestination,
		int channelToWriteTo)
{
    if (!(samplingRateRatio>0 && samplingRateRatio<=2))
        return -2;
    if(isListening()==false)
        return -1;
    static int numCalls=0;
    if(writePointer<0 /*|| (numCalls&16383)==0*/){ //if writePointer has not been initalized yet ...
#ifdef USE_JUCE
#else //debug
    	readPointer = headerLength;
#endif /* USE_JUCE */
    	// this cumbersome line means: start writing at a position which is as close as possible
    	// to the center of the buffer, but still is aligned to (payloadLength*x)+headerLength
    	// thus allowing buffering to allow clock drift to go either way
        writePointer = headerLength + ((bufferLength-headerLength)/payloadLength/2)*payloadLength;
                            // This will help keeping them in sync.
                            //TODO: handle what happens when the remote stream is interrupted and then restarted
        printf("write pointer inited at: %d\n", writePointer);
    }
    numCalls++;
    if(length>lastValidPointer) { 
        //not enough samples available, we fill the buffer with what is available, but the destination buffer will not be filled completely
        //at this very moment the other thread might be writing at most one payload into the buffer.
        //To avoid a race condition, we need to let alone the buffer where we are currently writing
        //as writing the payload also temporarily overwrites the previous headerLength samples, we need to account for them as well
        //TODO: This assumes that the writePointer and readPointer do not drift. When doing clock synchronization we will find out that it is not true!
        length=lastValidPointer-payloadLength-headerLength;
        if(length<0) //no samples available at all! 
            return 0;
    }
    for(int n=0; n<length; n++){
        destination[n*numChannelsInDestination+channelToWriteTo]=buffer[(int)(0.5+readPointer)];//simple ZOH non-interpolation (nearest neighbour)
        //  fprintf(fd,"%f, %d, %f;\n",readPointer,writePointer,destination[n]); //DEBUG
        readPointer+=samplingRateRatio;
        if((int)(0.5+readPointer)>=lastValidPointer){
            readPointer=readPointer-lastValidPointer+headerLength;
        }
    }
    return length;
}
int ReceiveAudioThread::getSamplesSrc(float *destination, int length, float samplingRateRatio){
	return getSamplesSrc(destination, length, samplingRateRatio, 1,0);
	// TODO: rewriting this so that it does not call the override method we can save a multiply and add
	// for each sample.
}
bool ReceiveAudioThread::isBufferReady(){
    return bufferReady;
}
#ifdef USE_JUCE
#else
void ReceiveAudioThread::startThread(){
	BeagleRT_scheduleAuxiliaryTask(receiveDataTask);
}
void ReceiveAudioThread::stopThread(){
	threadIsExiting=true;
}
bool ReceiveAudioThread::threadShouldExit(){
	return(gShouldStop || threadIsExiting );
}
RTIME ReceiveAudioThread::getLastTime(){
	return lastTime;
}
#endif /* USE_JUCE */
int ReceiveAudioThread::getTimestamp(){
	return timestamp;
}
void ReceiveAudioThread::run(){
    //  fd2=fopen("buffer.m","w"); //DEBUG
    //  fprintf(fd2, "buf=["); //DEBUG
	threadRunning=true;
	int maxCount=10;
	int count=0;
	// Clean the socket from anything that is currently in it.
#ifdef USE_JUCE
	// this is borrowed from BeagleRT's UdpServer class.
	int n;
	do {
		float waste;
		if(socket.waitUntilReady(true, 0)==0)
			break;
		n=socket.read((void*)&waste, sizeof(float), false);
		count++;
		if(n<0){
			printf("error\n");
			break;
		}
		printf("n: %d\n",n);
	} while (n>0 && (maxCount<=0 || count<maxCount));
#else
	for(unsigned int n=0; n<objAddrs.size(); n++){
		count=objAddrs[n]->socket.empty(maxCount);
	}
#endif /* USE_JUCE */
	printf("socket emptied with %d reads\n", count);

    while(!threadShouldExit()){ //TODO: check that the socket buffer is empty before starting
#ifdef USE_JUCE
        readUdpToBuffer(); // read into the oldBuffer
        sleep(sleepTime);
#else
		for(unsigned int n=0; n<ReceiveAudioThread::objAddrs.size(); n++){
			ReceiveAudioThread::objAddrs[n]->readUdpToBuffer();
		}
		usleep(sleepTime); //TODO: use rt_task_sleep instead
#endif /* USE_JUCE */
    }
    threadRunning=false;
    printf("Thread is not running \n");
    //  fprintf(fd,"];readPointer,writePointer,lastValidPointer,destination]=deal(var(:,1), var(:,2), var(:,3), var(:,4));"); //DEBUG
    //  fclose(fd);//DEBUG
    //  fprintf(fd2,"];");//DEBUG
    //  fclose(fd2); //DEBUG
}
