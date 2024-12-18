#include <list>
#include <vector>
#include <string>
#include <pthread.h>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include <Logger.h>
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_android.h>

#include <android/native_activity.h>
#include <android/input.h>
#include <android/native_activity.h>
#include <android/native_window_jni.h>
#include <android/looper.h>
#include <android/log.h>
#include <android/native_window.h>
#include "KittyMemory/MemoryPatch.h"
#include "Menu.h"
#include <Classes.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
//Target lib here

#include "Includes/Macros.h"

typedef int (*InitializeMotionEventFunc)(void *thiz, void *motionEvent, const void *inputMessage);

// Function pointer to hold the original initializeMotionEvent function
InitializeMotionEventFunc origInitializeMotionEvent = nullptr;

// Custom implementation of the initializeMotionEvent function
int myInitializeMotionEvent(void *thiz, void *motionEvent, const void *inputMessage) {
    // Call the original initializeMotionEvent function
    int result = origInitializeMotionEvent(thiz, motionEvent, inputMessage);

    // Extract touch event information
    AInputEvent *inputEvent = (AInputEvent *)thiz;

    // LOGI("inputEvent %p", inputEvent);
    int32_t action = AMotionEvent_getAction(inputEvent);
    int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
    int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    int32_t pointerCount = AMotionEvent_getPointerCount(inputEvent);

    // Process touch event based on the action
    switch (actionMasked) {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN: {
            // Touch down event
            int32_t pointerId = AMotionEvent_getPointerId(inputEvent, pointerIndex);
            float x = AMotionEvent_getX(inputEvent, pointerIndex);
            float y = AMotionEvent_getY(inputEvent, pointerIndex);
            // LOGI("X : %f Y : %f", x, y);
            // Call the ImGui_ImplAndroid_HandleInputEvent function with the touch event information
            ImGui_ImplAndroid_HandleInputEvent(static_cast<int>(x), static_cast<int>(y), AMOTION_EVENT_ACTION_DOWN);
            break;
        }
        case AMOTION_EVENT_ACTION_MOVE: {
            // Touch move event
            for (int32_t i = 0; i < pointerCount; ++i) {
                int32_t pointerId = AMotionEvent_getPointerId(inputEvent, i);
                float x = AMotionEvent_getX(inputEvent, i);
                float y = AMotionEvent_getY(inputEvent, i);

                //LOGI("X : %f Y : %f", x, y);
                // Call the ImGui_ImplAndroid_HandleInputEvent function with the touch event information
                ImGui_ImplAndroid_HandleInputEvent(static_cast<int>(x), static_cast<int>(y), AMOTION_EVENT_ACTION_MOVE);
            }
            break;
        }
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP: {
            // Touch up event
            int32_t pointerId = AMotionEvent_getPointerId(inputEvent, pointerIndex);
            float x = AMotionEvent_getX(inputEvent, pointerIndex);
            float y = AMotionEvent_getY(inputEvent, pointerIndex);

            // LOGI("X : %f Y : %f", x, y);
            // Call the ImGui_ImplAndroid_HandleInputEvent function with the touch event information
            ImGui_ImplAndroid_HandleInputEvent(static_cast<int>(x), static_cast<int>(y), AMOTION_EVENT_ACTION_UP);
            break;
        }
            // Add additional cases as needed for other touch event types

        default:
            break;
    }

    return result;
}
void SetupImgui() {
    if(!bInitDone)return;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2((float)Screen::get_width(), (float)Screen::get_height());

    // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.3f;
    style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 0;

    style.Colors[ImGuiCol_Text]                  = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.0f, 0.0f, 0.0, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_Button]           = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
    style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
    style.Colors[ImGuiCol_ButtonActive]     = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDimBg]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    style.Colors[ImGuiCol_ChildBg] = ImColor(0,0,0);
    style.Colors[ImGuiCol_Text] = ImColor(255,255,255);
    style.Colors[ImGuiCol_Header] = ImColor(30,30,30);
    style.Colors[ImGuiCol_HeaderActive] = ImColor(28,28,28);
    style.Colors[ImGuiCol_HeaderHovered] = ImColor(28,28,28);

    style.Colors[ImGuiCol_Button] = ImColor(36, 36, 36);
    style.Colors[ImGuiCol_ButtonActive] = ImColor(40, 40, 40);
    style.Colors[ImGuiCol_ButtonHovered] = ImColor(40, 40,40);

    style.Colors[ImGuiCol_FrameBg] = ImColor(30, 30, 30);
    style.Colors[ImGuiCol_FrameBgActive] = ImColor(28, 28, 28);
    style.Colors[ImGuiCol_FrameBgHovered] = ImColor(28, 28, 28);

    style.Colors[ImGuiCol_TitleBg] = ImColor(30, 30, 30);
    style.Colors[ImGuiCol_TitleBgActive] = ImColor(28, 28, 28);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(28, 28, 28);
    style.ScrollbarSize = 10.0f; // Set the size of the scrollbar to 10 pixels
    // Setup Platform/Renderer backends
    ImGui_ImplOpenGL3_Init("#version 100");

    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
    ImFontConfig icons_config;

    ImFontConfig CustomFont;
    CustomFont.FontDataOwnedByAtlas = false;
    CustomFont.SizePixels = 50.0f; // Set desired font size

    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.OversampleH = 2.5;
    icons_config.OversampleV = 2.5;
    icons_config.SizePixels = 35.0f; // Set desired font size

    CustomFont.FontDataOwnedByAtlas = false;

    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.OversampleH = 2.5;
    icons_config.OversampleV = 2.5;

    // We load the default font with increased size to improve readability on many devices with "high" DPI.
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 60.0f;
    io.Fonts->AddFontDefault(&font_cfg);

    // Arbitrary scale-up
    ImGui::GetStyle().ScaleAllSizes(3.0f);
}

