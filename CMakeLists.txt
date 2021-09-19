cmake_minimum_required(VERSION 3.14.0)

set(vst3sdk_SOURCE_DIR D:/VST/VST_SDK/VST3SDK)
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(SoundObject)

# suzumushi
unset(SMTG_CREATE_PLUGIN_LINK)
set (SMTG_CXX_STANDARD "20")

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(SoundObject     
    source/Aachen_KEMAR/IR_TBL_L_44.h
    source/Aachen_KEMAR/IR_TBL_R_44.h
    source/Aachen_KEMAR/IR_TBL_L_48.h
    source/Aachen_KEMAR/IR_TBL_R_48.h
    source/Aachen_KEMAR/IR_TBL_L_96.h
    source/Aachen_KEMAR/IR_TBL_R_96.h
    source/York_KEMAR/IR_TBL_L_44.h
    source/York_KEMAR/IR_TBL_R_44.h
    source/York_KEMAR/IR_TBL_L_48.h
    source/York_KEMAR/IR_TBL_R_48.h
    source/York_KEMAR/IR_TBL_L_96.h
    source/York_KEMAR/IR_TBL_R_96.h
    source/York_KU100/IR_TBL_L_44.h
    source/York_KU100/IR_TBL_R_44.h
    source/York_KU100/IR_TBL_L_48.h
    source/York_KU100/IR_TBL_R_48.h
    source/York_KU100/IR_TBL_L_96.h
    source/York_KU100/IR_TBL_R_96.h
    source/SO2ndordIIRfilters.h
    source/SOcids.h
    source/SOconfig.h
    source/SOcontroller.h
    source/SOcontroller.cpp
    source/SODDL.h
    source/SODSPparam.h
    source/SODSPparam.cpp
    source/SOentry.cpp
    source/SOextparam.h
    source/SOextparam.cpp
    source/SOHVLines.h
    source/SOHVLines.cpp
    source/SOHVLinescreator.cpp
    source/SOLPF.h
    source/SOparam.h
    source/SOpinna.h
    source/SOprocessor.h
    source/SOprocessor.cpp
    source/SOudsampling.h
    source/version.h
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
    target_sources(SoundObject
        PRIVATE
            resource/SOeditor.uidesc
    )
    target_link_libraries(SoundObject
        PRIVATE
            vstgui_support
    )
    smtg_add_vst3_resource(SoundObject "resource/SOeditor.uidesc")
endif()
# -------------------

smtg_add_vst3_snapshot(SoundObject resource/46570030EE935D3495AAEFCF49F7F2CF_snapshot.png)
smtg_add_vst3_snapshot(SoundObject resource/46570030EE935D3495AAEFCF49F7F2CF_snapshot_2.0x.png)


target_link_libraries(SoundObject
    PRIVATE
        sdk
)

if(SMTG_MAC)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
    smtg_set_bundle(SoundObject
		BUNDLE_IDENTIFIER foo
		INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS
    )
    smtg_set_debug_executable(SoundObject
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(SoundObject PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SoundObject)
    
        smtg_set_debug_executable(SoundObject
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif()
