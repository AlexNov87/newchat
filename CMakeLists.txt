cmake_minimum_required(VERSION 3.21)
set(CMAKE_VERBOSE_MAKEFILE ON)
list(APPEND CMAKE_PREFIX_PATH "F:/COMPILESOFT/vcpkg/installed/x64-mingw-static")

project(chat CXX)
set(CMAKE_CXX_STANDARD 23)


find_package(Poco COMPONENTS Net Foundation REQUIRED)


include(bst.cmake)

set(service 
const.h const.cpp
service.h service.cpp 
service_action_check.cpp
tokenizer.h tokenizer.cpp )

set(u
user_foo.cpp
)

set(cliserv 
srv.h 
chatroom.cpp
chatuser.cpp

abstract_sess.cpp
chatroomsess.cpp 
server_session.cpp

mainserv.cpp
server_responce.cpp
mainserv_operations.cpp
)

set(vars
   ${service}  ${u} ${cliserv}
)
add_executable(ex server.cpp ${vars} )

target_link_libraries(ex PRIVATE ${Boost_LIBRARIES} Poco::Net Poco::Foundation)


add_definitions(-DPOCO_STATIC)
add_executable(cl cli.cpp  ${vars} )
target_link_libraries(cl ${Boost_LIBRARIES})


if(Poco_FOUND)
MESSAGE(STATUS "!!!!!!!!!!!!!POCO FOUND!!  !!!!!!!!!!!!")
MESSAGE(STATUS "-->>" ${Poco_LIBRARIES} "   " ${Poco_DIR})
else()
MESSAGE(STATUS "POCO NOT  FOUND")
endif()