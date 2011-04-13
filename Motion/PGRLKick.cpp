/*! @file PGRLKick.h
    @brief Declaration of PGRL kick script optimizer

    @class PGRLKick
    @brief A PGRL kick script optimizer.

    @author Josiah Walker

  Copyright (c) 2010 Jason Kulk

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

#include "PGRLKick.h"
#include <string>


PGRLKick::PGRLKick(NUWalk* walk, NUSensorsData* data, NUActionatorsData* actions) {
    m_name="PGRLKick";
}

void PGRLKick::process(NUSensorsData* data, NUActionatorsData* actions) {
	
	MotionScript* kick = new MotionScript("kick");
	
	#ifdef USE_WALK
    if (m_walk)
        m_walk->kill();
    #endif
    
    kick->play(m_data, m_actions);
	m_completion_time = kick->timeFinished();
	m_head_completion_time = kick->timeFinishedWithHead();
	m_arm_completion_time = max(kick->timeFinishedWithLArm(), kick->timeFinishedWithRArm());
	
	delete kick
}

void PGRLKick::stop();
void PGRLKick::stopLegs();
void PGRLKick::kill();

bool PGRLKick::isActive();

