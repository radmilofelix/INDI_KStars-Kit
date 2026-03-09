#ifndef __FLATSPANNEL_H__
#define __FLATSPANNEL_H__

/*
    Flats Pannel Controller
    Copyright (C) 2023 Radmilo Felix

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "connectionplugins/connectionserial.h"
#include "connectionplugins/connectiontcp.h"
#include "indicom.h"

#include <cmath>
#include <cstring>
#include <memory>
#include <termios.h>
#include <unistd.h>

#include <ctime>
#include <cerrno>
#include <regex>

#pragma once
#include <defaultdevice.h>
#include "StringParse.h"

#define FLATSDEBUG

// Network
#define TIMERHIT_VALUE 1000 // milliseconds
#define TCPRETRIES 5
#define CONNECTIONRETRIES 5
#define COMMANDRETRIES 5
#define TCP_TIMEOUT 5
#define TCPSTOPCHAR "\n"

//      Communication
#define HARTBEATCALL "FlatsPannelCall\n"
#define HARTBEATRESPONSE "FlatsPannelAlive"
#define GETDATA "getData"
#define GETDATAHEADER "11"
#define SETDATA "2735"
#define CONFIRMATION "OK"
#define DATADELIMITER ";"

namespace Connection
{
	class Serial;
};

class FlatsPannel : public INDI::DefaultDevice
{
  public:
    FlatsPannel();
    virtual ~FlatsPannel() = default;

    virtual const char *getDefaultName() override;
    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
    virtual void TimerHit() override;
    bool TestConnection();
    std::string buffer;

    typedef enum
    {
        CONNECTION_NONE   = 1 << 0,
        CONNECTION_SERIAL = 1 << 1,
        CONNECTION_TCP    = 1 << 2
    } PannelConnection;


  private:
    bool SendCommand(std::string request, std::string expectedResponse, std::string delimiter);
	uint8_t pannelConnection { CONNECTION_SERIAL | CONNECTION_TCP };

	int TcpRequest(char *request, char *response, std::string stopChar);
    int tcpRetries;
    int connectionRetries;
    int commandRetries;
    void CloseConnection();
    bool Handshake();
	void GetData();
	void SetData();
    Connection::TCP *TCPConnection{ nullptr };
    Connection::Serial *serialConnection{ nullptr };
    int PortFD{ -1 };

	INDI::PropertyNumber PannelPwmNP {1};
};

#endif
