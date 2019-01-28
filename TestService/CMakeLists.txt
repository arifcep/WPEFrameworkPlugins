set(PLUGIN_NAME TestService)
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})

message("Setting up ${PLUGIN_NAME}")

set(PLUGIN_OPTIONS)

set(PLUGIN_SOURCES
        Module.cpp
        TestService.cpp
        CommandCore/TestCommandController.cpp
        TestUtilityImp.cpp
        Commands/Malloc.cpp
        Commands/Free.cpp
        Commands/Statm.cpp
#        Commands/Crash.cpp
)

# Library definition section
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})
add_library(${MODULE_NAME} SHARED ${PLUGIN_SOURCES})
target_link_libraries(${MODULE_NAME} ${PLUGINS_LIBRARIES})

# Library installation section
string(TOLOWER ${NAMESPACE} STORAGENAME)
install(TARGETS ${MODULE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${STORAGENAME}/plugins)

write_config(${PLUGIN_NAME} ${PLUGIN_NAME})
