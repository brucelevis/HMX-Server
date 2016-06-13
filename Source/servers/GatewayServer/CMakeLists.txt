########### DBServer ###############

file(GLOB_RECURSE GatewayServer_SRCS *.h *.cpp *.cc)

# precompiled header
if (USE_COREPCH)
  set(GatewayServer_PCH_HDR GatewayServer_PCH.h)
  set(GatewayServer_PCH_SRC GatewayServer_PCH.cpp)
endif ()

include_directories(
    ${CMAKE_SOURCE_DIR}/servers/Shared/CommonDef
    ${CMAKE_SOURCE_DIR}/servers/Shared/Protocol
    ${CMAKE_SOURCE_DIR}/servers/Shared/Utility
	
	${CMAKE_SOURCE_DIR}/servers/DyNetSocket
	${CMAKE_SOURCE_DIR}/servers/DyNetMysql
	
    ${BOOST_INCLUDE_DIR}
	${MYSQL_INCLUDE_DIR}
)

add_executable(GatewayServer
    ${GatewayServer_SRCS}
)

target_link_libraries(GatewayServer
    Shared
	LIBSocket
    ${BOOST_LIBRARIES}
)

IF(UNIX)
    target_link_libraries(GatewayServer
        pthread
        rt
        z
        dl
    )

    if( CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../Bin/Debug/)
    else()
        SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../Bin/Release/)
    endif()
ENDIF(UNIX)

if( WIN32 )
  if ( MSVC )
    SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../Bin/)

    add_custom_command(TARGET GatewayServer
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fep_cfg.xml $(TargetDir)
	  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fep_cfg.xml ${PROJECT_BINARY_DIR}/servers/GatewayServer
    )
  endif()
endif()

# Generate precompiled header
if (USE_COREPCH)
        add_cxx_pch(GatewayServer ${GatewayServer_PCH_HDR} ${GatewayServer_PCH_SRC})
endif ()
