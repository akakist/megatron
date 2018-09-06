cmake_minimum_required (VERSION 2.6)

add_library(core-geoip SHARED
    geoip.cpp
    geoip_reg.cpp
    geoip_impl.cpp
    CapsGeoIP.cpp
    ../../corelib/sqlite3Wrapper.cpp
)

target_link_libraries(core-geoip megatron-core crypto pthread sqlite3)

install(TARGETS core-geoip DESTINATION ${PLUGIN_TARGET_DIR})

