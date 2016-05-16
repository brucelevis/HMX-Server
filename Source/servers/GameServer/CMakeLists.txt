########### DBServer ###############

file(GLOB_RECURSE GameServer_SRCS *.h *.cpp *.cc)

# precompiled header
if (USE_COREPCH)
  set(GameServer_PCH_HDR GameServer_PCH.h)
  set(GameServer_PCH_SRC GameServer_PCH.cpp)
endif ()

include_directories(
    ${CMAKE_SOURCE_DIR}/servers/Shared/CommonDef
    ${CMAKE_SOURCE_DIR}/servers/Shared/Protocol
    ${CMAKE_SOURCE_DIR}/servers/Shared/Utility
	
	${CMAKE_SOURCE_DIR}/servers/DyNetSocket
	${CMAKE_SOURCE_DIR}/servers/DyNetMysql
	
    ${BOOST_INCLUDE_DIR}
)

add_executable(GameServer
    ${GameServer_SRCS}
)

target_link_libraries(GameServer
    Shared
	LIBSocket
    ${BOOST_LIBRARIES}
)

IF(UNIX)
    target_link_libraries(GameServer
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

    add_custom_command(TARGET GameServer
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ss_cfg.xml $(TargetDir)
	  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ss_cfg.xml ${PROJECT_BINARY_DIR}/servers/GameServer
    )
  endif()
endif()

# Generate precompiled header
if (USE_COREPCH)
        add_cxx_pch(GameServer ${GameServer_PCH_HDR} ${GameServer_PCH_SRC})
endif ()
