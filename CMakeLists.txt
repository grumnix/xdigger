cmake_minimum_required(VERSION 3.0)
project(xdigger)

include(GNUInstallDirs)

find_package(X11 REQUIRED)

file(GLOB XDIGGER_SOURCES *.c)
file(GLOB XDIGGER_AUDIO_FILES audio/*.au)

add_executable(xdigger
  ${XDIGGER_SOURCES}
  )
target_link_libraries(xdigger PRIVATE
  X11::X11
  X11::Xext
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
  )

install(TARGETS xdigger
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

install(FILES ${XDIGGER_AUDIO_FILES}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/games/xdigger/audio/
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
