//
// Created by sprin on 6/23/2023.
//

#ifndef FWDASSAULTNEW_STRUCT_H
#define FWDASSAULTNEW_STRUCT_H



namespace Struct {
    namespace Offsets{
        namespace MissionProgressData {
            uintptr_t count = 0;
        }
        namespace Character {
            uintptr_t moveSpeedRate = 0;
        }
    }

    namespace Functions{
        int MAX_MOVE_SPEED = 100;
        float moveSpeed = 10;
        bool autoQDailyWeekly;
    }
}


#endif //FWDASSAULTNEW_STRUCT_H
