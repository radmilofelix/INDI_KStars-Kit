/*******************************************************************************
 Magnetic ROR Roof, TCP/IP connection
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

#include "indi_rormag_ip.h"

// We declare an auto pointer to RollOff.
std::unique_ptr<RollOff> rollOff(new RollOff());


RollOff::RollOff()
{
    SetDomeCapability(DOME_CAN_PARK);
    setDomeConnection(CONNECTION_TCP);
    roofMoving=0;
    mountStatusFlag=0;
    tcpRetries=TCPRETRIES;
    connectionRetries = CONNECTIONRETRIES;
}

bool RollOff::initProperties()
{
    INDI::Dome::initProperties();
    SetParkDataType(PARK_NONE);
    addAuxControls();

    RoofPositionNP[0].fill("ROOFPOSITION", "Roof Position", "%g", 0, 18, 0, 0);
    RoofPositionNP.fill(getDeviceName(), "ROOFPOSITION", "Roof", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);
	RoofStatusTP[0].fill("ROOFSTATUS", "Roof Status", NULL);
	RoofStatusTP.fill(getDeviceName(), "ROOFSTATUS", "Roof", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);
    MountLockLP[0].fill("Mount Lock", "", IPS_IDLE);
    MountLockLP.fill(getDeviceName(), "Mount", "", MAIN_CONTROL_TAB, IPS_IDLE);
    return true;
}

bool RollOff::updateProperties()
{
    INDI::Dome::updateProperties();
    if (isConnected())
    {
        defineProperty(MountLockLP);
        defineProperty(RoofPositionNP);
        defineProperty(RoofStatusTP);
        deleteProperty(DomeMotionSP); // Removing the Motion switch inherited from the base driver
        SetupParms();
    }
    else
    {
        deleteProperty(MountLockLP);
        deleteProperty(RoofPositionNP);
        deleteProperty(RoofStatusTP);
    }
    return true;
}

bool RollOff::ISSnoopDevice(XMLEle *root)
{
	if(INDI::Dome::isLocked())
	{
		MountLockLP[0].setState(IPS_ALERT);
		MountLockLP.setState(IPS_ALERT);
	}
	else
	{
		MountLockLP[0].setState(IPS_OK);
		MountLockLP.setState(IPS_OK);
	}
	MountLockLP.apply();
    return INDI::Dome::ISSnoopDevice(root);
}

bool RollOff::SetupParms()
{
    // If we have parking data
    if (InitPark())
    {
        if (isParked())
        {
            fullOpenLimitSwitch   = ISS_OFF;
            fullClosedLimitSwitch = ISS_ON;
        }
        else
        {
            fullOpenLimitSwitch   = ISS_ON;
            fullClosedLimitSwitch = ISS_OFF;
        }
    }
    // If we don't have parking data
    else
    {
        fullOpenLimitSwitch   = ISS_OFF;
        fullClosedLimitSwitch = ISS_OFF;
    }
    return true;
}

bool RollOff::Handshake()
{
    if (isSimulation())
        return true;
    if(!TestConnection())
    {
        LOG_ERROR("Error connecting to Roof Sensors.");
        return false;
    }
    mountStatusFlag=0;
    return true;
}

const char *RollOff::getDefaultName()
{
    return (const char *)"ROR Mag IP";
}

bool RollOff::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
        if (!strcmp(name, ParkSP.getName()))
        {
            // Check if any switch is ON
            for (int i = 0; i < n; i++)
            {
                if (states[i] == ISS_ON)
                {
                    if (!strcmp(ParkSP[0].getName(), names[i]))
                    {
                        return (Park() != IPS_ALERT);
                    }
                    else
                    {
                        return (UnPark() != IPS_ALERT);
                    }
                }
            }
        }
    return INDI::Dome::ISNewSwitch(dev, name, states, names, n);
}

bool RollOff::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    return INDI::Dome::ISNewNumber(dev, name, values, names, n);
}

void RollOff::CloseConnection()
{
    Disconnect();
    mountStatusFlag=0;
    deleteProperty(MountLockLP);
    deleteProperty(RoofPositionNP);
    deleteProperty(RoofStatusTP);
    deleteProperty(ParkSP);
    tcpRetries=3;
    LOG_ERROR("TCP connection error, driver disconnected.");
}

bool RollOff::TestConnection()
{
    int rc = 0;
    char pCMD[MAXRBUF] = {0}, pRES[MAXRBUF] = {0};
	strcpy(pCMD, "RorMagIp-Connect");
    LOGF_DEBUG("CMD: %s", pCMD);
	rc=TcpRequest(pCMD, pRES, TCPSTOPCHAR);
    LOGF_DEBUG("RES: %s", pRES);
    if( !strcmp(pRES,"RorMagIp-Connect-Ok") )
    {
		return true;
	}
    else
    {
		return false;
	}
}

int RollOff::TcpRequest(char *request, char *response, char stopChar)
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
				LOGF_WARN("Error writing  tcp rquest to RorMagIP TCP server. Request: %s", request);
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
		#ifdef RORMAGIPDEBUG
			LOGF_WARN("Error reading response: %s.", errstr);
			LOGF_WARN("Error reading tcp response from RorMagIP device. Request: %s, Response: %s", request, response);
		#endif
		return rc;
	}
	tcpRetries = TCPRETRIES;
    response[strcspn(response, "\r")] = 0;
    response[strcspn(response, "\n")] = 0;
	response[nbytes_read]=0;
    return rc;
} 

int RollOff::GetRoofPosition()
{
    char pCMD[MAXRBUF] = {0}, pRES[MAXRBUF] = {0};
    strcpy(pCMD, "getRoofPosition");
    TcpRequest(pCMD, pRES, TCPSTOPCHAR);
    if(!strcmp(pRES,"Error"))
    {
		LOG_ERROR("Server could not get roof position!");
		return roofPosition; // old position
    }
    if(!strcmp(pRES,"Wait"))
    {
		LOG_WARN("Server waiting for roof position!");
		return roofPosition; // old position
    }
    roofPosition = atoi(pRES);
    RoofPositionNP[0].setValue(roofPosition);

    if(roofPosition == MINROOFPOSITION)
    {
		if(roofMoving < 0) // parking
		{
			roofMoving=0;
		}

		RoofStatusTP[0].fill("ROOFSTATUS", "Roof Status", "Roof is closed");
		if(roofMoving == 0)
		{
			ParkSP[0].setState(ISS_ON);
			ParkSP[1].setState(ISS_OFF);
			ParkSP.setState(IPS_OK);
			RoofPositionNP.setState(IPS_OK);
			RoofStatusTP.setState(IPS_OK);
		}
    }

    if(roofPosition == MAXROOFPOSITION)
    {
		if(roofMoving > 0) // unparking
		{
			roofMoving=0;
		}
		RoofStatusTP[0].fill("ROOFSTATUS", "Roof Status", "Roof is open");	
		if(roofMoving == 0)
		{
			ParkSP[0].setState(ISS_OFF);
			ParkSP[1].setState(ISS_ON);
			ParkSP.setState(IPS_OK);
			RoofPositionNP.setState(IPS_OK);
			RoofStatusTP.setState(IPS_OK);
		}
    }

    if(roofPosition > MAXROOFPOSITION)
    {
		RoofStatusTP[0].fill("ROOFSTATUS", "Roof Status", "ERROR - ROOF OVER MAXIMAL LIMIT");
		if(roofMoving == 0) // parking
			ParkSP.setState(IPS_ALERT);
		else
			ParkSP.setState(IPS_BUSY);
		ParkSP[0].setState(ISS_OFF);
		ParkSP[1].setState(ISS_OFF);
		RoofPositionNP.setState(IPS_ALERT);
		RoofStatusTP.setState(IPS_ALERT);
    }

    if(roofPosition < MINROOFPOSITION && roofPosition > 0)
    {
		RoofStatusTP[0].fill("ROOFSTATUS", "Roof Status", "ERROR - ROOF UNDER MINIMAL LIMIT");
		if(roofMoving == 0) // unparking
			ParkSP.setState(IPS_ALERT);
		else
			ParkSP.setState(IPS_BUSY);
		ParkSP[0].setState(ISS_OFF);
		ParkSP[1].setState(ISS_OFF);
		RoofPositionNP.setState(IPS_ALERT);
		RoofStatusTP.setState(IPS_ALERT);
    }

    if(roofPosition <= 0)
    {
		RoofStatusTP[0].fill("ROOFSTATUS", "Roof Status", "UNKNOWN ROOF POSITION");
		if(roofMoving == 0)
			ParkSP.setState(IPS_IDLE);
		else
			ParkSP.setState(IPS_BUSY);
		ParkSP[0].setState(ISS_OFF);
		ParkSP[1].setState(ISS_OFF);
		RoofPositionNP.setState(IPS_IDLE);
		RoofStatusTP.setState(IPS_IDLE);
    }

    if(roofPosition > MINROOFPOSITION && roofPosition < MAXROOFPOSITION)
    {
	if(roofMoving == 0)
	    ParkSP.setState(IPS_IDLE);
	else
	    ParkSP.setState(IPS_BUSY);
		ParkSP[0].setState(ISS_OFF);
		ParkSP[1].setState(ISS_OFF);
		RoofPositionNP.setState(IPS_BUSY);
		RoofStatusTP.setState(IPS_BUSY);
		RoofStatusTP[0].fill("ROOFSTATUS", "Roof Status", "Intermediary roof position");
    }
	ParkSP.apply();
	RoofStatusTP.apply();
	RoofPositionNP.apply();
    return roofPosition;
}


bool RollOff::saveConfigItems(FILE *fp)
{
    return INDI::Dome::saveConfigItems(fp);
}

IPState RollOff::Park()
{
	RoofPositionNP.setState(IPS_BUSY);
	RoofStatusTP.setState(IPS_BUSY);
	RoofPositionNP.apply();
	RoofStatusTP.apply();
	
    if(roofPosition <= MINROOFPOSITION && roofPosition > 0)
    {
	LOG_WARN("Roof is already fully closed.");
        return IPS_OK;
    }
    if(INDI::Dome::isLocked())
    {
		LOG_ERROR(": Can not park ROR wile mount is locking!");
		ParkSP.setState(IPS_ALERT);
		ParkSP.apply();
		return IPS_ALERT;
    }
    if(!roofMoving && (roofPosition > MINROOFPOSITION || roofPosition == 0))
    {
		roofMovementCycles = CYCLES_TO_ABANDON_MOTION;
        LOG_INFO("Roll off is parking...");
        roofMoving=-1;
		ParkSP.setState(IPS_BUSY);
		ParkSP.apply();
		int rc;
		char pCMD[MAXRBUF] = {0}, pRES[MAXRBUF] = {0};
		strcpy(pCMD, "closeRoof");
		rc=TcpRequest(pCMD , pRES, TCPSTOPCHAR);
		if(rc==0)
			return IPS_ALERT;
        return IPS_BUSY;
    }
    else
        return IPS_ALERT;
}

IPState RollOff::UnPark()
{
	RoofPositionNP.setState(IPS_BUSY);
	RoofStatusTP.setState(IPS_BUSY);
	RoofPositionNP.apply();
	RoofStatusTP.apply();
	
    if(roofPosition >= MAXROOFPOSITION)
    {
	LOG_WARN("Roof is already fully open.");
        return IPS_OK;
    }
    if(INDI::Dome::isLocked())
    {
	LOG_ERROR(": Can not unpark ROR wile mount is locking!");
	ParkSP.setState(IPS_ALERT);
	ParkSP.apply();
	return IPS_ALERT;
    }
    if(!roofMoving && roofPosition < MAXROOFPOSITION)
    {
		roofMovementCycles = CYCLES_TO_ABANDON_MOTION;
        LOG_INFO("Roll off is unparking...");
        roofMoving=1;
		ParkSP.setState(IPS_BUSY);
		ParkSP.apply();
		int rc;
		char pCMD[MAXRBUF] = {0}, pRES[MAXRBUF] = {0};
		strcpy(pCMD, "openRoof");
		rc=TcpRequest(pCMD, pRES, TCPSTOPCHAR);
		if(rc==0)
			return IPS_ALERT;
		return IPS_BUSY;
    }
    else
        return IPS_ALERT;
}

bool RollOff::SendMountStatus()
{ // mount status will be sent only if it changes
    char pCMD[MAXRBUF] = {0}, pRES[MAXRBUF] = {0};
	if(INDI::Dome::isLocked())
	{
		if(mountStatusFlag >= 0)
		{
			LOG_INFO("Sent mount locked status (unparked).");
			mountStatusFlag=-1;
			strcpy(pCMD, "mountUnParked");
			LOGF_DEBUG("CMD: %s", pCMD);
			if(TcpRequest(pCMD, pRES, TCPSTOPCHAR))
				return false;
		}
	}
	else
	{
		if(mountStatusFlag <= 0)
		{
			LOG_INFO("Sent mount unlocked status (parked).");
			mountStatusFlag=1;
			strcpy(pCMD, "mountParked");
			LOGF_DEBUG("CMD: %s", pCMD);
			if(TcpRequest(pCMD, pRES, TCPSTOPCHAR))
				return false;
		}
	}
	return true;
}

void RollOff::TimerHit()
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
		GetRoofPosition();
		SendMountStatus();
		if(roofMoving !=0)
		{
			if(roofMovementCycles)
			{
				roofMovementCycles--;
			}
			else
			{
				roofMoving=0;
			}
		}
	}

    SetTimer(TIMERHIT_VALUE);
}
