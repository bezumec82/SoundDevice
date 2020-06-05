#include "liraProtocol.h"
#include <QtDebug>

CliraProtocol::CliraProtocol(void)
{
	mMSGSegNumberOut = 0;
    mMSGSegNumberIn  = 0;
}

CliraProtocol::~CliraProtocol(void)
{
}

void CliraProtocol::dataFromPort(unsigned char *msgData,unsigned int msgDataLen)
{		
	vector <unsigned char> chunk;
    chunk.insert(chunk.end(),msgData,msgData+msgDataLen);
	mInData.push_back(chunk);
}

void DumpPacket2qDebug(unsigned char *msgPckg, unsigned int msgPckgLen)
{
    QString hex = "CMD 2 LIRA:";
    for (unsigned int i=0;i<msgPckgLen;i++)
    {
        hex +=QString::number(*(msgPckg+i),16);
    }
    qDebug()<<hex;
}

bool CliraProtocol::dataToPort( unsigned short  msgID,
                                unsigned char * msgData,
                                unsigned int    msgDataLen,
                                unsigned short  msgFlags,
                                unsigned char * msgPckg,
                                unsigned int    &msgPckgLen
                                )
{
    if (msgPckgLen>=msgDataLen+14+2)
	{
        unsigned short tmp_short = msgDataLen/2;
        memcpy((void*)msgPckg,(void*)&SYNCPACKET[0],sizeof(SYNCPACKET));
        //memcpy((void*)msgPckg,(void*)&SYNCPACKET[0],sizeof(SYNCPACKET));
        *(msgPckg+4)=tmp_short&0xff;
        *(msgPckg+5)=(tmp_short>>8)&0xff;
        *(msgPckg+6)=mMSGSegNumberOut&0xff;
        *(msgPckg+7)=(mMSGSegNumberOut>>8)&0xff;
        *(msgPckg+8)=(mMSGSegNumberOut>>16)&0xff;
        *(msgPckg+9)=(mMSGSegNumberOut>>24)&0xff;
        *(msgPckg+10)=msgID&0xff;
        *(msgPckg+11)=(msgID>>8)&0xff;
        *(msgPckg+12)=msgFlags&0xff;
        *(msgPckg+13)=(msgFlags>>8)&0xff;

        for (unsigned int i=0;i<msgDataLen/2;i++)
		{
            *(msgPckg+14+2*i)=*(msgData+2*i);
            *(msgPckg+14+2*i+1)=*(msgData+2*i+1);
		}

        unsigned short CRC16=getCRCFromData(msgPckg,14+msgDataLen);

        *(msgPckg+14+msgDataLen)=CRC16&0xff;
        *(msgPckg+14+msgDataLen+1)=(CRC16>>8)&0xff;

		mMSGSegNumberOut= (mMSGSegNumberOut==0xffffffff) ? 0 : mMSGSegNumberOut+1;
        msgPckgLen=msgDataLen+14+2;
		return true;
	}
    msgPckgLen=msgDataLen+14+2;
	return false;
}

bool CliraProtocol::getReceivedPacketFromText(unsigned short &msgID,
                       unsigned char *msgData,
                       unsigned int &msgLEN,
                       unsigned short &msgFLAGS)
{
    //While only PTT signal can happend!
    while ( !mInData.empty() )
    {
        vector <unsigned char> tdata = mInData.front();
        std::string msg;
        for (unsigned int i=0;i<tdata.size();i++)
        {
            msg.append(1,tdata[i]);
        }
        mInData.pop_front();
        msgID=LIRA_MSG_PTT_STATE;
        msgFLAGS=0;
        msgLEN=1;
        qDebug()<<QString("PTT text-usb event [msg=%1]").arg(msg.c_str());
        if (msg.find("Break")!=std::string::npos)
        {
            *msgData=0x00;
            return true;
        }else
        {
            if (msg.find("Call")!=std::string::npos)
            {
                *msgData=0x01;
                return true;
            }else
            {
                if (msg.find("FREE")!=std::string::npos)
                {
                    *msgData=0x02;
                    return true;
                }else
                {
                    if (msg.find("BUSY")!=std::string::npos)
                    {
                        *msgData=0x00;
                        return true;
                    }
                }
            }
        }        
    }
    return false;
}

