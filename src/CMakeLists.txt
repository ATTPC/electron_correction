add_executable(ecorr)

target_include_directories(ecorr PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/./
    ${ROOT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/nlohmann_json/include
)

target_sources(ecorr PRIVATE
    Config.h
    Config.cpp
    GenerateGas.h
    GenerateGas.cpp
    GenerateCorrection.h
    GenerateCorrection.cpp
    main.cpp
)

target_link_libraries(ecorr PRIVATE Garfield::Garfield ${ROOT_LIBRARIES})

set_target_properties(ecorr PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ECORR_BINARY_DIR})