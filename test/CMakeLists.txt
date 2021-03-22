PROJECT(qrencode_test)

########################## Basic ##########################

FILE(GLOB_RECURSE INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

FILE(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

SET(ALL_FILES
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)
    
SOURCE_GROUP("Header Files" FILES "${INCLUDE_FILES}")
SOURCE_GROUP("Source Files" FILES "${SOURCE_FILES}")


INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/src
)

ADD_EXECUTABLE(
    ${PROJECT_NAME}
    ${ALL_FILES}
)
###########################################################

# Link external libraries.
TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    qrencode
)