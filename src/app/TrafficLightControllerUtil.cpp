#include "impl/app/TrafficLightControllerApp.hpp"

std::string TrafficLightControllerApp::lightPatternToString(TrafficLightPattern pattern)
{
    std::string lightPatternString = "";

    switch(pattern)
    {
        case TrafficLightPattern::NorthSouthTurning:
            lightPatternString = "NorthSouthTurning";
            break;

        case TrafficLightPattern::NorthSouthThrough:
            lightPatternString = "NorthSouthThrough";
            break;
    
        case TrafficLightPattern::EastWestTurning:
            lightPatternString = "EastWestTurning";
            break;
    
        case TrafficLightPattern::EastWestThrough:
            lightPatternString = "EastWestThrough";
            break;

        default:
            std::cout << "Not a valid traffic light state." << std::endl;
            break;
    }

    return lightPatternString;
}

std::string TrafficLightControllerApp::laneToString(Lane lane)
{
    std::string laneString = "";

    switch(lane)
    {
        case Lane::N_N:
            laneString = "N_N";
            break;

        case Lane::N_W:
            laneString = "N_W";
            break;
    
        case Lane::S_S:
            laneString = "S_S";
            break;
    
        case Lane::S_E:
            laneString = "S_E";
            break;

        case Lane::E_E:
            laneString = "E_E";
            break;

        case Lane::E_N:
            laneString = "E_N";
            break;

        case Lane::W_W:
            laneString = "W_W";
            break;

        case Lane::W_S:
            laneString = "W_S";
            break;

        default:
            std::cout << "Not a valid lane." << std::endl;
            break;
    }

    return laneString;
}

void TrafficLightControllerApp::populateLightStates()
{
    for (int i = 0; i < TrafficLightPattern::NUM_PATTERNS; i++)
    {
        lightStates_[i].pattern = static_cast<TrafficLightPattern>(i);
        lightStates_[i].isOn = false;
        lightStates_[i].areOpposingLanesClear = false;
        lightStates_[i].startTime = 0;
        lightStates_[i].activeTime = 0;
    }

    /// Ideally I would move these min/max values into a config file
    /// and load them at runtime with a ConfigClass...
    lightStates_[TrafficLightPattern::NorthSouthTurning].minActiveTime = 10;
    lightStates_[TrafficLightPattern::NorthSouthTurning].maxActiveTime = 60;

    lightStates_[TrafficLightPattern::NorthSouthThrough].minActiveTime = 30;
    lightStates_[TrafficLightPattern::NorthSouthThrough].maxActiveTime = 120;
    
    lightStates_[TrafficLightPattern::EastWestTurning].minActiveTime = 10;
    lightStates_[TrafficLightPattern::EastWestTurning].maxActiveTime = 30;
    
    lightStates_[TrafficLightPattern::EastWestThrough].minActiveTime = 30;
    lightStates_[TrafficLightPattern::EastWestThrough].maxActiveTime = 60;
}

void TrafficLightControllerApp::populateVehicleStates()
{
    for (int i = 0; i < Lane::COUNT; i++)
    {
        vehicleStates_[i].lane = static_cast<Lane>(i);
        vehicleStates_[i].isWaiting = false;
        vehicleStates_[i].arrivalTime = 0;
        vehicleStates_[i].waitTime = 0;
    }
}

