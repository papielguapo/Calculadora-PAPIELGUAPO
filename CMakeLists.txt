cmake_minimum_required(VERSION 3.22)
project(Calculadora1a1 VERSION 1.0.0)

if (NOT DEFINED JUCE_PATH)
    set(JUCE_PATH "C:/JUCE" CACHE PATH "Path to the JUCE source tree")
endif()
add_subdirectory("${JUCE_PATH}" JUCE)

juce_add_plugin(Calculadora1a1
    COMPANY_NAME "PAPIELGUAPO"
    BUNDLE_ID "com.papielguapo.calculadora"
    PLUGIN_MANUFACTURER_CODE Papi
    PLUGIN_CODE C101
    PRODUCT_NAME "Calculadora"
    DESCRIPTION "Calculadora visual interactiva"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS VST3 Standalone)

target_sources(Calculadora1a1 PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_features(Calculadora1a1 PRIVATE cxx_std_17)
target_compile_definitions(Calculadora1a1 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(Calculadora1a1 PRIVATE
    juce::juce_audio_utils
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)
