#pragma once

#include <list>
#include <vector>
#include <string>
#include <memory.h>

using namespace std;

//MSG from LIRA
#define LIRA_MSG_VOICE_DATA 0x1000
#define LIRA_MSG_PTT_STATE  0x1003
#define LIRA_MSG_HND_STATE  0x1006
#define LIRA_MSG_PRM_SET    0x1080
#define LIRA_MSG_PWR_STATE  0x1081
//CMD toLIRA
#define LIRA_CMD_KEEP_ALIVE    0x1004
#define LIRA_CMD_INITIALIZE    0x1005
#define LIRA_CMD_PTT_COLOR     0x1060
#define LIRA_CMD_SET_INDICATOR 0x1061
#define LIRA_CMD_DISP_VALUE    0x1062

#define LIRA_USB_AUX_CONTROL_B "Aux_Control_B"
#define LIRA_USB_MULTI_TANGENTA "Tangenta_x4"

#pragma pack (push, 1)
struct LiraMSG
{
	struct LiramsgHeader
	{
		unsigned int msgStart;
		unsigned short msgLength;
		unsigned int msgSeqNumber;
		unsigned short msgID;
		unsigned short msgFlags;
	} msgHeader;
	vector <unsigned char>  msgData;
	unsigned short msgChecksum;
};
#pragma pack (pop)

const unsigned char SYNCPACKET[]={0x2d,0xab,0xe5,0xe4};

class CliraPacket
{
public:
	LiraMSG MSG;
public:
	CliraPacket(void);
	~CliraPacket(void);
	bool IsFull();
	unsigned int DoPlaceData(vector <unsigned char> &vdata);
	void ResetLoadPosition();
private:
	unsigned int mLoadPos;
};

class CliraProtocol
{
public:
	CliraProtocol(void);
	~CliraProtocol(void);

	void dataFromPort(unsigned char *msgDATA,
					  unsigned int msgDATALEN);
	bool dataToPort(unsigned short msgID, 
					unsigned char *msgDATA,
					unsigned int msgDATALEN,
					unsigned short msgFlags,
					unsigned char *msgPACKET,
					unsigned int &msgPACKETLEN
					);
    bool getReceivedPacket(unsigned short &msgID,
                           unsigned char *msgDATA,
                           unsigned int &msgLEN,
                           unsigned short &msgFLAGS);
    bool getReceivedPacketFromText(unsigned short &msgID,
                           unsigned char *msgDATA,
                           unsigned int &msgLEN,
                           unsigned short &msgFLAGS);
    void clearInData() {mInData.clear();}
private:
    list <vector <unsigned char> >  mInData;
    unsigned int mMSGSegNumberOut;
    unsigned int mMSGSegNumberIn;
	CliraPacket mInPacket;
	static unsigned short getCRCFromData(unsigned char *msgPACKET,
								         unsigned int msgPACKETLEN);
};
