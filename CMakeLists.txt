cmake_minimum_required(VERSION 3.0)
project(xdigger)

include(GNUInstallDirs)

find_package(X11 REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(SDL2  REQUIRED sdl2 IMPORTED_TARGET)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer IMPORTED_TARGET)

file(GLOB XDIGGER_SOURCES *.c)
file(GLOB XDIGGER_AUDIO_FILES audio/*.au audio/*.wav)

add_executable(xdigger
  ${XDIGGER_SOURCES}
  )
target_link_libraries(xdigger PRIVATE
  X11::X11
  X11::Xext
  PkgConfig::SDL2
  PkgConfig::SDL2_MIXER
  )
target_compile_options(xdigger PRIVATE
  # -Wno-unused-result
  # -Wno-deprecated-declarations
  # -Wno-format-truncation
  )
target_compile_definitions(xdigger PRIVATE
  -D_DEFAULT_SOURCE
  -D_POSIX_C_SOURCE=199309L
  -D_POSIX_SOURCE
  -D_XOPEN_SOURCE
  -DSOUND_SDL
  )

install(TARGETS xdigger
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

install(FILES ${XDIGGER_AUDIO_FILES}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/games/xdigger/
  )

install(FILES xdigger.level
  DESTINATION ${CMAKE_INSTALL_DATADIR}/games/xdigger/
  )

install(FILES pixmaps/xdigger.xpm
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps/
  )

install(FILES xdigger.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications/
  )

install(FILES xdigger.man
  DESTINATION ${CMAKE_INSTALL_DATADIR}/man/man6/
  RENAME xdigger.6
  )

# EOF #
