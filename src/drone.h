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

    ANOTATION_LOGIC("DRONE ACTION TYPES", "ACTION DEFINITION", {
        This Enumeration is used to define the action types of the drone.
        The first 2 bits are used to define the action type.
        
        to extract only the first 2 bits use the following mask:
        
        DRONE_ACTION_DATA_RANGE = bitmask(31, 30)
    })

    /* ACTION parameter */
    DRONE_ACTION_DATA_RANGE = 0b11000000000000000000000000000000, // drone action use 2bit[31:30]
    DRONE_ACTION_UNKNOWN    = 0b00000000000000000000000000000000,

    DRONE_ACTION_LANDED     = 0b00000000000000000000000000000000,
    DRONE_ACTION_THROTTLE   = 0b10000000000000000000000000000000,

    DRONE_ACTION_MOVEING    = 0b01000000000000000000000000000000,
    DRONE_ACTION_HOVERING   = 0b00000000000000000000000000000000,


    ANOTATION_LOGIC("DRONE ACTION TYPES", "LOGIC DEFINITION", {
        This Enumeration is used to define the LOGIC types of the drone.
        
        Theere are 6 types of logic:

        1. DRONE_LOGIC_PERFORMING
        
        2. DRONE_LOGIC_EXECUTE_VIDEO_RECORDING
        
        3. DRONE_LOGIC_EXECUTE_FOLLOW_HUMAN
        
        4. DRONE_LOGIC_EXECUTE_REGISTER_FACE
        
        5. DRONE_LOGIC_EXECUTE_REGISTER_OBJECT

        6. DRONE_LOGIC_EXECUTE_NAVIGATE (there is 2 types of logic)

            - navigation_logic_normal : potential field path planning
            - navigation_logic_swing  : potential field path planning + swing by path
            
            [2 of them are used to define the logic type.]

        use 3 bit[29:27] to define the logic type.
        And use 1 one to recognize the logic type.
    })

    /* LOGIC parameter */
    DRONE_LOGIC_PERFORMING_DATA_RANGE   = 0b00111111111111111111111111111111, // drone logic use 2bit[29:28]
    DRONE_LOGIC_PERFORMING              = 0b00100000000000000000000000000000,
    
    DRONE_LOGIC_EXECUTE_VIDEO_RECORDING = 0b00010000000000000000000000000000,
    DRONE_LOGIC_EXECUTE_FOLLOW_HUMAN    = 0b00001000000000000000000000000000,
    
    DRONE_LOGIC_EXECUTE_REGISTER_FACE   = 0b00000100000000000000000000000000,
    DRONE_LOGIC_EXECUTE_REGISTER_OBJECT = 0b00000010000000000000000000000000,
    
    DRONE_LOGIC_EXECUTE_NAVIGATE        = 0b00000001000000000000000000000000,
    DRONE_LOGIC_EXECUTE_NAVIGATE_NORMAL = 0b00000000100000000000000000000000,
    DRONE_LOGIC_EXECUTE_NAVIGATE_SWING  = 0b00000000010000000000000000000000,

    ANOTATION_LOGIC("DRONE ACTION TYPES", "RESERVED", {
        /*
        This Enumeration is used to define the LOGIC types of the drone.
        DRONE_CONTEXT_RESERVED_MASK is used to define the reserved mask.

        range is 0b00000000001111111111111111111111
        bitmask(31, 2) is used to define the reserved mask.
        */
    })


    DRONE_CONTEXT_RESERVED_MASK = 0b00000000001111111111111111111111

} DroneContext;



__forceinline DroneContext set_action_type(DroneContext action, DroneContext action_type)
{
    return (DroneContext)((action & ~DRONE_ACTION_DATA_RANGE) | (action_type & DRONE_ACTION_DATA_RANGE));
}


__forceinline DroneContext extract_action_type(DroneContext action)
{
    return (DroneContext)(action & DRONE_ACTION_DATA_RANGE);
}


__forceinline DroneContext extract_logic_type(DroneContext action)
{
    return (DroneContext)(action & DRONE_LOGIC_PERFORMING_DATA_RANGE);
}

__forceinline DroneContext extract_reserved_type(DroneContext action)
{
    return (DroneContext)(action & DRONE_CONTEXT_RESERVED_MASK);
}


#if defined(__cplusplus)
}
#endif

#endif