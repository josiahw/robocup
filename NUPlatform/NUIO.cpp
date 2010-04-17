/*! @file NUIO.cpp
    @brief Implementation of  NUIO input/output class

    @author Jason Kulk
 
 Copyright (c) 2009 Jason Kulk
 
    This file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "NUIO.h"
#include "ioconfig.h"

#include "NUIO/GameControllerPort.h"
#include "NUIO/TeamPort.h"
#include "NUIO/JobPort.h"

#include "NUIO/TcpPort.h"
#include "NUIO/RoboCupGameControlData.h"
#include "NUIO/NetworkPortNumbers.h"

#include "NUbot.h"
#include "NUPlatform/NUPlatform.h"
#include "Behaviour/Jobs.h"
#include "Tools/Image/NUimage.h"

#include <sstream>
#include <string>
using namespace std;

#include "debug.h"
#include "debugverbositynetwork.h"

/*! @brief Create a new NUIO interface to network and log files
    @param robotnumber the unique number of the robot (this is used to select a port offset)
    @param nubot a pointer to the nubot
 */
NUIO::NUIO(NUbot* nubot)
{
#if DEBUG_NETWORK_VERBOSITY > 4
    debug << "NUIO::NUIO(" << nubot << ")" << endl;
#endif
    
    m_nubot = nubot;            // we need the nubot so that we can access the public store
    
    #ifdef USE_NETWORK_GAMECONTROLLER
        m_gamecontroller_port = new GameControllerPort(m_nubot->GameInfo);
    #endif
    #ifdef USE_NETWORK_TEAMINFO
        createTeamPort(m_nubot->TeamInfo);
    #endif
    #ifdef USE_NETWORK_JOBS
        m_jobs_port = new JobPort(m_nubot->Jobs);
    #endif
    #ifdef USE_NETWORK_DEBUGSTREAM
        m_vision_port = new TcpPort(VISION_PORT);
    #endif
}

/*! @brief Create a new NUIO interface to network and log files. Use this version in NUview
    @param gameinfo a pointer to the public game information
    @param teaminfo a pointer to the public team information
    @param jobs a pointer to the public joblist
 
 */
NUIO::NUIO(GameInformation* gameinfo, TeamInformation* teaminfo, JobList* jobs)
{
#if DEBUG_NETWORK_VERBOSITY > 4
    debug << "NUIO::NUIO(" << static_cast<void*>(gameinfo) << ", " << static_cast<void*>(teaminfo) << ", " << static_cast<void*>(jobs) << ")" << endl;
#endif
    m_nubot = NULL;
    #ifdef USE_NETWORK_GAMECONTROLLER
        m_gamecontroller_port = new GameControllerPort(gameinfo);
    #endif
    #ifdef USE_NETWORK_TEAMINFO
        createTeamPort(teaminfo);
    #endif
    #ifdef USE_NETWORK_JOBS
        m_jobs_port = new JobPort(jobs);
    #endif
    #ifdef USE_NETWORK_DEBUGSTREAM
        m_vision_port = new TcpPort(VISION_PORT);
    #endif
}

/*! @brief Creates the team communications port
    
 This function is virtual because simulators will need to implement this differently.
 */
void NUIO::createTeamPort(TeamInformation* teaminfo)
{
    m_team_port = new TeamPort(teaminfo, TEAM_PORT);
}

NUIO::~NUIO()
{
#if DEBUG_NETWORK_VERBOSITY > 4
    debug << "NUIO::~NUIO()" << endl;
#endif
    if (m_gamecontroller_port != NULL)
        delete m_gamecontroller_port;
    if (m_team_port != NULL)
        delete m_team_port;
    if (m_vision_port != NULL)
        delete m_vision_port;
    if (m_jobs_port != NULL)
        delete m_jobs_port;
}

/*! @brief Stream insertion operator for a JobList
    @param io the nuio stream object
    @param jobs the job list to put in the stream
 */
NUIO& operator<<(NUIO& io, JobList& jobs)
{
    #ifdef USE_NETWORK_JOBS
        (*io.m_jobs_port) << jobs;
    #endif
    return io;
}

/*! @brief Stream insertion operator for a pointer to a JobList
    @param io the nuio stream object
    @param jobs the pointer to the job list to put in the stream
 */
NUIO& operator<<(NUIO& io, JobList* jobs)
{
    #ifdef USE_NETWORK_JOBS
        (*io.m_jobs_port) << jobs;
    #endif
    return io;
}

/*! @brief Sets the target ip address for the job port
    @param ipaddress the new target ip address.
 */
void NUIO::setJobPortTargetAddress(string ipaddress)
{
    if (m_jobs_port != NULL)
        m_jobs_port->setTargetAddress(ipaddress);
    else
        errorlog << "NUIO::setJobPortToBroadcast(). Network jobs are OFF" << endl;
}

/*! @brief Sets the job port to broadcast to all robots
 */
void NUIO::setJobPortToBroadcast()
{
    if (m_jobs_port != NULL)
        m_jobs_port->setBroadcast();
    else
        errorlog << "NUIO::setJobPortToBroadcast(). Network jobs are OFF" << endl;
}

/*! @brief Stream insertion operator for NUimage
    @param io the nuio stream object
    @param sensors the NUimage data to stream
 */
NUIO& operator<<(NUIO& io, NUimage& p_image)
{
    #ifdef USE_NETWORK_DEBUGSTREAM
        network_data_t netdata = io.m_vision_port->receiveData();
        if(netdata.size > 0)
        {
            io.m_vision_port->sendData(p_image);
        }
    #endif
    return io;
}

/*! @brief Stream insertion operator for a pointer to NUimage
    @param io the nuio stream object
    @param sensors the pointer to the NUimage data to put in the stream
 */
NUIO& operator<<(NUIO& io, NUimage* p_image)
{
    io << *p_image;
    return io;
}

