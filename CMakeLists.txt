cmake_minimum_required(VERSION 3.29)
project(petHttpServ)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0167 OLD)
find_package(Boost REQUIRED)


add_executable(petHttpServ main.cpp)

target_link_libraries(petHttpServ PRIVATE Boost::boost)

if(WIN32)
    target_link_libraries(petHttpServ PRIVATE
            ws2_32
            mswsock
            wsock32
    )

    target_compile_definitions(petHttpServ PRIVATE
            _WIN32_WINNT=0x0A00
    )
elseif(UNIX)
    target_link_libraries(petHttpServ PRIVATE
            pthread
    )

    find_library(RT_LIBRARY rt)
    if(RT_LIBRARY)
        target_link_libraries(petHttpServ PRIVATE ${RT_LIBRARY})
    endif()
endif()

if(MSVC)
    target_compile_options(petHttpServ PRIVATE /W4)
else()
    target_compile_options(petHttpServ PRIVATE -Wall -Wextra -Wpedantic)
endif()