bool CliraProtocol::getReceivedPacket(unsigned short &msgID,
                                      unsigned char *msgData,
                                      unsigned int &msgLEN,
                                      unsigned short &msgFLAGS)
{
	while (
		   (!mInData.empty())&&
		   (!mInPacket.IsFull())
		  )
	{
		unsigned int plase_point; 
		while(!(plase_point = mInPacket.DoPlaceData(mInData.front())))
		{
			if (mInData.size()==1) break;
			vector <unsigned char> tmp(mInData.front());
			mInData.pop_front();
			vector <unsigned char> &tmp1=mInData.front();
			tmp1.insert(tmp1.begin(),tmp.begin(),tmp.end());
		}
		if (plase_point>0)
		{
			vector <unsigned char> &tmp=mInData.front();
			if (tmp.size()==plase_point)
			{
				//No more need
				mInData.pop_front();
				continue;
			}else
			{
				//Delete fragment
                tmp.erase(tmp.begin(),tmp.begin()+plase_point);
				if (tmp.empty()) mInData.pop_front();
			}
		}
		if ((mInData.size()==1)&&(!(plase_point))) break;
	}

	if (mInPacket.IsFull())
	{
        msgLEN=mInPacket.MSG.msgData.size();
		msgID=mInPacket.MSG.msgHeader.msgID;
        /*
        if (!mMSGSegNumberIn)
        {
            if(mInPacket.MSG.msgHeader.msgSeqNumber!=mMSGSegNumberIn+1)
            {
                qDebug()<<QString("fial seq %1").arg(mMSGSegNumberIn);
            }
        }
        */
        mMSGSegNumberIn = mInPacket.MSG.msgHeader.msgSeqNumber;
        msgFLAGS=mInPacket.MSG.msgHeader.msgFlags;

		//Copy data
        for (unsigned int i=0;i<(mInPacket.MSG.msgData.size()/2);i++)
		{
            *(msgData+2*i)   = mInPacket.MSG.msgData[2*i];
            *(msgData+2*i+1) = mInPacket.MSG.msgData[2*i+1];
		}

		//ReInit receiver
		mInPacket.ResetLoadPosition();
		return true;
	}

	

	//No data found
	msgLEN=0;

	return false;
}

unsigned short CliraProtocol::getCRCFromData(unsigned char *msgPckg,
                                             unsigned int msgPckgLen)
{
	unsigned short crc_calc = 0xAAAA;
	unsigned short paket_word;
    for (unsigned int i=0;i<msgPckgLen/2;i++)
	{
        paket_word = *(msgPckg+2*i)+(*(msgPckg+2*i+1)<<8);
		crc_calc ^= paket_word;
        crc_calc = ( (crc_calc<<1) &0xFFFE ) | ( (crc_calc>>15) &0x0001 );
	}
	return crc_calc;
}

CliraPacket::CliraPacket(void)
{
	ResetLoadPosition();
}

CliraPacket::~CliraPacket(void)
{
}

bool CliraPacket::IsFull()
{
	return (mLoadPos>=(sizeof(LiraMSG::LiramsgHeader)+sizeof(unsigned short)))&&
		   (mLoadPos==(MSG.msgHeader.msgLength*sizeof(unsigned short))+
		              (sizeof(LiraMSG::LiramsgHeader)+sizeof(unsigned short)));
}

unsigned int CliraPacket::DoPlaceData(vector <unsigned char> &vdata)
{
	if ( (!IsFull()) && (vdata.size()) )
	{		
		unsigned int u_procceded=0;
		if (mLoadPos==0)
		{ //Load header
			//Find sync seq
			while ((u_procceded<vdata.size())&&(vdata[u_procceded]!=SYNCPACKET[0]))
			{
				u_procceded++;                
			}
            if (u_procceded)
            {
                qDebug()<<QString("lost sync [len=%1] - dropped").arg(u_procceded);
                return u_procceded;
            }
            if (vdata.size()<sizeof(unsigned int))
                return 0;
			if (!((vdata[0]==SYNCPACKET[0])&&
				  (vdata[1]==SYNCPACKET[1])&&
				  (vdata[2]==SYNCPACKET[2])&&
                  (vdata[3]==SYNCPACKET[3])))
                return 1;
            if (vdata.size()<sizeof(LiraMSG::LiramsgHeader))
                return 0;
			//Fill data
			MSG.msgHeader.msgStart=0xE4E5AB2D;
			MSG.msgHeader.msgLength=vdata[4]+(vdata[5]<<8);
			MSG.msgHeader.msgSeqNumber=vdata[6]+(vdata[7]<<8)+(vdata[8]<<16)+(vdata[9]<<24);
			MSG.msgHeader.msgID=vdata[10]+(vdata[11]<<8);
			MSG.msgHeader.msgFlags=vdata[12]+(vdata[13]<<8);
			u_procceded=sizeof(LiraMSG::LiramsgHeader);
			mLoadPos=u_procceded;
			return u_procceded;
		}

		if (mLoadPos==sizeof(LiraMSG::LiramsgHeader))
		{ //Load data chank
			u_procceded=MSG.msgHeader.msgLength*2+sizeof(unsigned short);
            if (u_procceded>vdata.size())
                return 0;
            MSG.msgData.insert(MSG.msgData.begin(),
				               vdata.begin(),
							   vdata.begin()+u_procceded-sizeof(unsigned short));
			MSG.msgChecksum = vdata[u_procceded-2]+((vdata[u_procceded-1])<<8);			
		}

		mLoadPos+=u_procceded;
		return u_procceded;
	}else
	{
		return 0;
	}
}

void CliraPacket::ResetLoadPosition()
{
	mLoadPos=0;
    MSG.msgData.resize(0);
}
