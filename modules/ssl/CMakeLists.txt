#cmake_minimum_required (VERSION 2.6)

add_library(core-ssl SHARED
    ssl.cpp
    ssl_reg.cpp
    st_rsa_impl.cpp
rsa-pub_512_1387663237.h
st_rsa_impl.h
)

target_link_libraries(core-ssl megatron-core crypto pthread dl)

install(TARGETS core-ssl DESTINATION ${PLUGIN_TARGET_DIR})

