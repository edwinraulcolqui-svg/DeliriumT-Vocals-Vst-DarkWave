cmake_minimum_required(VERSION 3.22)
project(DeliriumTVocals VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set JUCE_DIR to a local JUCE checkout, or allow CMake to fetch JUCE.
include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(DeliriumTVocals
    COMPANY_NAME "Dark Grave"
    BUNDLE_ID "com.darkgrave.deliriumtvocals"
    PLUGIN_MANUFACTURER_CODE DGrv
    PLUGIN_CODE DTV1
    FORMATS VST3 Standalone
    PRODUCT_NAME "DeliriumT Vocals"
    DESCRIPTION "Darkwave vocal processor by Dark Grave"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(DeliriumTVocals PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(DeliriumTVocals PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(DeliriumTVocals PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
