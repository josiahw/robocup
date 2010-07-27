/*! @file NUbot.cpp
    @brief Implementation of top-level NUbot class

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

#include "NUbot.h"

// ---------------------------------------------------------------- Compulsory header files
#include "NUPlatform/NUPlatform.h"
#include "NUPlatform/NUIO.h"
#include "Infrastructure/NUBlackboard.h"
#include "Infrastructure/NUSensorsData/NUSensorsData.h"
#include "Infrastructure/NUActionatorsData/NUActionatorsData.h"
#include "NUPlatform/NUActionators/NUSounds.h"
#include "Infrastructure/FieldObjects/FieldObjects.h"
#include "Infrastructure/Jobs/Jobs.h"
#include "Infrastructure/GameInformation/GameInformation.h"
#include "Infrastructure/TeamInformation/TeamInformation.h"

#include "debugverbositynubot.h"
#include "debug.h"

// --------------------------------------------------------------- Module header files
#ifdef USE_VISION
    #include "Infrastructure/NUImage/NUImage.h"
    #include "Vision/Vision.h"
#endif

#ifdef USE_BEHAVIOUR
    #include "Behaviour/Behaviour.h"
#endif

#ifdef USE_LOCALISATION
    #include "Localisation/Localisation.h"
#endif

#ifdef USE_MOTION
    #include "Motion/NUMotion.h"
#endif

// --------------------------------------------------------------- Thread header files
#if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
    #include "NUbot/SeeThinkThread.h"
#endif
#include "NUbot/SenseMoveThread.h"
#include "NUbot/WatchDogThread.h"

// --------------------------------------------------------------- NUPlatform header files
#if defined(TARGET_IS_NAOWEBOTS)
    #include "NUPlatform/Platforms/NAOWebots/NAOWebotsPlatform.h"
    #include "NUPlatform/Platforms/NAOWebots/NAOWebotsIO.h"
#elif defined(TARGET_IS_NAO)
    #include "NUPlatform/Platforms/NAO/NAOPlatform.h"
    #include "NUPlatform/Platforms/NAO/NAOIO.h"
#elif defined(TARGET_IS_CYCLOID)
    #include "NUPlatform/Platforms/Cycloid/CycloidPlatform.h"
    #include "NUPlatform/Platforms/Cycloid/CycloidIO.h"
#elif defined(TARGET_IS_NUVIEW)
    #error You should not be compiling NUbot.cpp when targeting NUview, you should use the virtualNUbot.
#else
    #error There is no platform (TARGET_IS_${}) defined
#endif

#include <time.h>
#include <signal.h>
#include <string>
#include <sstream>
#include <unistd.h>

#ifndef TARGET_OS_IS_WINDOWS
    #include <errno.h>
#endif
#ifndef TARGET_OS_IS_WINDOWS
    #include <execinfo.h>
#endif

NUbot* NUbot::m_this = NULL;

/*! @brief Constructor for the nubot
    
    The parameters are for command line arguements. Webots gives the binary arguements which tell us the 
    robot's number. Consequently, these args are passed down to the webots platform.
 
    @param argc the number of command line arguements
    @param *argv[] the array of command line arguements
 */
NUbot::NUbot(int argc, const char *argv[])
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::NUbot()." << endl;
    #endif
    NUbot::m_this = this;
    
    createErrorHandling();
    createPlatform(argc, argv);
    createBlackboard();
    createNetwork();
    createModules();
    createThreads();
    
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::NUbot(). Finished." << endl;
    #endif
}

/*! @brief Destructor for the nubot
 */
NUbot::~NUbot()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::~NUbot()." << endl;
    #endif
    
    #ifdef USE_MOTION
        m_motion->kill();
        m_platform->kill();
        m_platform->msleep(1500);
    #endif
    
    destroyThreads();
    destroyModules();
    destroyNetwork();
    destroyBlackboard();
    destroyPlatform();
    
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::~NUbot(). Finished!" << endl;
    #endif
}

/*! @brief Connects error signals with the termination handler
 */
void NUbot::createErrorHandling()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::createErrorHandling()." << endl;
    #endif
    #ifndef TARGET_OS_IS_WINDOWS
        signal(SIGILL, terminationHandler);
        signal(SIGSEGV, terminationHandler);
        signal(SIGBUS, terminationHandler); 
        signal(SIGABRT, terminationHandler);
    #endif
}

/*! @brief Creates the Platform (Robot hardware interface) */
void NUbot::createPlatform(int argc, const char *argv[])
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::createPlatform()." << endl;
    #endif
    
    #if defined(TARGET_IS_NAOWEBOTS)
        m_platform = new NAOWebotsPlatform(argc, argv);
    #elif defined(TARGET_IS_NAO)
        m_platform = new NAOPlatform();
    #elif defined(TARGET_IS_CYCLOID)
        m_platform = new CycloidPlatform();
    #endif
}

/*! @brief Destroys the Platform, aka delete the m_platform */
void NUbot::destroyPlatform()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::destroyPlatform()." << endl;
    #endif
    
    delete m_platform;
    m_platform = 0;
}

