# SPDX-License-Identifier: Apache-2.0

if(CONFIG_WIFI_WINC1500)
  zephyr_library_include_directories(./)

  zephyr_library_sources(
    wifi_winc1500_nm_bsp.c
    wifi_winc1500_nm_bus_wrapper.c
    wifi_winc1500.c
    )
endif()
