
#ifndef ble_h //avoid defining twice 
#define ble_h

#include <stdio.h>
#include <string.h>
 // todo: delete the uncessary files here. 
#include "esp_log.h"
#include "nvs_flash.h"

// NimBLE core
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"

// GAP service (device name, identity)
#include "services/gap/ble_svc_gap.h"
void ble_init(void);

void ble_start_advertising(void); //restart advertising ( should start automatically with a call to ble_init.)

#endif