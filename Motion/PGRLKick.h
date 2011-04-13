/*! @file PGRLKick.h
    @brief Declaration of PGRL kick script optimizer

    @class PGRLKick
    @brief A PGRL kick script optimizer.

    @author Josiah Walker

  Copyright (c) 2011 Josiah Walker

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

#ifndef PGRL_KICK_H
#define PGRL_KICK_H

#include "NUMotionProvider.h"
#include <string>

class PGRLKick: public NUMotionProvider
{
public:
    PGRLKick(NUWalk* walk, NUSensorsData* data, NUActionatorsData* actions);

    /*! @brief Returns the name of this motion provider */
    std::string getName() {
        return m_name;}

    void process(NUSensorsData* data, NUActionatorsData* actions);

    void stop();
    void stopHead() {;}
    void stopArms() {;}
    void stopLegs();
    void kill();

    bool isActive();
    virtual bool isUsingHead() {return false;}
    virtual bool isUsingArms() {return false;}
    virtual bool isUsingLegs() {return isActive();}

    bool isReady() {return true;}
    bool requiresHead() {return false;}
    bool requiresArms() {return false;}
    bool requiresLegs() {return true;}
protected:
    std::string m_name;
    NUSensorsData* m_data;
    NUActionatorsData* m_actions;
};

#endif
