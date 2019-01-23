set(PLUGIN_NAME TestService)
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})

message("Setting up ${PLUGIN_NAME}")

set(PLUGIN_OPTIONS)

set(PLUGIN_SOURCES
        Module.cpp
        TestService.cpp
        TestCommandControllerImp.cpp
        TestCommandController.cpp
#        TestCaseController.cpp
#        TestSuiteController.cpp
#        TestControllerImplementation.cpp
#        MemoryAllocationTS.cpp
#        CrashTS.cpp
)

# update cxx_flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Library definition section
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})
add_library(${MODULE_NAME} SHARED ${PLUGIN_SOURCES})
target_link_libraries(${MODULE_NAME} ${PLUGINS_LIBRARIES})

# Library installation section
string(TOLOWER ${NAMESPACE} STORAGENAME)
install(TARGETS ${MODULE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${STORAGENAME}/plugins)

write_config(${PLUGIN_NAME} ${PLUGIN_NAME})