/*! @brief Creates the Blackboard (the public data storage class) 
 
    This sets the m_blackboard and also set the global pointer Blackboard. Use the global, to access the blackboard
 */
void NUbot::createBlackboard()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::createBlackboard()." << endl;
    #endif
    
    m_blackboard = new NUBlackboard();
    m_blackboard->add(m_platform->getNUSensorsData());
    m_blackboard->add(m_platform->getNUActionatorsData());
    m_blackboard->add(new FieldObjects());
    m_blackboard->add(new JobList());
    m_blackboard->add(new GameInformation(m_platform->getRobotNumber(), m_platform->getTeamNumber()));
    m_blackboard->add(new TeamInformation(m_platform->getRobotNumber(), m_platform->getTeamNumber()));
}

/*! @brief Destroys the Blackboard, aka delete the m_blackboard */
void NUbot::destroyBlackboard()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::destroyBlackboard()." << endl;
    #endif
    
    delete m_blackboard;
    m_blackboard = 0;
}

/*! @brief Creates the Network */
void NUbot::createNetwork()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::createNetwork()." << endl;
    #endif
    
    #if defined(TARGET_IS_NAOWEBOTS)
        m_io = new NAOWebotsIO(this, dynamic_cast<NAOWebotsPlatform*>(m_platform));
    #elif defined(TARGET_IS_NAO)
        m_io = new NAOIO(this);
    #elif defined(TARGET_IS_CYCLOID)
        m_io = new CycloidIO(this);
    #endif
}

/*! @brief Destroys the Network, aka delete the m_io */
void NUbot::destroyNetwork()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::destroyNetwork()." << endl;
    #endif
    
    delete m_io;
    m_io = 0;
}

/*! @brief Creates the Modules, this includes Vision, Localisation, Behaviour and Motion */
void NUbot::createModules()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::createModules()." << endl;
    #endif
    
    #ifdef USE_VISION
        m_vision = new Vision();
    #endif
        
    #ifdef USE_LOCALISATION
        #if defined(TARGET_IS_NAOWEBOTS)
            m_localisation = new Localisation(Platform->getRobotNumber());
        #else
            m_localisation = new Localisation();
        #endif // defined(TARGET_IS_NAOWEBOTS)
    #endif
        
    #ifdef USE_BEHAVIOUR
        m_behaviour = new Behaviour();
    #endif
        
    #ifdef USE_MOTION
        m_motion = new NUMotion(m_blackboard->Sensors, m_blackboard->Actions);
    #endif
}

/*! @brief Destroys all of the modules, aka delete m_vision, m_localisation, m_behaviour, m_motion */
void NUbot::destroyModules()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::destroyModules()." << endl;
    #endif
    
    #ifdef USE_VISION
        delete m_vision;
        m_vision = 0;
    #endif
        
    #ifdef USE_LOCALISATION
        delete m_localisation;
        m_localisation = 0;
    #endif
        
    #ifdef USE_BEHAVIOUR
        delete m_behaviour;
        m_behaviour = 0;
    #endif
        
    #ifdef USE_MOTION
        delete m_motion;
        m_motion = 0;
    #endif
}

/*! @brief Create nubot's threads
 */
void NUbot::createThreads()
{
#if DEBUG_NUBOT_VERBOSITY > 1
    debug << "NUbot::createThreads(). Constructing threads." << endl;
#endif
    
    #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
        m_seethink_thread = new SeeThinkThread(this);
    #endif
        
    m_sensemove_thread = new SenseMoveThread(this);
    m_sensemove_thread->start();
    
    #ifndef TARGET_IS_NAOWEBOTS
        m_watchdog_thread = new WatchDogThread(this);
        m_watchdog_thread->start();
    #endif
    
    #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
        m_seethink_thread->start();
    #endif

#if DEBUG_NUBOT_VERBOSITY > 1
    debug << "NUbot::createThreads(). Finished." << endl;
#endif
}

/*! @brief Destroys the nubot's threads */
void NUbot::destroyThreads()
{
    #if DEBUG_NUBOT_VERBOSITY > 0
        debug << "NUbot::destroyThreads()." << endl;
    #endif
    
    #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
        m_seethink_thread->stop();
    #endif
    #ifndef TARGET_IS_NAOWEBOTS
        m_watchdog_thread->stop();
    #endif
    m_sensemove_thread->stop();
    
    #ifndef TARGET_IS_NAOWEBOTS
        delete m_watchdog_thread;
        m_watchdog_thread = 0;
    #endif
    
    delete m_sensemove_thread;
    m_sensemove_thread = 0;
    
    #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
        delete m_seethink_thread;
        m_seethink_thread = 0;
    #endif
}

/*! @brief The nubot's main loop
    
    The nubot's main loop. This function will probably never return.
 
    The idea is to simply have 
    @verbatim
    NUbot* nubot = new NUbot(arc, argv);
    nubot->run();
    delete nubot;
    @endverbatim

 */
