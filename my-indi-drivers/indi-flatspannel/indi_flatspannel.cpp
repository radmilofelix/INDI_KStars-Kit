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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
   USA

*/

#include "indi_flatspannel.h"

std::unique_ptr<FlatsPannel> dewHeaterAndFan(new FlatsPannel());

FlatsPannel::FlatsPannel()
{
    setVersion(1, 1);
    tcpRetries=TCPRETRIES;
    connectionRetries = CONNECTIONRETRIES;
    commandRetries = COMMANDRETRIES;
}

int FlatsPannel::TcpRequest(char *request, char *response, std::string stopChar)
{
    if (getActiveConnection() == serialConnection)
    {
        PortFD = serialConnection->getPortFD();
    }
    else if (getActiveConnection() == TCPConnection)
    {
        PortFD = TCPConnection->getPortFD();
    }

    int rc = 0, nbytes_written = 0, nbytes_read = 0;
	char errstr[MAXRBUF];

	while(tcpRetries)
	{
		tcflush(PortFD, TCIOFLUSH);
		rc = tty_write_string(PortFD, request, &nbytes_written);
		if( rc== TTY_OK )
		{
			break;
		}
		else
		{
			tcpRetries--;
			if( tcpRetries == 0)
			{
				tty_error_msg(rc, errstr, MAXRBUF);
				LOGF_ERROR("Error writing command %s: %s.", request, errstr);
				LOGF_WARN("Error writing  tcp rquest to FlatPannel TCP server. Request: %s", request);
				return rc;
			}
		}
		usleep(500000);
	}
	tcflush(PortFD, TCIOFLUSH);
	rc = tty_read_section(PortFD, response, stopChar[0], TCP_TIMEOUT, &nbytes_read);
	if( rc != TTY_OK )
	{
		tty_error_msg(rc, errstr, MAXRBUF);
		#ifdef FLATSDEBUG
			LOGF_WARN("Error reading response: %s.", errstr);
			LOGF_WARN("Error reading tcp response from RORWeatherIP device. Request: %s, Response: %s", request, response);
		#endif
		return rc;
	}
	tcpRetries = TCPRETRIES;
    response[strcspn(response, "\r")] = 0;
    response[strcspn(response, "\n")] = 0;
	response[nbytes_read]=0;
    return rc;
}

void FlatsPannel::CloseConnection()
{
    Disconnect();
	INDI::PropertySwitch svp = getSwitch("CONNECTION");
	svp.onUpdate([svp, this]() mutable
	{
        if (!isConnected())
        {
            svp.setState(IPS_ALERT);
            svp.apply("Cannot change property while device is disconnected.");
            return;
        }
        auto index = svp.findOnSwitchIndex();
        if (index < 0)
            return;
 
        svp.setState(IPS_ALERT);
    });

	deleteProperty(PannelPwmNP);
    tcpRetries=TCPRETRIES;
    LOG_ERROR("TCP connection error, closing driver connection.");
}


bool FlatsPannel::initProperties()
{
    DefaultDevice::initProperties();

    // Heater duty cycles
    PannelPwmNP[0].fill("PANNELPWM", "Pannel PWM", "%g", 0, 65535, 1000, 0);
    PannelPwmNP.fill(getDeviceName(), "PANNELPWM", "Pannel PWM", MAIN_CONTROL_TAB, IP_RW, 0, IPS_IDLE);

    if (pannelConnection & CONNECTION_SERIAL)
    {
        serialConnection = new Connection::Serial(this);
        serialConnection->registerHandshake([&]()
        {
            return Handshake();
        });
        serialConnection->setDefaultBaudRate(Connection::Serial::B_115200);
        registerConnection(serialConnection);
    }

    if (pannelConnection & CONNECTION_TCP)
    {
        TCPConnection = new Connection::TCP(this);
        TCPConnection->setDefaultHost("192.168.1.1");
        TCPConnection->setDefaultPort(10001);
        TCPConnection->registerHandshake([&]()
        {
            return Handshake();
        });
        registerConnection(TCPConnection);
	}

    return true;
}

