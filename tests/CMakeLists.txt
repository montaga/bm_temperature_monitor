cmake_minimum_required(VERSION 3.14)
project(temperature_monitor_tests)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

include(GoogleTest)

# Option to build ADC ISR tests instead of full temperature monitor tests
option(BUILD_ADC_ISR_TESTS "Build ADC ISR tests instead of full temperature monitor tests" OFF)

if(BUILD_ADC_ISR_TESTS)
  # Add ADC ISR test executable (without EMULATED_ISR)
  add_executable(
    adc_isr_tests
    test_adc_isr.cpp
    mock_adc.c
  )

  # Add C source files for adc_isr_tests
  target_sources(adc_isr_tests PRIVATE
    ../project/common/ringbuffer.c
    ../project/bsp/isr.c
  )

  # Set C standard for C files
  set_source_files_properties(
    mock_adc.c
    ../project/common/ringbuffer.c
    ../project/bsp/isr.c
    PROPERTIES LANGUAGE C
    C_STANDARD 11
  )

  # Include directories
  target_include_directories(adc_isr_tests PRIVATE
    ../project
    .
  )

  # Link GoogleTest
  target_link_libraries(
    adc_isr_tests
    GTest::gtest_main
  )

  # Add compile definitions
  target_compile_definitions(adc_isr_tests PRIVATE
    HAL_IMPL_MOCK
  )

  gtest_discover_tests(adc_isr_tests)
else()
  # Add test executable
  add_executable(
    temperature_monitor_tests
    test_temp_monitor.cpp
    test_config_loader.cpp
    test_ringbuffer.cpp
    mock_gpio.c
  )

  # Add C source files for temperature_monitor_tests
  target_sources(temperature_monitor_tests PRIVATE
    ../project/app/temp_monitor.c
    ../project/app/config_loader.c
    ../project/common/ringbuffer.c
    ../project/hal/mock/adc.c
    ../project/hal/mock/i2c.c
    ../project/bsp/isr.c
  )

  # Set C standard for C files
  set_source_files_properties(
    mock_gpio.c
    ../project/app/temp_monitor.c
    ../project/app/config_loader.c
    ../project/common/ringbuffer.c
    ../project/hal/mock/adc.c
    ../project/hal/mock/i2c.c
    ../project/bsp/isr.c
    PROPERTIES LANGUAGE C
    C_STANDARD 11
  )

  # Include directories
  target_include_directories(temperature_monitor_tests PRIVATE
    ../project
    .
  )

  # Link GoogleTest
  target_link_libraries(
    temperature_monitor_tests
    GTest::gtest_main
    m  # math library for sinf, etc.
    pthread
  )

  # Add compile definitions
  target_compile_definitions(temperature_monitor_tests PRIVATE
    EMULATED_ISR
    HAL_IMPL_MOCK
  )

  gtest_discover_tests(temperature_monitor_tests)
endif()

include(GoogleTest)