void NUbot::run()
{
#if defined(TARGET_IS_NAOWEBOTS)
    int count = 0;
    double previoussimtime;
    NAOWebotsPlatform* webots = (NAOWebotsPlatform*) m_platform;
    int timestep = int(webots->getBasicTimeStep());
    while (true)
    {
        previoussimtime = Platform->getTime();
        webots->step(timestep);           // stepping the simulator generates new data to run motion, and vision data
        #if defined(USE_MOTION)
            m_sensemove_thread->startLoop();
        #endif
        
        #if defined(USE_VISION) or defined(USE_LOCALISATION) or defined(USE_BEHAVIOUR) or defined(USE_MOTION)
            if (count%2 == 0)           // depending on the selected frame rate vision might not need to be updated every simulation step
            {
                m_seethink_thread->startLoop();         
                m_seethink_thread->waitForLoopCompletion();
            }
        #endif
        
        #if defined(USE_MOTION)
            m_sensemove_thread->waitForLoopCompletion();
        #endif
        
        count++;
    };
#else
    #if !defined(USE_VISION) and (defined(USE_BEHAVIOUR) or defined(USE_LOCALISATION) or defined(USE_MOTION))
        while (true)
        {
            periodicSleep(33);
            m_seethink_thread->startLoop();
            m_seethink_thread->waitForLoopCompletion();
        }
    #endif
#endif
}

void NUbot::periodicSleep(int period)
{
    static double starttime = Platform->getTime();
    double timenow = Platform->getTime();
    double requiredsleeptime = period - (timenow - starttime);
    if (requiredsleeptime > 0)
        Platform->msleep(requiredsleeptime);
    starttime = Platform->getTime();
}

/*! @brief Handles unexpected termination signals
 */
void NUbot::terminationHandler(int signum)
{
    errorlog << "TERMINATION HANDLER: ";
    debug << "TERMINATION HANDLER: ";
    cout << "TERMINATION HANDLER: ";
    
    #ifndef TARGET_OS_IS_WINDOWS
        if (signum == SIGILL)
        {
            errorlog << "SIGILL" << endl;
            debug << "SIGILL" << endl;
            cout << "SIGILL" << endl;
        }
        else if (signum == SIGSEGV)
        {
            errorlog << "SIGSEGV" << endl;
            debug << "SIGSEGV" << endl;
            cout << "SIGSEGV" << endl;
        }
        else if (signum == SIGBUS)
        {
            errorlog << "SIGBUS" << endl;
            debug << "SIGBUS" << endl;
            cout << "SIGBUS" << endl;
        }
        else if (signum == SIGABRT)
        {
            errorlog << "SIGABRT" << endl;
            debug << "SIGABRT" << endl;
            cout << "SIGABRT" << endl;
        }
    #else
        errorlog << endl;
        debug << endl;
        cout << endl;
    #endif
    
    #ifndef TARGET_OS_IS_WINDOWS
        void *array[10];
        size_t size;
        char **strings;
        size = backtrace(array, 10);
        strings = backtrace_symbols(array, size);
        for (size_t i=0; i<size; i++)
            errorlog << strings[i] << endl;
    #endif
    
    if (NUbot::m_this != NULL)
    {
        // safely kill motion
        #ifdef USE_MOTION
            NUbot::m_this->m_motion->kill();
        #endif
        
        // play sound to indicate the error
        #ifndef TARGET_OS_IS_WINDOWS
            if (signum == SIGILL)
                Blackboard->Actions->addSound(0, NUSounds::ILLEGAL_INSTRUCTION);
            else if (signum == SIGSEGV)
                Blackboard->Actions->addSound(0, NUSounds::SEG_FAULT);
            else if (signum == SIGBUS)
                Blackboard->Actions->addSound(0, NUSounds::BUS_ERROR);
            else if (signum == SIGABRT)
                Blackboard->Actions->addSound(0, NUSounds::ABORT);
        #endif
        NUbot::m_this->m_platform->kill();
        NUbot::m_this->m_platform->msleep(1500);
    }
    #ifndef TARGET_OS_IS_WINDOWS
        signal(signum, SIG_DFL);
        raise(signum);
    #endif
}

/*! @brief 'Handles an unhandled exception; logs the backtrace to errorlog
    @param e the exception
 */
void NUbot::unhandledExceptionHandler(exception& e)
{
	#ifndef TARGET_OS_IS_WINDOWS
        //!< @todo TODO: check whether the exception is serious, if it is fail safely
        Blackboard->Actions->addSound(0, NUSounds::UNHANDLED_EXCEPTION);
        errorlog << "UNHANDLED EXCEPTION. " << endl;
        debug << "UNHANDLED EXCEPTION. " << endl; 
        void *array[10];
        size_t size;
        char **strings;
        size = backtrace(array, 10);
        strings = backtrace_symbols(array, size);
        for (size_t i=0; i<size; i++)
            errorlog << strings[i] << endl;
        errorlog << e.what() << endl;
	#endif
}


