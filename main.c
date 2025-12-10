#include "FreeRTOS.h"
#include "task.h"
#include "scheduler.h"
#include "timeline_config.h"
#include "uart.h"

int main(void)
{
    // Inizializzazione hardware base (se serve)
    // hwInit();
	UART_init();
	UART_printf("Main: init UART\n");

    // Configuro lo scheduler con la tabella statica
    // Eventualmente spostare nella funzione vTaskStartScheduler()
    vConfigureScheduler(&g_timelineConfig);

	UART_printf("Main: starting FreeRTOS\n");

    // (eventualmente creare altri task di sistema)

    // Avvio lo scheduler FreeRTOS
    vTaskStartScheduler();

	UART_printf("ERROR\n");

    // Non dovremmo mai arrivare qui
    for (;;);
}