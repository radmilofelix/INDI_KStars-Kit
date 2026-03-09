/*******************************************************************************
 ROR Roof weather sensors, TCP/IP connection
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
#include "indi_rorweather_ip.h"

// We declare an auto pointer to RorWeather.
std::unique_ptr<RorWeather> rorweather(new RorWeather());

RorWeather::RorWeather()
{
    setVersion(1, 0);
    setWeatherConnection(CONNECTION_TCP);
    tcpRetries=TCPRETRIES;
    connectionRetries = CONNECTIONRETRIES;
}

const char *RorWeather::getDefaultName()
{
    return (const char *)"ROR Weather IP";
}

void RorWeather::CloseConnection()
{
	Disconnect();

/*
    ISwitchVectorProperty *svp = getSwitch("CONNECTION");
    char *namesSw[2];
    ISState statesSw[2];
    statesSw[0] = ISS_OFF;
    statesSw[1] = ISS_ON;
    namesSw[0]  = const_cast<char *>("Connect");
    namesSw[1]  = const_cast<char *>("Disconnect");
    IUUpdateSwitch(svp, statesSw, namesSw, 2);
    svp->s = IPS_ALERT;
    IDSetSwitch(svp, nullptr);
//*/
    tcpRetries=3;
    LOG_ERROR("TCP connection error, driver disconnected.");
}

int RorWeather::TcpRequest(char *request, char *response, char stopChar)
{
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
				LOGF_WARN("Error writing  tcp rquest to RORWeatherIP TCP server. Request: %s", request);
				return rc;
			}
		}
		usleep(500000);
	}

	tcflush(PortFD, TCIOFLUSH);
	rc = tty_read_section(PortFD, response, stopChar, TCP_TIMEOUT, &nbytes_read);
	if( rc != TTY_OK )
	{
		tty_error_msg(rc, errstr, MAXRBUF);
		#ifdef RORWEATHERIPDEBUG
			LOGF_WARN("Error reading response: %s.", errstr);
			LOGF_WARN("Error reading tcp response from RORWeatherIP device. Request: %s, Response: %s", request, response);
		#endif
		return rc;
	}
	tcpRetries = TCPRETRIES;
	response[nbytes_read]=0;
    return rc;
}

