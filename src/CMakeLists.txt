CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(hopin)

# Tell CMake to run moc when necessary:
SET(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)

include_directories("qslog")
add_subdirectory("qslog")

SET(SOURCES main.cpp /
            mainwindow.cpp /
            controller.cpp /
            network.cpp /
            miner.cpp /
            requestsender.cpp )

# For Apple set the icns file containing icons
IF(APPLE)
  # icon files to copy in the bundle
  SET( OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/images/hopin.icns )
  # set where in the bundle to put the icns files
  SET_SOURCE_FILES_PROPERTIES( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  SET( CPUMINER ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/pooler-cpuminer-2.3.2-osx64/minerd )

  SET_SOURCE_FILES_PROPERTIES(
    ${CPUMINER}
    PROPERTIES
    MACOSX_PACKAGE_LOCATION
    MacOS/bin
  )

  QT5_WRAP_UI(UI_HEADERS mainwindow.ui)

  QT5_ADD_RESOURCES (RESOURCES ../resources/res.qrc)

  ADD_EXECUTABLE(hopin MACOSX_BUNDLE ${QSLOG_SRCS} ${OSX_ICON_FILES} ${CPUMINER} ${SOURCES} ${UI_HEADERS} ${RESOURCES})

  QT5_USE_MODULES(hopin Widgets Network)

  SET_TARGET_PROPERTIES( hopin PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../resources/mac/Info.plist )

ENDIF(APPLE)

IF(WIN32)

  set(CMAKE_CXX_FLAGS "-mwindows")

  #seting windows icon
  IF( MINGW )
    # resource compilation for MinGW
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hopinico.o
                        COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../resources/win.rc
                             -o ${CMAKE_CURRENT_BINARY_DIR}/hopinico.o )
    SET(SOURCES ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/hopinico.o)

  ENDIF( MINGW )

  QT5_WRAP_UI(UI_HEADERS mainwindow.ui)

  QT5_ADD_RESOURCES (RESOURCES ../resources/res.qrc)

  ADD_EXECUTABLE(hopin WIN32 ${QSLOG_SRCS} ${CPUMINER} ${SOURCES} ${UI_HEADERS} ${RESOURCES})

  QT5_USE_MODULES(hopin Widgets Network)

  ADD_CUSTOM_COMMAND(TARGET hopin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/../3rdparty/pooler-cpuminer-2.3.2-win64"
    $<TARGET_FILE_DIR:hopin>/bin)

  ADD_CUSTOM_COMMAND(TARGET hopin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/../3rdparty/qt5/win32"
    $<TARGET_FILE_DIR:hopin>)

ENDIF(WIN32)
