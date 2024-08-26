add_library(mod_player INTERFACE)

target_sources(mod_player INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/mod_play.c
  ${CMAKE_CURRENT_LIST_DIR}/sound_i2s.c
)

target_include_directories(mod_player INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(mod_player
  INTERFACE
  pico_stdlib
  hardware_dma
  hardware_pio
  hardware_irq
)