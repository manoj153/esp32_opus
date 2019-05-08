/* Play WAV file from SD Card

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
//#include "fatfs_stream.h"
#include "spiffs_stream.h"
#include "i2s_stream.h"
// #include "wav_decoder.h"
#include "opus_decoder.h"
#include "esp_peripherals.h"
//#include "periph_sdcard.h"
#include "periph_spiffs.h"

#define PLAYBACK_RATE       48000
#define PLAYBACK_CHANNEL    2
#define PLAYBACK_BITS       16

static const char *TAG = "SDCARD_WAV_EXAMPLE";

static audio_element_handle_t create_spiffs_stream(int sample_rates, int bits, int channels, audio_stream_type_t type)
{
    spiffs_stream_cfg_t spiffs_cfg = SPIFFS_STREAM_CFG_DEFAULT();
    spiffs_cfg.type = type;
    audio_element_handle_t spiffs_stream = spiffs_stream_init(&spiffs_cfg);
    mem_assert(spiffs_stream);
    audio_element_info_t writer_info = {0};
    audio_element_getinfo(spiffs_stream, &writer_info);
    writer_info.bits = bits;
    writer_info.channels = channels;
    writer_info.sample_rates = sample_rates;
    audio_element_setinfo(spiffs_stream, &writer_info);
    return spiffs_stream;
}

void app_main(void)
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t  i2s_stream_writer, wav_decoder;

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);
    esp_log_level_set("PERIPH_SPIFFS", ESP_LOG_INFO);
    ESP_LOGI(TAG, "[ 1 ] Mount sdcard");
    // Initialize peripherals management
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

   periph_spiffs_cfg_t spiffs_cfg = {
        .root = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_periph_handle_t spiffs_handle = periph_spiffs_init(&spiffs_cfg);



    // Start spiffs & button peripheral
    esp_periph_start(set, spiffs_handle);
   

    // Wait until spiffs is mounted
    while (!periph_spiffs_is_mounted(spiffs_handle)) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }    

    

   
    ESP_LOGI(TAG, "[3.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[3.1] Create fatfs stream to read data from sdcard");
    audio_element_handle_t spiffs_reader_el = create_spiffs_stream(PLAYBACK_RATE, PLAYBACK_BITS, PLAYBACK_CHANNEL, AUDIO_STREAM_READER);
    // fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    // fatfs_cfg.type = AUDIO_STREAM_READER;
    // fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);

    ESP_LOGI(TAG, "[3.2] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[3.3] Create wav decoder to decode wav file");
    opus_decoder_cfg_t wav_cfg = DEFAULT_OPUS_DECODER_CONFIG();
    wav_decoder = decoder_opus_init(&wav_cfg);

    ESP_LOGI(TAG, "[3.4] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, spiffs_reader_el, "file");
    audio_pipeline_register(pipeline, wav_decoder, "wav");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[3.5] Link it together [sdcard]-->fatfs_stream-->wav_decoder-->i2s_stream-->[codec_chip]");
    audio_pipeline_link(pipeline, (const char *[]) {"file", "wav", "i2s"}, 3);

    ESP_LOGI(TAG, "[3.6] Set up  uri (file as fatfs_stream, wav as wav decoder, and default output is i2s)");
    audio_element_set_uri(spiffs_reader_el, "/spiffs/1.opus");


    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);


    ESP_LOGI(TAG, "[ 5 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);

    ESP_LOGI(TAG, "[ 6 ] Listen for all pipeline events");
    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) wav_decoder
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            audio_element_getinfo(wav_decoder, &music_info);

            ESP_LOGI(TAG, "[ * ] Receive music info from wav decoder, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            audio_element_setinfo(i2s_stream_writer, &music_info);
            i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
            continue;
        }
        /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (int) msg.data == AEL_STATUS_STATE_STOPPED) {
            ESP_LOGW(TAG, "[ * ] Stop event received");
            break;
        }
    }

    ESP_LOGI(TAG, "[ 7 ] Stop audio_pipeline");
    audio_pipeline_terminate(pipeline);

    // audio_pipeline_unregister(pipeline, fatfs_stream_reader);
    audio_pipeline_unregister(pipeline, spiffs_reader_el);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);
    audio_pipeline_unregister(pipeline, wav_decoder);
    

    /* Terminal the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Stop all periph before removing the listener */
    esp_periph_set_stop_all(set);

    audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    //audio_element_deinit(fatfs_stream_reader);
    audio_element_deinit(i2s_stream_writer);
    audio_element_deinit(wav_decoder);
    esp_periph_set_destroy(set);
}

void playback_task()
{



}