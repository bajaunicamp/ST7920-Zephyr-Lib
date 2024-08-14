// Esse programa de expemplo printa "Hello World!" no LOG no momento em que o led pisca

#include "zephyr/device.h"
#include "zephyr/sys/printk.h"
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER();

const struct gpio_dt_spec *const led = &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);

int main(){
  LOG_INF("Inicializando");

  if(!device_is_ready(led->port)){
    LOG_ERR("Led não está funcinando");
  }

  gpio_pin_configure_dt(led, GPIO_OUTPUT_ACTIVE);
  
  // Note que o log do programa em execução acontece os dois de uma
  // vez, isso ocorre porque ele é assícrono
  // Por isso tome cuidado com essas funções
  while (true) {
    printk("Led aceso\n");
    gpio_pin_set_dt(led, 1);

    // Além do LOG, temos o printk

    k_msleep(1000);

    printk("Led apagado\n");
    gpio_pin_set_dt(led, 0);
    k_msleep(1000);
  }
}
