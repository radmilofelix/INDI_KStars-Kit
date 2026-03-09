#ifndef __RORMAGIP_H__
#define __RORMAGIP_H__

/*******************************************************************************
 Copyright(c) 2021 Radmilo Felix. All rights reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.
 .
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 .
 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#pragma once

#include "indidome.h"
#include "connectionplugins/connectionserial.h"
#include "connectionplugins/connectiontcp.h"
#include "indicom.h"
#include <cmath>
#include <cstring>
#include <ctime>
#include <memory>
#include <termios.h>

//#define RORMAGIPDEBUG

#define TCP_TIMEOUT 5 /* Timeout in seconds */
#define MAXROOFPOSITION 15
#define MINROOFPOSITION 3
#define ROLLOFF_DURATION 10 // 10 seconds until roof is fully opened or closed
#define TIMERHIT_VALUE 3000 // milliseconds
#define CYCLES_TO_ABANDON_MOTION 4 // Cycles limit to abandon roof movement. One cycle is about 15 sec
#define TCPSTOPCHAR '\0'
#define TCPRETRIES 5
#define CONNECTIONRETRIES 5

class RollOff : public INDI::Dome
{
  public:
    RollOff();
    virtual ~RollOff() = default;

    virtual bool initProperties() override;
    const char *getDefaultName() override;
    bool updateProperties() override;
	virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;    
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
    virtual bool saveConfigItems(FILE *fp) override;
    virtual bool ISSnoopDevice(XMLEle *root) override;

  protected:
	virtual bool Handshake() override;
	int TcpRequest(char *request, char *response, char stopChar); 
    void TimerHit() override;
    virtual IPState Park() override;
    virtual IPState UnPark() override;
    void CloseConnection();
    bool TestConnection();

	INDI::PropertyLight MountLockLP {1};
	INDI::PropertyNumber RoofPositionNP {1};
	INDI::PropertyText RoofStatusTP {1};

  private:
    Connection::TCP *TCPConnection{ nullptr };
    Connection::Serial *serialConnection{ nullptr };
    bool SetupParms();
    ISState fullOpenLimitSwitch { ISS_ON };
    ISState fullClosedLimitSwitch { ISS_OFF };
    double MotionRequest { 0 };
    struct timeval MotionStart { 0, 0 };
    int GetRoofPosition();
    bool SendMountStatus();
    int roofPosition;
    int roofMoving;
    int mountStatusFlag;
    int tcpRetries;
	int connectionRetries;
	int roofMovementCycles;
};

#endif
