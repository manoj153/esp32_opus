deps_config := \
	/home/radev1/esp/esp-idf/components/app_trace/Kconfig \
	/home/radev1/esp/esp-idf/components/aws_iot/Kconfig \
	/home/radev1/esp/esp-idf/components/bt/Kconfig \
	/home/radev1/esp/esp-idf/components/driver/Kconfig \
	/home/radev1/esp/esp-idf/components/efuse/Kconfig \
	/home/radev1/esp/esp-idf/components/esp32/Kconfig \
	/home/radev1/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/radev1/esp/esp-idf/components/esp_event/Kconfig \
	/home/radev1/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/radev1/esp/esp-idf/components/esp_http_server/Kconfig \
	/home/radev1/esp/esp-idf/components/esp_https_ota/Kconfig \
	/home/radev1/esp/esp-idf/components/espcoredump/Kconfig \
	/home/radev1/esp/esp-idf/components/ethernet/Kconfig \
	/home/radev1/esp/esp-idf/components/fatfs/Kconfig \
	/home/radev1/esp/esp-idf/components/freemodbus/Kconfig \
	/home/radev1/esp/esp-idf/components/freertos/Kconfig \
	/home/radev1/esp/esp-idf/components/heap/Kconfig \
	/home/radev1/esp/esp-idf/components/libsodium/Kconfig \
	/home/radev1/esp/esp-idf/components/log/Kconfig \
	/home/radev1/esp/esp-idf/components/lwip/Kconfig \
	/home/radev1/esp/esp-idf/components/mbedtls/Kconfig \
	/home/radev1/esp/esp-idf/components/mdns/Kconfig \
	/home/radev1/esp/esp-idf/components/mqtt/Kconfig \
	/home/radev1/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/radev1/esp/esp-idf/components/openssl/Kconfig \
	/home/radev1/esp/esp-idf/components/pthread/Kconfig \
	/home/radev1/esp/esp-idf/components/spi_flash/Kconfig \
	/home/radev1/esp/esp-idf/components/spiffs/Kconfig \
	/home/radev1/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/radev1/esp/esp-idf/components/unity/Kconfig \
	/home/radev1/esp/esp-idf/components/vfs/Kconfig \
	/home/radev1/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/radev1/esp/esp-idf/components/app_update/Kconfig.projbuild \
	/home/radev1/esp/esp-adf/components/audio_hal/Kconfig.projbuild \
	/home/radev1/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/radev1/esp/esp-adf/components/esp-adf-libs/Kconfig.projbuild \
	/home/radev1/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/radev1/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/radev1/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
