########### DBServer ###############

file(GLOB_RECURSE LoginServer_SRCS *.h *.cpp *.cc)

# precompiled header
if (USE_COREPCH)
  set(LoginServer_PCH_HDR LoginServer_PCH.h)
  set(LoginServer_PCH_SRC LoginServer_PCH.cpp)
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

add_executable(LoginServer
    ${LoginServer_SRCS}
)

target_link_libraries(LoginServer
    Shared
	LIBSocket
    ${BOOST_LIBRARIES}
)

IF(UNIX)
    target_link_libraries(LoginServer
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

    add_custom_command(TARGET LoginServer
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ls_cfg.xml $(TargetDir)
	  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ls_cfg.xml ${PROJECT_BINARY_DIR}/servers/LoginServer
    )
  endif()
endif()

# Generate precompiled header
if (USE_COREPCH)
        add_cxx_pch(LoginServer ${LoginServer_PCH_HDR} ${LoginServer_PCH_SRC})
endif ()
