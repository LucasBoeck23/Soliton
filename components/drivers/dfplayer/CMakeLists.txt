idf_component_register(
    SRCS "src/drv_dfplayer.c"
    INCLUDE_DIRS "include"
    REQUIRES hal
    PRIV_REQUIRES ""
)

# Layer enforcement: drivers só podem depender de hal e ESP-IDF
set(COMPONENT_LAYER "drivers")
