//
// Created by sprin on 6/23/2023.
//

#ifndef FWDASSAULTNEW_STRUCT_H
#define FWDASSAULTNEW_STRUCT_H



namespace Struct {
    namespace Offsets{
        namespace Component{
            uintptr_t get_transform = 0;
        }
        namespace Transform{
            uintptr_t get_position = 0;
        }
        namespace Camera{
            uintptr_t get_main = 0;
            uintptr_t WorldToScreenPoint = 0;
        }
        namespace MissionProgressData{
            uintptr_t get_count = 0;
        }
        namespace Physics{
            uintptr_t Raycast = 0;
        }
    }

    namespace Functions{
        int movementSpeed = 10;
        bool isAutoQDailyW = false;
    }
}


#endif //FWDASSAULTNEW_STRUCT_H