bool RorWeather::TestConnection()
{
	int rc = 0;
	char pCMD[MAXRBUF] = {0}, pRES[MAXRBUF] = {0};
	strcpy(pCMD, "RorWeatherIp-Connect");
	LOGF_DEBUG("CMD: %s", pCMD);
	rc=TcpRequest(pCMD ,pRES, TCPSTOPCHAR);
	LOGF_DEBUG("RES: %s", pRES);
	if( !strcmp(pRES,"RorWeatherIp-Connect-Ok") )
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool RorWeather::Handshake()
{
    if (isSimulation())
        return true;
    if(!TestConnection())
    {
        LOG_ERROR("Error connecting to Weather Service.");
        return false;
    }
    SetTimer(3000);
    return true;
}

bool RorWeather::initProperties()
{
    INDI::Weather::initProperties();

    ControlWeatherNP[CONTROL_TEMPERATURE].fill("Temperature (C)", "Temperature (C)", "%.2f", -50, 70, 10, 0);
    ControlWeatherNP[CONTROL_HUMIDITY].fill("Humidity (%)", "Humidity (%)", "%.2f", 0, 100, 10, 0);
    ControlWeatherNP[CONTROL_RAIN].fill("Rain", "Rain", "%.f", 0, 100, 10, 0);
    ControlWeatherNP[CONTROL_RAINFALL].fill("Rainfall (mm)", "Rainfall (mm)", "%.2f", 0, 100, 10, 0);
    ControlWeatherNP[CONTROL_WINDINSTANT].fill("Wind Instant (kph)", "Wind Instant (kph)", "%.2f", 0, 100, 5, 0);
    ControlWeatherNP[CONTROL_WINDSPEED].fill("Wind Speed (kph)", "Wind Speed (kph)", "%.2f", 0, 100, 5, 0);
    ControlWeatherNP[CONTROL_WINDGUST].fill("Wind Gust (kph)", "Wind Gust (kph)", "%.2f", 0, 100, 5, 0);
    ControlWeatherNP[CONTROL_WARMROOMTEMPERATURE].fill("Warmroom Temp (C)", "Warmroom Temp (C)", "%.2f", -50, 70, 10, 0);
    ControlWeatherNP[CONTROL_WARMROOMHUMIDITY].fill("Warmroom humidity (%)", "Wamroom humidity (%)", "%.2f", 0, 100, 10, 0);
    ControlWeatherNP[CONTROL_PRESSURE].fill("Pressure (hPa)", "Pressure (hPa)", "%.2f", 0, 2000, 10, 0);
    ControlWeatherNP[CONTROL_OBSERVATORYTEMPERATURE].fill("Observatory Temp (C)", "Observatory Temp (C)", "%.2f", -50, 70, 10, 0);
    ControlWeatherNP[CONTROL_OBSERVATORYHUMIDITY].fill("Observatory humidity (%)", "Observatory humidity (%)", "%.2f", 0, 100, 10, 0);
    ControlWeatherNP[CONTROL_UPS].fill("UPS", "UPS", "%.f", 0, 100, 10, 0);
    ControlWeatherNP.fill(getDeviceName(), "WEATHER_CONTROL", "Weather Values", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);

/*
	Add a physical weather measurable parameter to the weather driver. The weather value has three zones:
		OK: Set minimum and maximum values for acceptable values.
		Warning: Set minimum and maximum values for values outside of Ok range and in the dangerous warning zone.
		Alert: Any value outsize of Ok and Warning zone is marked as Alert.
*/
    addParameter("WEATHER_TEMPERATURE", "Temperature (C)", -10, 40, 15);
    addParameter("WEATHER_WIND_INSTANT", "Wind Instant (kph)", 0, 20, 15);
    addParameter("WEATHER_WIND_SPEED", "Wind Speed (kph)", 0, 20, 15);
    addParameter("WEATHER_WIND_GUST", "Wind Gust (kph)", 0, 20, 15);
    addParameter("WEATHER_RAIN_HOUR", "Rain ", 0, 0, 15);
    addParameter("WEATHER_UPS", "UPS ", 0, 0, 15);

/*
	setCriticalParameter Set parameter that is considered critical to the operation of the observatory.
	The parameter state can affect the overall weather driver state which signals the client
	to take appropriate action depending on the severity of the state.
*/
    setCriticalParameter("WEATHER_TEMPERATURE");
    setCriticalParameter("WEATHER_WIND_INSTANT");
    setCriticalParameter("WEATHER_WIND_SPEED");
    setCriticalParameter("WEATHER_WIND_GUST");
    setCriticalParameter("WEATHER_RAIN_HOUR");
    setCriticalParameter("WEATHER_UPS");
    addDebugControl();
    return true;
}

bool RorWeather::updateProperties()
{
    INDI::Weather::updateProperties();

    if (isConnected())
    {
        defineProperty(ControlWeatherNP);
	}
    else
    {
        deleteProperty(ControlWeatherNP);
 
}
    return true;
}

IPState RorWeather::updateWeather()
{
    setParameterValue("WEATHER_TEMPERATURE", ControlWeatherNP[CONTROL_TEMPERATURE].getValue());
    setParameterValue("WEATHER_WIND_INSTANT", ControlWeatherNP[CONTROL_WINDINSTANT].getValue());
    setParameterValue("WEATHER_WIND_SPEED", ControlWeatherNP[CONTROL_WINDSPEED].getValue());
    setParameterValue("WEATHER_WIND_GUST", ControlWeatherNP[CONTROL_WINDGUST].getValue());
    setParameterValue("WEATHER_RAIN_HOUR", ControlWeatherNP[CONTROL_RAIN].getValue());
    setParameterValue("WEATHER_UPS", ControlWeatherNP[CONTROL_UPS].getValue());
    return IPS_OK;
}

bool RorWeather::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (!strcmp(name, ControlWeatherNP.getName()))
        {
			ControlWeatherNP.update(values, names, n);
//            IUUpdateNumber(ControlWeatherNP, values, names, n);
            ControlWeatherNP.setState(IPS_OK);
			ControlWeatherNP.apply();
//            IDSetNumber(ControlWeatherNP, nullptr);
            LOG_INFO("Values are updated and should be active on the next weather update.");
            return true;
        }
    }
    return INDI::Weather::ISNewNumber(dev, name, values, names, n);
}

//bool RorWeather::saveConfigItems(FILE *fp)
//{
//    INDI::Weather::saveConfigItems(fp);

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// using INDI::poperty as a pointer is deprecated.
// todo: find id ControlWeatherNP has to be saved and if yes, how.
// see https://www.indilib.org/developers/developer-manual/101-standard-properties.html when the site is back online again
//
//    IUSaveConfigNumber(fp, ControlWeatherNP);
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

