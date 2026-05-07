#include <stdio.h>

#include "esp_log.h"
#include "ble.h"
#include "power.h"
#include "buttons.h"
// NimBLE includes
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

void app_main(void)
{

    ble_init(); 
}