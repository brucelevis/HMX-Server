file(GLOB_RECURSE headers_CommonDef_H CommonDef/*.h)
file(GLOB_RECURSE headers_Protocol_H Protocol/*.h)
file(GLOB_RECURSE headers_Utility_H Utility/*.h)

file(GLOB_RECURSE sources_CommonDef_CC CommonDef/*.cc)
file(GLOB_RECURSE sources_CommonDef_CPP CommonDef/*.cpp)
file(GLOB_RECURSE sources_Protocol_CC Protocol/*.cc)
file(GLOB_RECURSE sources_Protocol_CPP Protocol/*.cpp)
file(GLOB_RECURSE sources_Utility_CC Utility/*.cc)
file(GLOB_RECURSE sources_Utility_CPP Utility/*.cpp)

include_directories(
  ${MYSQL_INCLUDE_DIR}
  ${BOOST_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/servers/DyNetSocket
  ${CMAKE_SOURCE_DIR}/servers/DyNetMysql
  ${CMAKE_SOURCE_DIR}/servers/Shared/CommonDef
  ${CMAKE_SOURCE_DIR}/servers/Shared/Protocol
  ${CMAKE_SOURCE_DIR}/servers/Shared/Utility
  ${PROTOBUL_INCLUDE_DIR}
)

set(Shared_SRCS
  ${headers_CommonDef_H}
  ${headers_Protocol_H}
  ${headers_Utility_H}
  ${sources_CommonDef_CC}
  ${sources_CommonDef_CPP}
  ${sources_Protocol_CC}
  ${sources_Protocol_CPP}
  ${sources_Utility_CC}
  ${sources_Utility_CPP}
)

add_library(Shared STATIC
  ${Shared_SRCS}
)

target_link_libraries(Shared
    ${BOOST_LIBRARIES}
)

if( WIN32 )
  if ( MSVC )
    SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../Bin/)
  endif()
endif()