void TrafficLightControllerApp::makeOpposingLanesMap()
{
    std::array<Lane, Lane::COUNT> northNorthOpposing = { Lane::PLACEHOLDER,
                                                         Lane::N_W,
                                                         Lane::PLACEHOLDER,
                                                         Lane::S_E,
                                                         Lane::E_E,
                                                         Lane::E_N,
                                                         Lane::W_W,
                                                         Lane::W_S
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::N_N, northNorthOpposing));

    std::array<Lane, Lane::COUNT> northWestOpposing = { Lane::N_N,
                                                        Lane::PLACEHOLDER,
                                                        Lane::S_S,
                                                        Lane::PLACEHOLDER,
                                                        Lane::E_E,
                                                        Lane::E_N,
                                                        Lane::W_W,
                                                        Lane::W_S
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::N_W, northWestOpposing));

    std::array<Lane, Lane::COUNT> southSouthOpposing = { Lane::PLACEHOLDER,
                                                         Lane::N_W,
                                                         Lane::PLACEHOLDER,
                                                         Lane::S_E,
                                                         Lane::E_E,
                                                         Lane::E_N,
                                                         Lane::W_W,
                                                         Lane::W_S
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::S_S, southSouthOpposing));

    std::array<Lane, Lane::COUNT> southEastOpposing = { Lane::N_N,
                                                        Lane::PLACEHOLDER,
                                                        Lane::S_S,
                                                        Lane::PLACEHOLDER,
                                                        Lane::E_E,
                                                        Lane::E_N,
                                                        Lane::W_W,
                                                        Lane::W_S
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::S_E, southEastOpposing));

    std::array<Lane, Lane::COUNT> eastEastOpposing = { Lane::N_N,
                                                       Lane::N_W,
                                                       Lane::S_S,
                                                       Lane::S_E,
                                                       Lane::PLACEHOLDER,
                                                       Lane::E_N,
                                                       Lane::PLACEHOLDER,
                                                       Lane::W_S
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::E_E, eastEastOpposing));

    std::array<Lane, Lane::COUNT> eastNorthOpposing = { Lane::N_N,
                                                        Lane::N_W,
                                                        Lane::S_S,
                                                        Lane::PLACEHOLDER,
                                                        Lane::E_E,
                                                        Lane::PLACEHOLDER,
                                                        Lane::W_W,
                                                        Lane::W_S
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::E_N, eastNorthOpposing));

    std::array<Lane, Lane::COUNT> westWestOpposing = { Lane::N_N,
                                                       Lane::N_W,
                                                       Lane::S_S,
                                                       Lane::S_E,
                                                       Lane::PLACEHOLDER,
                                                       Lane::E_N,
                                                       Lane::PLACEHOLDER,
                                                       Lane::W_S
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::W_W, westWestOpposing));

    std::array<Lane, Lane::COUNT> westSouthOpposing = { Lane::N_N,
                                                        Lane::N_W,
                                                        Lane::S_S,
                                                        Lane::S_E,
                                                        Lane::E_E,
                                                        Lane::PLACEHOLDER,
                                                        Lane::W_W,
                                                        Lane::PLACEHOLDER
                                                    };
    laneToOpposingLaneMap_.insert(std::pair<Lane, std::array<Lane, Lane::COUNT> >(Lane::W_S, westSouthOpposing));
}

void TrafficLightControllerApp::makePatternToLaneMap()
{
    std::array<Lane, Lane::COUNT> northSouthTurning = { Lane::PLACEHOLDER,
                                                        Lane::N_W,
                                                        Lane::PLACEHOLDER,
                                                        Lane::S_E,
                                                        Lane::PLACEHOLDER,
                                                        Lane::PLACEHOLDER,
                                                        Lane::PLACEHOLDER,
                                                        Lane::PLACEHOLDER
                                                    };
    patternToLaneMap_.insert(std::pair<TrafficLightPattern, std::array<Lane, Lane::COUNT> >(TrafficLightPattern::NorthSouthTurning, northSouthTurning));

    std::array<Lane, Lane::COUNT> northSouthThrough = { Lane::N_N,
                                                        Lane::PLACEHOLDER,
                                                        Lane::S_S,
                                                        Lane::PLACEHOLDER,
                                                        Lane::PLACEHOLDER,
                                                        Lane::PLACEHOLDER,
                                                        Lane::PLACEHOLDER,
                                                        Lane::PLACEHOLDER
                                                    };
    patternToLaneMap_.insert(std::pair<TrafficLightPattern, std::array<Lane, Lane::COUNT> >(TrafficLightPattern::NorthSouthThrough, northSouthThrough));

    std::array<Lane, Lane::COUNT> eastWestTurning = { Lane::PLACEHOLDER,
                                                      Lane::PLACEHOLDER,
                                                      Lane::PLACEHOLDER,
                                                      Lane::PLACEHOLDER,
                                                      Lane::PLACEHOLDER,
                                                      Lane::E_N,
                                                      Lane::PLACEHOLDER,
                                                      Lane::W_S
                                                    };
    patternToLaneMap_.insert(std::pair<TrafficLightPattern, std::array<Lane, Lane::COUNT> >(TrafficLightPattern::EastWestTurning, eastWestTurning));

    std::array<Lane, Lane::COUNT> eastWestThrough = { Lane::PLACEHOLDER,
                                                      Lane::PLACEHOLDER,
                                                      Lane::PLACEHOLDER,
                                                      Lane::PLACEHOLDER,
                                                      Lane::E_E,
                                                      Lane::PLACEHOLDER,
                                                      Lane::W_W,
                                                      Lane::PLACEHOLDER
                                                    };
    patternToLaneMap_.insert(std::pair<TrafficLightPattern, std::array<Lane, Lane::COUNT> >(TrafficLightPattern::EastWestThrough, eastWestThrough));
}
