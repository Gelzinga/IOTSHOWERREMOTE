idf_component_register(SRCS "ble.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash bt esp_event) # uses bt for the nimble includes 

