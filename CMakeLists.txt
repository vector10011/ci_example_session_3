cmake_minimum_required(VERSION 3.22)
################################## BASE SETUP ###############################################################
include("${CMAKE_CURRENT_SOURCE_DIR}/base-setup.cmake")

string(SUBSTRING "${DEVICE}" 0 11 DEVICE)

string(TOLOWER ${DEVICE} STM32_DEVICE_LC)
string(TOUPPER ${DEVICE} STM32_DEVICE_UC)
message(STATUS "Device_LC: " ${STM32_DEVICE_LC})
message(STATUS "Device_UC: " ${STM32_DEVICE_UC})

string(SUBSTRING "${STM32_DEVICE_UC}" 0 7 STM32_CORE)
string(SUBSTRING "${STM32_DEVICE_UC}" 0 11 STM32_FLASH_def) # todo
string(SUBSTRING "${STM32_DEVICE_UC}" 0 9 STM32_MODEL_UC)
string(SUBSTRING "${STM32_DEVICE_LC}" 0 9 STM32_MODEL_LC)

set(c_target "${STM32_MODEL_UC}xx")

# Toolchain
include("${CMAKE_CURRENT_LIST_DIR}/cmake/gcc-arm-none-eabi.cmake")

################################## PROJECT SETUP ###############################################################
project(${PROJECT_NAME}
	VERSION ${VER}
	DESCRIPTION "${DESC}"
	LANGUAGES C CXX ASM
)

# Общие стандарты (можно переопределять из cache/preset)
set(CMAKE_C_STANDARD 11 CACHE STRING "C standard")
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Полезно при работе с IDE и статанализаторами
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Device: ${DEVICE}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

############################# MCU / CPU parameters + CMSIS ##################################

# Linker script
set(linker_script_SRC
	${CMAKE_SOURCE_DIR}/cmsis-core/linker/${STM32_DEVICE_LC}.ld
)
# Add sources to executable
list(APPEND sources_SRCS 
	${CMAKE_SOURCE_DIR}/cmsis-core/startup/vector_${STM32_MODEL_LC}xx.c
	${CMAKE_SOURCE_DIR}/cmsis-core/startup/startup_common.c

	${CMAKE_SOURCE_DIR}/no_system_files/sysmem.c
	${CMAKE_SOURCE_DIR}/no_system_files/syscalls.c
)

# Include CMSIS / Device paths
list(APPEND include_DIRS
	${CMAKE_SOURCE_DIR}/cmsis-core/Include
	${CMAKE_SOURCE_DIR}/cmsis-core/Device/Include
)

################################## TARGET DEFINITIONS ###############################################################
add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${sources_SRCS})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${include_DIRS})

target_compile_definitions( ${CMAKE_PROJECT_NAME} PRIVATE
	${symbols_SYMB}
	${c_target}
	${STM32_DEVICE_UC}
	${STM32_CORE}
	${STM32_FLASH_def}
	$<$<COMPILE_LANGUAGE:C>: ${symbols_c_SYMB}>
	$<$<COMPILE_LANGUAGE:CXX>: ${symbols_cxx_SYMB}>
	$<$<COMPILE_LANGUAGE:ASM>: ${symbols_asm_SYMB}>
)

################################## COMPILER OPTIONS ################################################################ Compiler options
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
	-mthumb
	${main_cpu_PARAMS}
	${cpu_PARAMS}
	${compiler_OPTS}
	-Wall
	-Wextra
	-Wpedantic
	-Wno-unused-parameter
	# -Wno-volatile
	# -Wold-style-cast
	# -Wuseless-cast
	# -Wsuggest-override
	# $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP>
	$<$<CONFIG:Debug>:-Og -g3 -ggdb>
	$<$<CONFIG:Release>:-Os -g0>
)

################################## LINK OPTIONS ###############################################################
target_link_libraries(${CMAKE_PROJECT_NAME} ${link_LIBS})

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
	-T${linker_script_SRC}
	${main_cpu_PARAMS}
	${cpu_PARAMS}
	${linker_OPTS}
	"-Wl,-Map=${CMAKE_PROJECT_NAME}.map"
	"-u _printf_float" # STDIO float formatting support (remove if not used)
	"--specs=nosys.specs"
	"-Wl,--start-group"
	"-lc"
	"-lm"
	"-lstdc++"
	"-lsupc++"
	"-Wl,--end-group"
	"-Wl,-z,max-page-size=8" # Allow good software remapping across address space (with proper GCC section making)
	"-Wl,--print-memory-usage"
)

################################## POST ###############################################################

# Execute post-build to print size, generate hex and bin
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
	COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
	COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${CMAKE_PROJECT_NAME}> > ${CMAKE_PROJECT_NAME}.dis
)