// Hook Functions
int (*old_MissionProgressData_get_Count)(void *instance);
int MissionProgressData_get_Count(void *instance) {
    // Always return a fixed value
    return 100;
}

// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    while (!g_il2cpp) {
        g_il2cpp = findLibrary("libil2cpp.so");
        sleep(1);
    }

    LOGI("libil2cpp.so: %p", g_il2cpp);

    Il2Cpp::Attach();

    sleep(5);

    // Save offsets
    Struct::Offsets::MissionProgressData::count = (uintptr_t) Il2Cpp::GetFieldOffset("Assembly-CSharp.dll", "Celes2","MissionProgressData", "count");
    // Our Hook
    // DobbyHook(reinterpret_cast<void *>(reinterpret_cast<void *>(g_il2cpp + 0x191B5CC), (void *) MissionProgressData_get_Count, (void **) &old_MissionProgressData_get_Count), reinterpret_cast<dobby_dummy_func_t>(replacement_func), reinterpret_cast<dobby_dummy_func_t *>(&origin_func));
    // HOOK(g_il2cpp + 0x191B5CC, MissionProgressData_get_Count, old_MissionProgressData_get_Count);

    bInitDone = true;

    void *sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));

    if (nullptr != sym_input){
        origInitializeMotionEvent = reinterpret_cast<InitializeMotionEventFunc>(sym_input);
        DobbyHook((void *)origInitializeMotionEvent, reinterpret_cast<dobby_dummy_func_t>(myInitializeMotionEvent), reinterpret_cast<dobby_dummy_func_t*>(&origInitializeMotionEvent));
    }
    
    return nullptr;
}

jobjectArray getFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    //Toasts added here so it's harder to remove it
    MakeToast(env, context, OBFUSCATE("Modded by Eikarna"), Toast::LENGTH_LONG);

    const char *features[] = {
            OBFUSCATE("Category_Auto"),//Not counted
            OBFUSCATE("2_Toggle_Auto Daily/Weekly Quest"),//1
            OBFUSCATE("3_SeekBar_Movement Speed (TODO)_0_100"),//14

    };

    //Now you dont have to manually update the number everytime;
    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,jint featNum, jstring featName, jint value,jboolean boolean, jstring str) {

    switch (featNum) {
        case 2:
            Struct::Functions::autoQDailyWeekly = boolean;
            break;
	case 3:
            Struct::Functions::moveSpeed = value;
            break;
    }
}

