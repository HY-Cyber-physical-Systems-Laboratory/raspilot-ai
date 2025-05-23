#ifndef __DRONE_H__
#define __DRONE_H__


#include "stdint.h"
#include "module_test.h"


#if defined(__cplusplus)
extern "C" {

typedef enum : uint32_t // 32bit
#else
typedef enum
#endif
{


    ANOTATION_LOGIC("DRONE ACTION TYPES", " ACTION DEFINITION", ""
        This Enumeration is used to define the action types of the drone.
        The first 2 bits are used to define the action type.
        
        to extract only the first 2 bits use the following mask:
        
        DRONE_ACTION_DATA_RANGE = 0b11000000000000000000000000000000


    ")

    /* ACTION parameter */
    DRONE_ACTION_DATA_RANGE = 0b11000000000000000000000000000000, // drone action use 2bit[31:30]
    DRONE_ACTION_UNKNOWN    = 0b00000000000000000000000000000000,

    DRONE_ACTION_LANDED     = 0b00000000000000000000000000000000,
    DRONE_ACTION_THROTTLE   = 0b10000000000000000000000000000000,

    DRONE_ACTION_MOVEING    = 0b11000000000000000000000000000000,
    DRONE_ACTION_HOVERING   = 0b10000000000000000000000000000000,

} DroneActionType;



static DroneActionType extract_action_type(uint32_t action)
{
    return (DroneActionType)(action & DRONE_ACTION_DATA_RANGE);
}




#if defined(__cplusplus)
}
#endif

#endif