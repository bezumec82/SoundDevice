#pragma once

#include <QSysInfo>
#include <QStringList>
#include <QDebug>

//need platform define
#ifdef Q_WS_WIN
#include <windows.h>
#include <setupapi.h>
#include <algorithm>
#endif

#ifdef Q_WS_X11
#include <sys/types.h>
#include <dirent.h>
#endif

namespace liraEnumerator
{

    enum liraUSBDevEnum
    {
        tangenta	= 0x8003,
        speaker     = 0x8004,
        microphone  = 0x8008,
        headset     = 0x8009,
        iline	    = 0x800d,
        tmicrophone	= 0x800e,
        handset		= 0x800f,
        control     = 0x8020,
        controlEx   = 0x6001
    };

    struct USBDeviceDSC
    {
        QString        device_name;
        QString        device_location;
        liraUSBDevEnum device_type;
        unsigned short device_vid;
        unsigned short device_rev;
    };

    class usbEnum
    {
    public:
        usbEnum(const std::wstring &manufacture);
        virtual ~usbEnum(void);
        bool doScan();
        USBDeviceDSC * Find(const liraUSBDevEnum devType);
        USBDeviceDSC * Find(const QString &devLocation);
        QList <USBDeviceDSC *> USBDeviceDSCs;
    private:
        void doScan(const std::wstring &manufacture);
        void clear();
        std::wstring m_manufacture;
    };

}


