#include "usbEnum.h"

namespace liraEnumerator{

usbEnum::usbEnum(const std::wstring &manufacture)
{
    const std::string manuf_conv( manufacture.begin(), manufacture.end() );
    qDebug() << "Manufacturer : " << manuf_conv.c_str();
	m_manufacture=manufacture;
}

usbEnum::~usbEnum(void)
{
	clear();
}

void usbEnum::clear()
{
	while (!USBDeviceDSCs.empty())
	{
		USBDeviceDSC *pdsc = USBDeviceDSCs.front();
		USBDeviceDSCs.pop_front();
		delete pdsc;
	} 
}

USBDeviceDSC *usbEnum::Find(const liraUSBDevEnum devType)
{
    QList<USBDeviceDSC *>::iterator liraUSBDev;
	for ( liraUSBDev=USBDeviceDSCs.begin();
		  liraUSBDev!=USBDeviceDSCs.end();
		  liraUSBDev++)
	{
		if ((*liraUSBDev)->device_type==devType)
			return *liraUSBDev;
	}
	return NULL;
}

USBDeviceDSC *usbEnum::Find(const QString &devLocation)
{
    QList<USBDeviceDSC *>::iterator liraUSBDev;
    for ( liraUSBDev=USBDeviceDSCs.begin();
          liraUSBDev!=USBDeviceDSCs.end();
          liraUSBDev++)
    {
        if ((*liraUSBDev)->device_location==devLocation)
            return *liraUSBDev;
    }
    return NULL;
}


bool usbEnum::doScan()
{
	clear();
	doScan(m_manufacture);
	return (USBDeviceDSCs.size()>0);
}


void usbEnum::doScan(const std::wstring &manufacture)
{
    USBDeviceDSC *pdsc;
    if (!manufacture.empty())
    {
        DIR *dp;
        struct dirent *dirp;
        if((dp = opendir("/dev/lira")) != NULL)
        {
            qDebug() << "LIRA driver is present in the system.\n";
            QStringList devTypeList;
            while ((dirp = readdir(dp)) != NULL)
            {
                //Device catalog list
                QString dir(dirp->d_name);
                if ( (dir!=".") && (dir!="..") )
                    devTypeList.append(dir);
            }
            DIR *dp_dev;
            struct dirent *dirp_dev;
            for (int i=0;i<devTypeList.count();i++)
            {
                if ((dp_dev = opendir(QString(QString("/dev/lira/")+devTypeList.at(i)).toAscii().data())) != NULL)
                {
                    while ((dirp_dev = readdir(dp_dev)) != NULL) {
                        //Device catalog list
                        QString dev(dirp_dev->d_name);
                        if ( (dev!=".") && (dev!="..") )
                        {
                            pdsc = new USBDeviceDSC();

                            pdsc->device_location=dev;
                            pdsc->device_name=QString("/dev/lira/%1/%2").arg(devTypeList.at(i)).arg(dev);
                            pdsc->device_rev=0x0400;
                            pdsc->device_type=(liraUSBDevEnum) devTypeList.at(i).toInt(NULL,16);
                            pdsc->device_vid=0xfff0;
                            USBDeviceDSCs.push_back( pdsc );
                        }
                    }
                    closedir(dp_dev);
                }
            };

            closedir(dp);
        } else qDebug() << "No LIRA driver is present in the system.\n";
    }
}

}