//    return true;
//}

void RorWeather::GetWeather()
{
	char pCmd[MAXRBUF] = {0};
	char pBuffer[MAXRBUF] = {0};
	strcpy(pCmd,"getWeather");
	TcpRequest(pCmd, pBuffer, TCPSTOPCHAR);
	if(!strcmp(pBuffer,"Error"))
	{
		LOG_ERROR("Error in getting weather data!");
		rainStatus = 0;
		rainFallStatus = 0;
		windInstantStatus = 0;
		windSpeedStatus = 0;
		windGustStatus = 0;
		externalTemperatureDsStatus = 0;
		externalTemperatureDhtStatus = 0;
		externalHumidityStatus = 0;
		warmroomTemperatureStatus = 0;
		warmroomHumidityStatus = 0;
		airPressureStatus = 0;
		observatoryTemperatureStatus = 0;
		observatoryHumidityStatus = 0;
		upsStatus = 0;
		return;
	}

	if(!strcmp(pBuffer,"Wait"))
	{
	    #ifdef RORWEATHERIPDEBUG
	    LOG_WARN("Server waiting for weather data!");
	    #endif
	    return;
	}
	int counter=0;
	int paramCounter=0;
	char parameter[10];
	for(int i=0; i<(int)strlen(pBuffer); i++)
	{
		if(pBuffer[i] != 10)
		{
			parameter[i-counter]=pBuffer[i];
		}
		else
		{
			parameter[i-counter]=0;
			switch(paramCounter)
			{
				case 0:
					rainStatus = atoi(parameter);
				break;
				case 1:
					rainFallStatus = atof(parameter);
				break;
				case 2:
					windInstantStatus = atof(parameter)*3.6;
				break;
				case 3:
					windSpeedStatus = atof(parameter)*3.6;
				break;
				case 4:
					windGustStatus = atof(parameter)*3.6;
				break;
				case 5:
					externalTemperatureDsStatus = atof(parameter);
				break;
				case 6:
					externalTemperatureDhtStatus = atof(parameter);
				break;
				case 7:
					externalHumidityStatus = atof(parameter);
				break;
				case 8:
					airPressureStatus = atof(parameter);
				break;
				case 9:
					warmroomTemperatureStatus = atof(parameter);
				break;
				case 10:
					warmroomHumidityStatus = atof(parameter);
				break;
				case 11:
					observatoryTemperatureStatus = atof(parameter);
				break;
				case 12:
					observatoryHumidityStatus = atof(parameter);
				break;
				case 13:
					upsStatus = atoi(parameter);
				break;
			}
			counter=i+1;
			paramCounter++;
		}
	}

    ControlWeatherNP[CONTROL_TEMPERATURE].setValue(externalTemperatureDhtStatus);
    ControlWeatherNP[CONTROL_HUMIDITY].setValue(externalHumidityStatus);
    ControlWeatherNP[CONTROL_RAIN].setValue(rainStatus);
    ControlWeatherNP[CONTROL_RAINFALL].setValue(rainFallStatus);
    ControlWeatherNP[CONTROL_WINDINSTANT].setValue(windInstantStatus);
    ControlWeatherNP[CONTROL_WINDSPEED].setValue(windSpeedStatus);
    ControlWeatherNP[CONTROL_WINDGUST].setValue(windGustStatus);
    ControlWeatherNP[CONTROL_WARMROOMTEMPERATURE].setValue(warmroomTemperatureStatus);
    ControlWeatherNP[CONTROL_WARMROOMHUMIDITY].setValue(warmroomHumidityStatus);
    ControlWeatherNP[CONTROL_PRESSURE].setValue(airPressureStatus);
    ControlWeatherNP[CONTROL_OBSERVATORYTEMPERATURE].setValue(observatoryTemperatureStatus);
    ControlWeatherNP[CONTROL_OBSERVATORYHUMIDITY].setValue(observatoryHumidityStatus);
    ControlWeatherNP[CONTROL_UPS].setValue(upsStatus);
	ControlWeatherNP.setState(IPS_OK);
	ControlWeatherNP.apply();
//    IDSetNumber(ControlWeatherNP, nullptr);
}

void RorWeather::TimerHit()
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
		GetWeather();
	}

	SetTimer(TIMERHIT_VALUE);
}