__attribute__((constructor))
void lib_main() {
    pthread_t ptid;
    pthread_create(&ptid, nullptr, hack_thread, nullptr);
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/MainActivity"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);

    jclass c = globalEnv->FindClass("com/android/support/Menu");
    if (c != nullptr){
        static const JNINativeMethod menuMethods[] = {
              {OBFUSCATE("RichWebView"), OBFUSCATE("(Ljava/lang/String;)Landroid/view/View;"), reinterpret_cast<void *>(RichWebView)},
              {OBFUSCATE("RichTextView"), OBFUSCATE("(Ljava/lang/String;)Landroid/view/View;"), reinterpret_cast<void *>(RichTextView)},
              {OBFUSCATE("Category"), OBFUSCATE("(Ljava/lang/String;)Landroid/view/View;"), reinterpret_cast<void *>(Category)},
              {OBFUSCATE("ButtonLink"), OBFUSCATE("(Ljava/lang/String;Ljava/lang/String;)Landroid/view/View;"), reinterpret_cast<void *>(ButtonLink)},
              {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
              {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
              {OBFUSCATE("isGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
              {OBFUSCATE("InitializeField"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(InitializeField)},
              {OBFUSCATE("isNotInGame"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isNotInGame)},
              {OBFUSCATE("AddViews"),  OBFUSCATE("()V"), reinterpret_cast<void *>(AddViews)},
              {OBFUSCATE("FeatureLister"),  OBFUSCATE("([Ljava/lang/String;Landroid/widget/LinearLayout;)V"), reinterpret_cast<void *>(FeatureLister)},
              {OBFUSCATE("isViewCollapsed"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isViewCollapsed)},
              {OBFUSCATE("Thread"),  OBFUSCATE("(Landroid/widget/FrameLayout;Landroid/content/Context;)V"), reinterpret_cast<void *>(Thread)},
              {OBFUSCATE("onDestroy"),  OBFUSCATE("(Landroid/widget/FrameLayout;Landroid/view/WindowManager;)V"), reinterpret_cast<void *>(onDestroy)},
              {OBFUSCATE("dp"),  OBFUSCATE("(I)I"), reinterpret_cast<void *>(dp)},
              {OBFUSCATE("onStartCommandn"), OBFUSCATE("(Landroid/content/Intent;II)I"), reinterpret_cast<void *>(onStartCommand)},
              {OBFUSCATE("convertDipToPixels"),  OBFUSCATE("(I)I"), reinterpret_cast<void *>(convertDipToPixels)},
              {OBFUSCATE("setHeadingText"),  OBFUSCATE("(Landroid/widget/TextView;)V"), reinterpret_cast<void *>(setHeadingText)},
              {OBFUSCATE("setTitleText"),  OBFUSCATE("(Landroid/widget/TextView;)V"), reinterpret_cast<void *>(setTitleText)},
              {OBFUSCATE("MakeWindow"),  OBFUSCATE("()V"), reinterpret_cast<void *>(MakeWindow)},
              {OBFUSCATE("MakeLogo"), OBFUSCATE("()V"), reinterpret_cast<void *>(MakeLogo)},
              {OBFUSCATE("MakeMenu"), OBFUSCATE("()V"), reinterpret_cast<void *>(MakeMenu)},
              {OBFUSCATE("settingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(settingsList)},
              {OBFUSCATE("getFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(getFeatureList)},

        };

        int mm = globalEnv->RegisterNatives(c, menuMethods, sizeof(menuMethods) / sizeof(JNINativeMethod));
        if (mm != JNI_OK) {
            LOGE(OBFUSCATE("Menu methods error"));
            return mm;
        }
    }
    else{
        LOGE(OBFUSCATE("JNI error"));
        return JNI_ERR;
    }

    jclass p = globalEnv->FindClass( OBFUSCATE("com/android/support/Preferences"));
    if (p != nullptr){
        static const JNINativeMethod prefmethods[] = {
                { OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
        };

        int pm = globalEnv->RegisterNatives(p, prefmethods, sizeof(prefmethods) / sizeof(JNINativeMethod));
        if (pm != JNI_OK){
            LOGE(OBFUSCATE("Preferences methods error"));
            return pm;
        }
    }
    else{
        LOGE(OBFUSCATE("JNI error"));
        return JNI_ERR;
    }

    if (RegisterMain(globalEnv) != 0)
        return JNI_ERR;

    return JNI_VERSION_1_6;
}
