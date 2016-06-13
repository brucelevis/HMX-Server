########### DBServer ###############

file(GLOB_RECURSE WorldServer_SRCS *.h *.cpp *.cc)

# precompiled header
if (USE_COREPCH)
  set(WorldServer_PCH_HDR WorldServer_PCH.h)
  set(WorldServer_PCH_SRC WorldServer_PCH.cpp)
endif ()

include_directories(
    ${CMAKE_SOURCE_DIR}/servers/Shared/CommonDef
    ${CMAKE_SOURCE_DIR}/servers/Shared/Protocol
    ${CMAKE_SOURCE_DIR}/servers/Shared/Utility
	
	${CMAKE_SOURCE_DIR}/servers/DyNetSocket
	${CMAKE_SOURCE_DIR}/servers/DyNetMysql

    ${ENGINE_COMMON_INCLUDE_DIR}
    ${ENGINE_NET_INCLUDE_DIR}

    ${BOOST_INCLUDE_DIR}
	${MYSQL_INCLUDE_DIR}
    ${TCMALLOC_INCLUDE_DIR}
    ${LOG4CPLUS_INCLUDE_DIR}
    ${BREAKPAD_INCLUDE_DIR}

    ${TINYXML_INCLUDE_DIR}
)

add_executable(WorldServer
    ${WorldServer_SRCS}
)

target_link_libraries(WorldServer
    Shared
	LIBSocket
    ${BOOST_LIBRARIES}
)

IF(UNIX)
    target_link_libraries(WorldServer
        pthread
        rt
    )

    if( CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../Bin/Debug/)
    else()
        SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../Bin/Release/)
    endif()
ENDIF(UNIX)

if( WIN32 )
  if ( MSVC )
    SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../Bin/)
    add_custom_command(TARGET WorldServer
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ws_cfg.xml $(TargetDir)
	  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ws_cfg.xml ${PROJECT_BINARY_DIR}/servers/WorldServer
    )
	
  endif()
endif()

# Generate precompiled header
if (USE_COREPCH)
        add_cxx_pch(WorldServer ${WorldServer_PCH_HDR} ${WorldServer_PCH_SRC})
endif ()
