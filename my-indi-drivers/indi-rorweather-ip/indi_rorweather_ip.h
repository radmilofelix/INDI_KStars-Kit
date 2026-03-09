#ifndef __RORWEATHERIP_H__
#define __RORWEATHERIP_H__

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

#include "indiweather.h"
#include "connectionplugins/connectionserial.h"
#include "connectionplugins/connectiontcp.h"
#include "indicom.h"
#include <cmath>
#include <cstring>
#include <ctime>
#include <memory>
#include <termios.h>

//#define RORWEATHERIPDEBUG

#define TCP_TIMEOUT 5 /* Timeout in seconds */
#define TCPSTOPCHAR '\0'
#define TCPRETRIES 5
#define CONNECTIONRETRIES 5
#define TIMERHIT_VALUE 5000 // milliseconds
#define HARTBEATCALL "RorWeatherIp-Connect\n"
#define HARTBEATRESPONSE "RorWeatherIp-Connect-Ok"

class RorWeather : public INDI::Weather
{
  public:
    RorWeather();
    virtual ~RorWeather() = default;
	virtual bool Handshake() override;
    const char *getDefaultName() override;

    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;

  protected:
    virtual IPState updateWeather() override;
//    virtual bool saveConfigItems(FILE *fp) override;
    int TcpRequest(char *request, char *response, char stopChar);
    void TimerHit() override;
    void GetWeather();
    void CloseConnection();
    bool TestConnection();

  private:
	std::string buffer;
    Connection::TCP *TCPConnection{ nullptr };
    Connection::Serial *serialConnection{ nullptr };

//    INumber ControlWeatherN[14];
//    INumberVectorProperty ControlWeatherNP;

	INDI::PropertyNumber ControlWeatherNP{14};

    enum
    {
        CONTROL_WEATHER,
        CONTROL_TEMPERATURE,
        CONTROL_HUMIDITY,
        CONTROL_RAIN,
        CONTROL_RAINFALL,
        CONTROL_WINDINSTANT,
        CONTROL_WINDSPEED,
        CONTROL_WINDGUST,
        CONTROL_PRESSURE,
        CONTROL_WARMROOMTEMPERATURE,
        CONTROL_WARMROOMHUMIDITY,
        CONTROL_OBSERVATORYTEMPERATURE,
        CONTROL_OBSERVATORYHUMIDITY,
        CONTROL_UPS
    };

	int rainStatus;
	float rainFallStatus;
	float windInstantStatus;
	float windSpeedStatus;
	float windGustStatus;
	float externalTemperatureDsStatus;
	float externalTemperatureDhtStatus;
	float externalHumidityStatus;
	float warmroomTemperatureStatus;
	float warmroomHumidityStatus;
	float airPressureStatus;
	float observatoryTemperatureStatus;
	float observatoryHumidityStatus;
	int upsStatus;

    int tcpRetries;
    int connectionRetries;
};




#endif
