# Out-of-tree build of the plugin against an installed KTorrent (the Flatpak one).
# KTorrent does not install its plugin headers, so they come from a source checkout
# of the same release, and the two libraries are linked by path.
cmake_minimum_required(VERSION 3.16)
project(KTorrentRemoveScrubPlugin VERSION 1.0)

set(KTORRENT_SRC "" CACHE PATH "ktorrent source checkout (same release as the installed app)")
set(LIBKTORRENT_SRC "" CACHE PATH "libktorrent source checkout (same release as the installed app)")
set(KTORRENT_LIBDIR "/app/lib" CACHE PATH "directory holding libktcore and libKTorrent6")
set(KT_VERSION_MAJOR 26 CACHE STRING "")
set(KT_VERSION_MINOR 4 CACHE STRING "")
set(KT_VERSION_MICRO 3 CACHE STRING "")

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
kde_enable_exceptions()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(KF6 REQUIRED COMPONENTS Config CoreAddons I18n Parts XmlGui)

add_definitions(-DTRANSLATION_DOMAIN="ktorrent"
                -DVERSION_MAJOR=${KT_VERSION_MAJOR}
                -DVERSION_MINOR=${KT_VERSION_MINOR}
                -DVERSION_MICRO=${KT_VERSION_MICRO})

kcoreaddons_add_plugin(RemoveScrubPlugin INSTALL_NAMESPACE "ktorrent_plugins")
target_sources(RemoveScrubPlugin PRIVATE removescrubplugin.cpp)

target_include_directories(RemoveScrubPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs
    ${KTORRENT_SRC}/libktcore
    ${LIBKTORRENT_SRC}/src)

find_library(KTCORE_LIB NAMES ktcore libktcore.so.16 PATHS ${KTORRENT_LIBDIR} NO_DEFAULT_PATH REQUIRED)
find_library(KTORRENT6_LIB NAMES KTorrent6 PATHS ${KTORRENT_LIBDIR} NO_DEFAULT_PATH REQUIRED)

target_link_libraries(RemoveScrubPlugin
    ${KTCORE_LIB}
    ${KTORRENT6_LIB}
    Qt6::Widgets
    KF6::ConfigCore
    KF6::CoreAddons
    KF6::I18n
    KF6::Parts
    KF6::XmlGui)
