//
// Created by springmusk on 6/23/2023.
//

#ifndef FWDASSAULTNEW_CLASSES_H
#define FWDASSAULTNEW_CLASSES_H

#include <Il2Cpp.h>
#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Quaternion.hpp>
#include <Struct.h>

bool clearMousePos = true, setup = false;

uintptr_t g_il2cpp = 0;
bool bInitDone = false;

class Screen {
public:
    static int get_width() {
        auto Screen_get_width = (int (*)()) (g_il2cpp+0x290B080);
        return Screen_get_width();
    }
    static int get_height() {
        auto Screen_get_height = (int (*)()) (g_il2cpp+0x290B0C8);
        return Screen_get_height();
    }
};

#endif //FWDASSAULTNEW_CLASSES_H
