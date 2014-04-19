# ======================================================
# QsLog: Logging library
# in the root CMakeFile:
# add_subdirectory("3rparty/QsLog")
# add_executable(MagicPad WIN32 ... ${QSLOG_SRCS} ... )
# ======================================================

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(QSLOG_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/QsLog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QsLogDest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QsLogDestConsole.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QsLogDestFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/QsLogDestFunctor.cpp
    PARENT_SCOPE)
