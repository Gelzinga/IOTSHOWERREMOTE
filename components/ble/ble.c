#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

// NimBLE core
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"

// GAP service (device name, identity)
#include "services/gap/ble_svc_gap.h"
#include "host/ble_gap.h"


#include "ble.h"


static const char *TAG = "BLE_MODULE";

/*
 * Internal state
 * Keeps track of whether we are advertising or not
 */
static bool is_advertising = false;

/*
 * Forward declarations
 */
 
static void ble_app_on_sync(void);
static void host_task(void *param);
static void start_advertising(void);
static int gap_event_handler(struct ble_gap_event *event, void *arg);
// forward declarations are useful for telling a compiler about functions
//This way if you call these functions before they are defined ( because c is linear)
//There isn't an error because these are technically defined here. 


/*
 * ============================================================
 * BLE INITIALIZATION
 * ============================================================
 */

/*
 * ble_init()
 * ------------------------------------------------------------
 * This function:
 * 1. Initializes NVS (required for BLE bonding/storage)
 * 2. Initializes NimBLE host stack
 * 3. Registers sync callback (called when BLE is ready)
 * 4. Starts FreeRTOS task that runs BLE stack
 */
void ble_init(void)
{
    ESP_LOGI(TAG, "Initializing BLE module");

    // Required for BLE storage (bonding keys, etc.)( This initializes the Non valitale storage, which is used for keys and device states. )
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize NimBLE host stack
    nimble_port_init();

    // Called when BLE stack is fully ready
    ble_hs_cfg.sync_cb = ble_app_on_sync;

    // Start BLE task (runs forever in background)
    nimble_port_freertos_init(host_task);

    ESP_LOGI(TAG, "BLE init complete");
}

/*
 * ============================================================
 * BLE SYNC CALLBACK
 * ============================================================
 *
 * This runs automatically when:
 * - BLE stack is fully initialized
 * - controller + host are ready
 *
 * This is where we:
 * - set device name
 * - initialize GAP service
 * - optionally start advertising
 */
static void ble_app_on_sync(void)
{
    ESP_LOGI(TAG, "BLE stack synced (ready)");

    /*
     * Initialize GAP service
     * This enables:
     * - device name handling
     * - basic BLE identity services
     */
    ble_svc_gap_init();

    /*
     * Set the Bluetooth device name
     * This is what phones will see during scanning
     */
    ble_svc_gap_device_name_set("PENNY_IS_SEXY");

    /*
     * Automatically start advertising once BLE is ready
     */
    start_advertising();
}

/*
 * ============================================================
 * ADVERTISING
 * ============================================================
 *
 * Advertising = making device discoverable
 * Without this, phone will NEVER see the device
 */
static void start_advertising(void)
{
    int rc;

    /*
     * Advertising parameters (simple defaults)
     * You can tune:
     * - interval (power vs responsiveness)
     * - connectability
     */
    struct ble_gap_adv_params adv_params = {0};

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // general discoverable

    /*
     * Advertising data (what phones see during scan)
     */
    struct ble_hs_adv_fields fields; //make a struct named fields of type ble_hs_adv_fields
    memset(&fields, 0, sizeof(fields));  //clear the struct 

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP; //flags ( discoverability, etc.)

    const char *name = "PENNY_IS_SEXY"; // Name that is seen by devices- creates a literal string in memory
    fields.name = (uint8_t *)name; // Type cast - this makes it so that it is in an acceptable format for the api. 
    fields.name_len = strlen(name); // Length of the name 
    fields.name_is_complete = 1; // dummy variable that is needed for the API. 

    ble_gap_adv_set_fields(&fields); // pass the struct fields into the set function. 

    /*
     * Start advertising
     */
    rc = ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,
        NULL,
        BLE_HS_FOREVER,
        &adv_params,
        gap_event_handler, //register callback
        NULL
    );
       

    if (rc == 0) { // rc will be 0 if it is sucessful. 
        is_advertising = true;
        ESP_LOGI(TAG, "Advertising started");
    } else {
        ESP_LOGE(TAG, "Advertising failed: rc=%d", rc);
    }
}

/*
 * Public API: restart advertising if needed
 */
void ble_start_advertising(void)
{
    if (!is_advertising) {
        start_advertising();
    }
}

/*
 * ============================================================
 * BLE HOST TASK
 * ============================================================
 *
 * This runs the BLE stack loop forever.
 * Think of it as:
 * "Bluetooth operating system thread"
 */
static void host_task(void *param)
{
    ESP_LOGI(TAG, "BLE host task started");

    nimble_port_run();

    /*
     * Normally never reached
     * Only runs if BLE stack stops
     */
    nimble_port_freertos_deinit();
}

//this function is for internal refrencing, telling you whether or not a phone has connected. 
static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    /* Local variables */
    int rc = 0;
    struct ble_gap_conn_desc desc;

    /* Handle different GAP event */
    switch (event->type) {

    /* Connect event */
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        ESP_LOGI(TAG, "connection %s; status=%d",
                event->connect.status == 0 ? "established" : "failed",
                event->connect.status);

        /* Connection succeeded */
        if (event->connect.status == 0) {
            /* Check connection handle */
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc != 0) {
                ESP_LOGE(TAG,
                        "failed to find connection by handle, error code: %d",
                        rc);
                return rc;
            }

            /* Print connection descriptor and turn on the LED */
            // print_conn_desc(&desc);
            // led_on();

            /* Try to update connection parameters */
            struct ble_gap_upd_params params = {.itvl_min = desc.conn_itvl,
                                                .itvl_max = desc.conn_itvl,
                                                .latency = 3,
                                                .supervision_timeout =
                                                    desc.supervision_timeout};
            rc = ble_gap_update_params(event->connect.conn_handle, &params);
            if (rc != 0) {
                ESP_LOGE(
                    TAG,
                    "failed to update connection parameters, error code: %d",
                    rc);
                return rc;
            }
        }
        /* Connection failed, restart advertising */
        else {
            start_advertising();
        }
        return rc;

    /* Disconnect event */
    case BLE_GAP_EVENT_DISCONNECT:
        /* A connection was terminated, print connection descriptor */
        ESP_LOGI(TAG, "disconnected from peer; reason=%d",
                event->disconnect.reason);

        /* Turn off the LED */
        // led_off();

        /* Restart advertising */
        start_advertising();
        return rc;



    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        ESP_LOGI(TAG, "connection updated; status=%d",
                event->conn_update.status);

        /* Print connection descriptor */
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGE(TAG, "failed to find connection by handle, error code: %d",
                    rc);
            return rc;
        }
        // print_conn_desc(&desc);
        return rc;
    }
    
    return rc;
}
