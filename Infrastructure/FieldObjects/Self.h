#ifndef FIELDOBJECTS_SELF_H
#define FIELDOBJECTS_SELF_H

#include "../../Tools/Math/Vector3.h"
#include <vector>
#include <iostream>
class StationaryObject;
class MobileObject;

class Self {
	private:
		Vector3<float> WorldModelLocation;
		Vector3<float> WorldModelLocationError;
	bool amILost;
	public:
		Self();
		~Self();
		Self(float x, float y);
        void updateLocationOfSelf(float wmX, float wmY, float heading, float sdX, float sdY, float sdHeading,bool lost);
		float wmX(){return WorldModelLocation[0];}
		float wmY(){return WorldModelLocation[1];}
		float Heading(){return WorldModelLocation[2];}
		std::vector<float> wmState();
        float sdX() {return WorldModelLocationError[0];}
        float sdY() {return WorldModelLocationError[1];}
        float sdHeading() {return WorldModelLocationError[2];}
        bool lost();
    
        std::vector<float> CalculateDifferenceFromFieldState(const std::vector<float> desiredState);
        std::vector<float> CalculateDifferenceFromFieldLocation(const std::vector<float> desiredLocation);
        std::vector<float> CalculateDifferenceFromStationaryObject(const StationaryObject& theObject);
        float CalculateDistanceToStationaryObject(const StationaryObject& theObject);
        float CalculateBearingToStationaryObject(const StationaryObject& theObject);
    
        std::vector<float> CalculateDifferenceFromGoal(const StationaryObject& goalpost);
        float CalculateAngularWidthOfGoal(const StationaryObject& goalpost);
        float CalculateAngularWidthOfGoalFromMobileObject(const StationaryObject& goalpost, const MobileObject& mobileobject);
        bool sdHeadingLessThanGoalWidth(const StationaryObject& goalpost, float num_stddev = 2);
    
        std::vector<float> CalculateClosestInterceptToMobileObject(const MobileObject& theObject);
        float CalculateYInterceptOfMobileObject(const MobileObject& theObject);
		float CalculateXInterceptOfMobileObject(const MobileObject& theObject);
        std::vector<float> CalculatePositionBetweenMobileObjectAndGoal(const MobileObject& mobileobject, const StationaryObject& goalpost, float distancefrommobile);
        std::vector<float> CalculatePositionToProtectGoalFromMobileObject(const MobileObject& mobileobject, const StationaryObject& goalpost, float blockingwidth);

        /*!
        @brief Output streaming operation.
        @param output The output stream.
        @param p_loc The source localisation data to be streamed.
        */
        friend std::ostream& operator<< (std::ostream& output, const Self& p_self);

        /*!
        @brief Input streaming operation.
        @param input The input stream.
        @param p_kf The destination localisation data to be streamed to.
        */
        friend std::istream& operator>> (std::istream& input, Self& p_self);
};

#endif
