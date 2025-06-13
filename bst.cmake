set(Boost_USE_STATIC_LIBS ON)
#sta-boostsata  dyn-boostdyn
set(VAR sta)
set(BOOST_ROOT "F:/LIBSCPP/boost${VAR}") #где дирректория буста
set(BOOST_INCLUDEDIR "F:/LIBSCPP/boost${VAR}/include/boost-1_86") #где дирректория буста инклюдов
set(BOOST_LINK "F:/LIBSCPP/boost${VAR}/lib") 
 
include_directories(${BOOST_INCLUDEDIR}) #ДЛЯ ПОИСКА КОМПИЛЯЦИИ include
include_directories(${BOOST_ROOT})#ДЛЯ ПОИСКА КОРНЯ
link_directories(${BOOST_LINK})#ДЛЯ ПОИСКА ЛИНКОВОК
 
##ВАЖНО!!!! ЕСЛИ КОМПИЛЯТОР MINGW
if(MINGW)
  link_libraries(ws2_32 wsock32)
endif()
 
#КОМПОНЕНТЫ БУСТА
set (COMP_PACK 
   serialization
   json
   coroutine
)
 
find_package(Boost COMPONENTS ${COMP_PACK} REQUIRED)

if(Boost_FOUND)
MESSAGE(STATUS "-->>"${Boost_LIBRARIES})
MESSAGE(STATUS "-->>"${BOOST_LINK})
endif()