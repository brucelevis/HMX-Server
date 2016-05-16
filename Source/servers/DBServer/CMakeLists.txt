########### DBServer ###############

file(GLOB_RECURSE DBServer_SRCS *.h *.cpp *.cc)

# precompiled header
if (USE_COREPCH)
  set(DBServer_PCH_HDR DBServer_PCH.h)
  set(DBServer_PCH_SRC DBServer_PCH.cpp)
endif ()

include_directories(
    ${CMAKE_SOURCE_DIR}/servers/Shared/CommonDef
    ${CMAKE_SOURCE_DIR}/servers/Shared/Protocol
    ${CMAKE_SOURCE_DIR}/servers/Shared/Utility
	
	${CMAKE_SOURCE_DIR}/servers/DyNetSocket
	${CMAKE_SOURCE_DIR}/servers/DyNetMysql
	
    ${MYSQL_INCLUDE_DIR}
    ${BOOST_INCLUDE_DIR}
)

add_executable(DBServer
    ${DBServer_SRCS}
)

target_link_libraries(DBServer
    Shared
	LIBSocket
	LIBMysql
    ${MYSQL_LIBRARIES}
    ${BOOST_LIBRARIES}
)

IF(UNIX)
    target_link_libraries(DBServer
        pthread
        rt
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

    add_custom_command(TARGET DBServer
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dp_cfg.xml $(TargetDir)
	  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dp_cfg.xml ${PROJECT_BINARY_DIR}/servers/DBServer
    )
  endif()
endif()

# Generate precompiled header
if (USE_COREPCH)
        add_cxx_pch(DBServer ${DBServer_PCH_HDR} ${DBServer_PCH_SRC})
endif ()
