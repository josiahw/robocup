/*! @file ScriptedPoseProvider.cpp
    @brief Implementation of Pose behaviour class

    @author Josh Wilson
 
 Copyright (c) 2010 Josh Wilson
 
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

#include "ScriptedPoseProvider.h"

#include "Infrastructure/Jobs/JobList.h"
#include "Infrastructure/NUActionatorsData/NUActionatorsData.h"
#include "Infrastructure/NUBlackboard.h"
#include "Infrastructure/GameInformation/GameInformation.h"

#include "Infrastructure/Jobs/MotionJobs/HeadJob.h"
#include "Infrastructure/Jobs/MotionJobs/WalkJob.h"
#include "Infrastructure/Jobs/MotionJobs/MotionFreezeJob.h"
#include "Infrastructure/Jobs/VisionJobs/SaveImagesJob.h"


#include <math.h>
#include "debug.h"
#include "debugverbositybehaviour.h"

using namespace std;

ScriptedPoseProvider::ScriptedPoseProvider(Behaviour* manager) : BehaviourProvider(manager)
{
    m_pitch_index = 0;
    m_yaw_index = 25;
    m_num_pitch_motions = 15;
    m_num_yaw_motions = 49;
    isStart = 0;
    m_saving_images = false;
    
    //XXX: add params, etc
    int rounds = 0;
    float fitness = 0.0;
    vector<Parameter>* v = new vector();
    
    //data for one pose step
    v->add(new Parameter("TimeStepSize",val,0.,1.));
    v->add(new Parameter("LShoulderRoll",0.5,-0.3141,1.326));
    v->add(new Parameter("RShoulderRoll",1.0,-0.3141,1.326));
    v->add(new Parameter("LHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("RHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("LHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("RHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("LHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("RHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("LKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("RKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("LAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("RAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("LAnkleRoll",val,-0.7689,0.3977));
    v->add(new Parameter("RAnkleRoll",val,-0.7689,0.3977));
    
    v->add(new Parameter("TimeStepSize",val,0.,1.));
    v->add(new Parameter("LShoulderRoll",0.5,-0.3141,1.326));
    v->add(new Parameter("RShoulderRoll",1.0,-0.3141,1.326));
    v->add(new Parameter("LHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("RHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("LHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("RHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("LHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("RHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("LKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("RKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("LAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("RAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("LAnkleRoll",val,-0.7689,0.3977));
    v->add(new Parameter("RAnkleRoll",val,-0.7689,0.3977));
    
    v->add(new Parameter("TimeStepSize",val,0.,1.));
    v->add(new Parameter("LShoulderRoll",0.5,-0.3141,1.326));
    v->add(new Parameter("RShoulderRoll",1.0,-0.3141,1.326));
    v->add(new Parameter("LHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("RHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("LHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("RHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("LHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("RHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("LKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("RKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("LAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("RAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("LAnkleRoll",val,-0.7689,0.3977));
    v->add(new Parameter("RAnkleRoll",val,-0.7689,0.3977));
    
    v->add(new Parameter("TimeStepSize",val,0.,1.));
    v->add(new Parameter("LShoulderRoll",0.5,-0.3141,1.326));
    v->add(new Parameter("RShoulderRoll",1.0,-0.3141,1.326));
    v->add(new Parameter("LHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("RHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("LHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("RHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("LHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("RHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("LKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("RKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("LAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("RAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("LAnkleRoll",val,-0.7689,0.3977));
    v->add(new Parameter("RAnkleRoll",val,-0.7689,0.3977));
    
    v->add(new Parameter("TimeStepSize",val,0.,1.));
    v->add(new Parameter("LShoulderRoll",0.5,-0.3141,1.326));
    v->add(new Parameter("RShoulderRoll",1.0,-0.3141,1.326));
    v->add(new Parameter("LHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("RHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("LHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("RHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("LHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("RHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("LKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("RKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("LAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("RAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("LAnkleRoll",val,-0.7689,0.3977));
    v->add(new Parameter("RAnkleRoll",val,-0.7689,0.3977));
    
    v->add(new Parameter("TimeStepSize",val,0.,1.));
    v->add(new Parameter("LShoulderRoll",0.5,-0.3141,1.326));
    v->add(new Parameter("RShoulderRoll",1.0,-0.3141,1.326));
    v->add(new Parameter("LHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("RHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("LHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("RHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("LHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("RHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("LKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("RKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("LAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("RAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("LAnkleRoll",val,-0.7689,0.3977));
    v->add(new Parameter("RAnkleRoll",val,-0.7689,0.3977));
    
    v->add(new Parameter("TimeStepSize",val,0.,1.));
    v->add(new Parameter("LShoulderRoll",0.5,-0.3141,1.326));
    v->add(new Parameter("RShoulderRoll",1.0,-0.3141,1.326));
    v->add(new Parameter("LHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("RHipYawPitch",val,-1.1452,0.74));
    v->add(new Parameter("LHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("RHipPitch",val,-1.773,0.4839));
    v->add(new Parameter("LHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("RHipRoll",val,-0.3776,0.7904));
    v->add(new Parameter("LKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("RKneePitch",val,-0.0923,2.112));
    v->add(new Parameter("LAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("RAnklePitch",val,-1.189,0.9225));
    v->add(new Parameter("LAnkleRoll",val,-0.7689,0.3977));
    v->add(new Parameter("RAnkleRoll",val,-0.7689,0.3977));
    
    optimiser = new PGRLOptimiser("ScriptOptimiser",v);

    m_script = MotionScript("kick");
    m_script_playing = false;

}


ScriptedPoseProvider::~ScriptedPoseProvider()
{

}

void ScriptedPoseProvider::doBehaviour()
{
    //Saving images
    if(singleChestClick())

    {
	m_saving_images = true;
        m_jobs->addVisionJob(new SaveImagesJob(m_saving_images, true));
	

    }
    else if(m_saving_images == true)
    {
	m_saving_images = false;

        m_jobs->addVisionJob(new SaveImagesJob(m_saving_images, true));
    }
    

    doSelectedMotion();
    

}

void ScriptedPoseProvider::doSelectedMotion()
{
    //Initialisation: First 50 Frames, will be used to stand up
    /*if (isStart < 50)
    {
        vector<float> zero(m_actions->getSize(NUActionatorsData::Head), 0);
        m_actions->add(NUActionatorsData::Head, m_current_time, zero, 50);
        m_jobs->addMotionJob(new WalkJob(0.001,0.001,0.001));
		isStart++;
    }
    else if (isStart < 200)
    {
        m_jobs->addMotionJob(new MotionFreezeJob());
    	isStart++;
    }
    //Start the behaviour:
    else 
    {*/
    

    if (!m_script_playing and Blackboard->GameInfo->getCurrentState() == GameInformation::PlayingState) 
    {
		m_script.play(m_data, m_actions);

		m_script_playing = true;
        //m_jobs->addMotionJob(new WalkJob(0,0,0));
    } else if (Blackboard->GameInfo->getCurrentState() != GameInformation::PlayingState) {
		m_script_playing = false;
		
		
		//XXX: add inter-round reload of script for new params
	}
}