bool FlatsPannel::updateProperties()
{
    DefaultDevice::updateProperties();

    if (isConnected())
    {
        defineProperty(PannelPwmNP);
    }
    else
    {
        deleteProperty(PannelPwmNP);
    }

    return true;
}

const char *FlatsPannel::getDefaultName()
{
    return (const char *)"Flats Pannel";
}

bool FlatsPannel::Handshake()
{

    if (isSimulation())
        return true;
    if(!TestConnection())
    {
        LOG_ERROR("Handshake:Connection error.");
        return false;
    }
    return true;
}

bool FlatsPannel::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {

    }

    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

bool FlatsPannel::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
	
	
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (strcmp(name, PannelPwmNP.getName()) == 0)
        {
			PannelPwmNP.update(values, names, n);
            PannelPwmNP.setState(IPS_BUSY);
            PannelPwmNP.apply();
			SetData();
            return true;
        }
    }

    return INDI::DefaultDevice::ISNewNumber(dev, name, values, names, n);
}

bool FlatsPannel::TestConnection()
{
    return SendCommand( HARTBEATCALL, HARTBEATRESPONSE, TCPSTOPCHAR);
}

void FlatsPannel::TimerHit()
{
    if (!isConnected())
    {
		LOGF_WARN( "TimerHit: %s", "Device disconnected" );
        return; //  No need to reset timer if we are not connected anymore
	}
    if( !TestConnection() )
    {
		if( connectionRetries == 0 )
		{
			CloseConnection();
		}
		else
		{
			connectionRetries--;
		}
	}
	else
	{
		connectionRetries = CONNECTIONRETRIES;
		GetData();
	}

    SetTimer(getCurrentPollingPeriod());
}


bool FlatsPannel::SendCommand(std::string request, std::string expectedResponse, std::string delimiter)
{
    char pRES[MAXRBUF] = {0};
    while (commandRetries)
    {
        TcpRequest( (char*)request.c_str(), pRES, TCPSTOPCHAR);
		buffer = pRES;
        StringParse stringParse( buffer, delimiter);
        if( stringParse.GetElement(0) == expectedResponse)
        {
            commandRetries = COMMANDRETRIES;
            return true;
        }
        else
        {
			if( stringParse.GetElement(0) == "OUTOFRANGE" )
			{
				commandRetries = COMMANDRETRIES;
				return false;
			}
			if( stringParse.GetElement(0) == "NODELIMITER" )
			{
				if( buffer == expectedResponse )
				{
					commandRetries = COMMANDRETRIES;
					return true;
				}
			}
		}
        commandRetries--;
        usleep(500000);
    }
	LOGF_WARN( "SendCommand error, request: %s, response: %s, expected: %s", request.c_str(), buffer.c_str(), expectedResponse.c_str() ); 
    commandRetries = COMMANDRETRIES;
    return false;
}


void FlatsPannel::GetData()
{
	if( SendCommand( GETDATA, GETDATAHEADER, DATADELIMITER))
	{
		StringParse stringParse( buffer, DATADELIMITER);
		PannelPwmNP[0].setValue(stoi( stringParse.GetElement(1) ));
		PannelPwmNP.setState(IPS_OK);
		PannelPwmNP.apply();
	}
	else
	{
		LOGF_WARN( "GetData: %s", "Response wrong" );
	}
}
	
void FlatsPannel::SetData()
{
	std::string dataBuffer;
	dataBuffer = SETDATA;
	dataBuffer += DATADELIMITER;
	dataBuffer += std::to_string( (int)PannelPwmNP[0].getValue()) ;
	dataBuffer += DATADELIMITER;
	SendCommand(dataBuffer, CONFIRMATION,  DATADELIMITER);
}
