﻿# CMakeList.txt : CMake project for X-Lang-Engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)

project ("xlang_eng")

# add_compile_definitions(XLANG_ENG_DBG=1)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories("../Api")
include_directories("../Inc")
include_directories("../utils")
include_directories("../Executor")
include_directories("../Ast")
include_directories("../Parse")
include_directories("../Serialize")
include_directories("../Ipc")
include_directories("../Thread")
include_directories("../Ext")
include_directories("../Core")
include_directories("../Text")
include_directories("../Scripts")
include_directories("../Tensor")
include_directories("../Jit")
include_directories("../log")
include_directories(".")

FILE(GLOB Api_SRC "../Api/value.cpp")
FILE(GLOB Utils_SRC "../utils/*.cpp")
FILE(GLOB Executor_SRC "../Executor/*.cpp")
FILE(GLOB Core_SRC "../Core/*.cpp")
FILE(GLOB Ast_SRC "../Ast/*.cpp")
FILE(GLOB Serialize_SRC "../Serialize/*.cpp")
FILE(GLOB Parse_SRC "../Parse/*.cpp")
FILE(GLOB Thread_SRC "../Thread/*.cpp")
FILE(GLOB Ext_SRC "../Ext/*.cpp")
FILE(GLOB Text_SRC "../Text/*.cpp")
FILE(GLOB Ipc_SRC "../Ipc/*.cpp")
FILE(GLOB Nn_SRC "../Tensor/*.cpp")
FILE(GLOB Jit_SRC "../Jit/*.cpp")
FILE(GLOB Main_SRC "*.cpp" "../log/log.cpp")

add_library(${PROJECT_NAME} SHARED 
	${Api_SRC}
	${Utils_SRC}
	${Executor_SRC}
	${Core_SRC}
	${Ast_SRC}
	${Parse_SRC}
	${Serialize_SRC}
	${Thread_SRC}
	${Ext_SRC}
	${Text_SRC}
	${Ipc_SRC}
	${Nn_SRC}
	${Jit_SRC}
	${Main_SRC}
)

if (WIN32)
	#Inheritance via dominance warning
	target_compile_options(${PROJECT_NAME} PRIVATE "/wd4250;")
else()
    target_link_libraries(${PROJECT_NAME} pthread dl)
endif()

#[[

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()
]]


#[[
add_library(${PROJECT_NAME}.mini SHARED 
	${Api_SRC}
	${Utils_SRC}
	${Core_SRC}
	${Ast_SRC}
	${Parse_SRC}
	${Main_SRC}
)

if (WIN32)
	#Inheritance via dominance warning
	target_compile_options(${PROJECT_NAME}.mini PRIVATE "/wd4250;")
else()
    target_link_libraries(${PROJECT_NAME}.mini pthread dl)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME}.mini PROPERTY CXX_STANDARD 20)
endif()
]]

