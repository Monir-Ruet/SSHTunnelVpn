cmake_minimum_required(VERSION 3.19)
project(SSHTunneller LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Network)
find_package(libssh REQUIRED)

qt_standard_project_setup()

set(UI
    ui/mainwindow.ui
)

qt_add_executable(SSHTunneller
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${UI}
    include/mainwindow.h src/core/mainwindow.cpp
    include/socksproxy.h src/server/socksproxy.cpp
    include/ssh.h src/libssh/ssh.cpp
    include/sshworker.h src/libssh/sshworker.cpp
    include/credential.h src/utils/credential.cpp
    include/settings.h src/utils/settings.cpp
    include/server.h src/server/server.cpp
    include/socket.h src/server/socket.cpp
    include/socketthread.h src/server/socketthread.cpp
    src/libssh/sshsessionpool.h src/libssh/sshsessionpool.cpp
)

qt_add_translations(
    TARGETS SSHTunneller
    TS_FILES SSHTunneller_en_US.ts
)

target_link_libraries(SSHTunneller
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Network
        ssh
)

target_include_directories(SSHTunneller PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(SSHTunneller PROPERTIES AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

include(GNUInstallDirs)

install(TARGETS SSHTunneller
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SSHTunneller
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
