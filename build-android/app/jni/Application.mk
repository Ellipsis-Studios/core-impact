APP_STL := c++_static 
APP_PLATFORM := android-23
APP_ABI := arm64-v8a armeabi-v7a x86 x86_64
APP_CPPFLAGS += -std=c++17
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti
APP_CPPFLAGS += -Wno-conversion-null
APP_SHORT_COMMANDS := true