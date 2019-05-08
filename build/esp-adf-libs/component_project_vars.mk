# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += /home/radev1/esp/esp-adf/components/esp-adf-libs/esp_audio/include /home/radev1/esp/esp-adf/components/esp-adf-libs/esp_codec/include/codec /home/radev1/esp/esp-adf/components/esp-adf-libs/esp_codec/include/processing /home/radev1/esp/esp-adf/components/esp-adf-libs/recorder_engine/include /home/radev1/esp/esp-adf/components/esp-adf-libs/esp_sr/include /home/radev1/esp/esp-adf/components/esp-adf-libs/esp_ssdp/include /home/radev1/esp/esp-adf/components/esp-adf-libs/esp_dlna/include /home/radev1/esp/esp-adf/components/esp-adf-libs/esp_upnp/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/esp-adf-libs -lesp-adf-libs -L/home/radev1/esp/esp-adf/components/esp-adf-libs/esp_audio/lib -L/home/radev1/esp/esp-adf/components/esp-adf-libs/esp_codec/lib -L/home/radev1/esp/esp-adf/components/esp-adf-libs/recorder_engine/lib -L/home/radev1/esp/esp-adf/components/esp-adf-libs/esp_sr/lib -L/home/radev1/esp/esp-adf/components/esp-adf-libs/esp_ssdp/lib -L/home/radev1/esp/esp-adf/components/esp-adf-libs/esp_upnp/lib -L/home/radev1/esp/esp-adf/components/esp-adf-libs/esp_dlna/lib -lesp_codec -lesp_audio -lesp-amr -lesp-amrwbenc -lesp-aac -lesp-ogg-container -lesp-opus -lesp-tremor -lesp-flac -lrecorder_engine -lesp_ssdp -lesp_upnp -lesp_dlna -lvad -lesp_wakenet -lnn_model_hilexin_wn3 
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += esp-adf-libs
COMPONENT_LDFRAGMENTS += 
component-esp-adf-libs-build: 
