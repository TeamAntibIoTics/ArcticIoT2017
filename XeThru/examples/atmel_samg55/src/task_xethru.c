/*
 * task_xethru.c
 *
 * Created: 09.05.2016 13.14.06
 *
 */ 

#include "task_xethru.h"

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Atmel library includes. */
#include "freertos_usart_serial.h"

/* XeThru protocol implementation. */
#include "mcp_wrapper.h"
#include "xtid.h"

#include "arm_math.h"


#define TASK_XETHRU_STACK_SIZE						(1000)
#define TASK_XETHRU_PRIORITY					(tskIDLE_PRIORITY+0)
#define TASK_XETHRU_RX_STACK_SIZE						(2000)
#define TASK_XETHRU_RX_PRIORITY					(tskIDLE_PRIORITY+0)
#define TASK_XETHRU_TX_STACK_SIZE						(500)
#define TASK_XETHRU_TX_PRIORITY					(tskIDLE_PRIORITY+0)


/* The size of the buffer used to receive characters from the USART driver.
 * This equals the length of the longest string used in this file. */
#define XETHRU_RX_BUFFER_SIZE          (100)
#define XETHRU_TX_BUFFER_SIZE          (100)

/* The baud rate to use at startup. */
#define USART_BAUD_RATE         (115200)

/*
 * Tasks used to develop the USART drivers. 
 */
static void xethru_usart_tx_task(void *pvParameters);
static void xethru_usart_rx_task(void *pvParameters);
static void xethru_task(void *pvParameters);


/* The buffer provided to the USART driver to store incoming character in. */
static uint8_t receive_buffer[XETHRU_RX_BUFFER_SIZE+1]  __attribute__ ((aligned (4)));

mcp_wrapper_t* mcpw = NULL;
xSemaphoreHandle mcpw_sync_responce_semaphore;

static xQueueHandle xethru_usart_tx_queue;


bool mcpw_wait_for_response(uint32_t timeout);
void mcpw_response_ready(void);
bool mcpw_send_bytes(void* mcpw_void);
void mcpw_on_host_parser_presence_single(PresenceSingleData presence_single_data, void * user_data);


bool mcpw_wait_for_response(uint32_t timeout)
{
	if (xSemaphoreTake(mcpw_sync_responce_semaphore, timeout / portTICK_RATE_MS) == pdTRUE)
		return true;
	else
		return false;
}

void mcpw_response_ready(void)
{
	xSemaphoreGive(mcpw_sync_responce_semaphore);
}

void mcpw_on_host_parser_respiration(RespirationData respiration_data, void * user_data)
{
    printf("Respiration: Counter=%u", respiration_data.frame_counter);
    printf(", State=%u", respiration_data.sensor_state);
    printf(", RPM=%u", respiration_data.respiration_rate);
    printf(", Distance=%u.%02.2u", (int)respiration_data.distance, (int)((respiration_data.distance-(int)respiration_data.distance)*100.0f));
    printf(", Movement (breathing pattern)=%d.%02.2u", (int)respiration_data.movement, (int)((fabs(respiration_data.movement)-(int)fabs(respiration_data.movement))*100.0f));
    printf(", SigQ=%u", respiration_data.signal_quality);
    printf("\n");
}

void mcpw_on_host_parser_sleep(SleepData sleep_data, void * user_data)
{
    printf("Sleep: Counter=%u", sleep_data.frame_counter);
    printf(", State=%u", sleep_data.sensor_state);
    printf(", RPM=%u.%02.2u", (int)sleep_data.respiration_rate, (int)((sleep_data.respiration_rate-(int)sleep_data.respiration_rate)*100.0f));
    printf(", Distance=%u.%02.2u", (int)sleep_data.distance, (int)((sleep_data.distance-(int)sleep_data.distance)*100.0f));
    printf(", SigQ=%u", sleep_data.signal_quality);
    printf(", MovementSlow=%u.%02.2u", (int)sleep_data.movement_slow, (int)((sleep_data.movement_slow-(int)sleep_data.movement_slow)*100.0f));
    printf(", MovementFast=%u.%02.2u", (int)sleep_data.movement_fast, (int)((sleep_data.movement_fast-(int)sleep_data.movement_fast)*100.0f));
    printf("\n");
}

void mcpw_on_host_parser_presence_single(PresenceSingleData presence_single_data, void * user_data)
{
	printf("Counter=%u", presence_single_data.frame_counter);
	printf(", State=");
	if (presence_single_data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_PRESENCE)
		printf("Presence");
	else if (presence_single_data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_NO_PRESENCE)
		printf("No presence");
	else
		printf("Initializing");
	if (presence_single_data.distance>0)
		printf(", Distance=%u.%02.2u", (int)presence_single_data.distance, (int)((presence_single_data.distance-(int)presence_single_data.distance)*100.0f));
	printf("\n");
}

void task_xethru_init(Usart *usart_base)
{
	freertos_usart_if freertos_usart;
	freertos_peripheral_options_t driver_options = {
		receive_buffer,								/* The buffer used internally by the USART driver to store incoming characters. */
		XETHRU_RX_BUFFER_SIZE,									/* The size of the buffer provided to the USART driver to store incoming characters. */
		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,	/* The priority used by the USART interrupts. */
		USART_RS232,									/* Configure the USART for RS232 operation. */
		(USE_TX_ACCESS_MUTEX | USE_RX_ACCESS_MUTEX)
	};

	const sam_usart_opt_t usart_settings = {
		USART_BAUD_RATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_NO,
		US_MR_NBSTOP_1_BIT,
		US_MR_CHMODE_NORMAL,
		0 /* Only used in IrDA mode. */
	}; 

	/* Initialise the USART interface. */
	freertos_usart = freertos_usart_serial_init(usart_base,
			&usart_settings,
			&driver_options);
	configASSERT(freertos_usart);
	
	xethru_usart_tx_queue = xQueueCreate(100, sizeof(uint8_t));

	/* Create the tasks. */
	xTaskCreate(xethru_usart_tx_task, (const signed char *const) "XtTx",
			TASK_XETHRU_TX_STACK_SIZE, (void *) freertos_usart,
			TASK_XETHRU_TX_PRIORITY, NULL);
	xTaskCreate(xethru_usart_rx_task, (const signed char *const) "XtRx",
			TASK_XETHRU_RX_STACK_SIZE, (void *) freertos_usart,
			TASK_XETHRU_RX_PRIORITY + 1, NULL);
	xTaskCreate(xethru_task, (const signed char *const) "XtCtr",
			TASK_XETHRU_STACK_SIZE, (void *) NULL,
			TASK_XETHRU_PRIORITY + 1, NULL);
}

static void xethru_usart_tx_task(void *pvParameters)
{
	freertos_usart_if usart_port;
	static uint8_t local_buffer[XETHRU_TX_BUFFER_SIZE+1];
	const portTickType time_out_definition = (100UL / portTICK_RATE_MS),
			short_delay = (10UL / portTICK_RATE_MS);
	xSemaphoreHandle notification_semaphore;
	unsigned portBASE_TYPE string_index;
	status_code_t returned_status;

	/* The (already open) USART port is passed in as the task parameter. */
	usart_port = (freertos_usart_if)pvParameters;

	/* Create the semaphore to be used to get notified of end of
	transmissions. */
	vSemaphoreCreateBinary(notification_semaphore);
	configASSERT(notification_semaphore);

	/* Start with the semaphore in the expected state - no data has been sent
	yet.  A block time of zero is used as the semaphore is guaranteed to be
	there as it has only just been created. */
	xSemaphoreTake(notification_semaphore, 0);

	uint8_t data;
	for (;;) 
	{
		/* Wait for data. */
		int i=0;
		while ( xQueueReceive( xethru_usart_tx_queue, &data, 10UL / portTICK_RATE_MS) == pdTRUE)
        {
			local_buffer[i++] = data;
			if (i>=XETHRU_TX_BUFFER_SIZE) break;
        }		
		
		if (i>0)
		{
			// Start send. 
			returned_status = freertos_usart_write_packet_async(usart_port,
					local_buffer, 
					i,
					time_out_definition, notification_semaphore);
			// configASSERT(returned_status == STATUS_OK);

			/* The async version of the write function is being used, so wait for
			the end of the transmission.  No CPU time is used while waiting for the
			semaphore.*/
			xSemaphoreTake(notification_semaphore, time_out_definition * 2);
			
		}

		vTaskDelay(100UL / portTICK_RATE_MS);
	}
}

static void xethru_usart_rx_task(void *pvParameters)
{
	freertos_usart_if usart_port;
	static uint8_t rx_buffer[XETHRU_RX_BUFFER_SIZE+1];
	uint32_t received = 1;
	unsigned portBASE_TYPE string_index;

	/* The (already open) USART port is passed in as the task parameter. */
	usart_port = (freertos_usart_if)pvParameters;

	string_index = 0;

	for (;;) 
	{
		received = freertos_usart_serial_read_packet(usart_port, rx_buffer, XETHRU_RX_BUFFER_SIZE, received == 0 ? 100UL / portTICK_RATE_MS : 10UL / portTICK_RATE_MS);
		if (received == 0) continue;

		mcpw_process_rx_data(mcpw, rx_buffer, received);
	}
}


bool mcpw_send_bytes(void *mcpw_void)
{
	mcp_wrapper_t* mcpw = (mcp_wrapper_t*)mcpw_void;
	for(int i=0;i<mcpw->tx_buf_index;i++)
	{
		if( xQueueSend( xethru_usart_tx_queue, mcpw->tx_buf+i, 10UL / portTICK_RATE_MS ) != pdPASS )
		{
			return false;
		}
	}
}


static void xethru_task(void *pvParameters)
{

    printf("Connecting to XeThru module.\n");
    printf("Starting serial port read thread.\n");
	

	vSemaphoreCreateBinary(mcpw_sync_responce_semaphore);
	xSemaphoreTake(mcpw_sync_responce_semaphore, 0);

	unsigned char mcpw_instance_memory[mcpw_get_instance_size()];
	mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
    mcpw->mcp_host_parser->sleep = mcpw_on_host_parser_sleep; // X2M200 sleep message
    mcpw->mcp_host_parser->respiration = mcpw_on_host_parser_respiration; // X2M200 respiration message
    mcpw->mcp_host_parser->presence_single = mcpw_on_host_parser_presence_single; // X4M300 presence single message

    int res = 0;
#if 0 // X4M300 Presence demo
    res = mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
    res |= mcpw_load_profile(mcpw, XTS_ID_APP_PRESENCE_2);
    res |= mcpw_set_detection_zone(mcpw, 0.6, 3.0);
#else // X2M200 Sleep demo
	mcpw_module_reset(mcpw);
	vTaskDelay(1000UL / portTICK_RATE_MS);
    res |= mcpw_load_profile(mcpw, XTS_ID_APP_SLEEP);
    res |= mcpw_set_detection_zone(mcpw, 0.4, 2.0);
#endif	
    res |= mcpw_set_sensitivity(mcpw, 9);
    res |= mcpw_set_led_control(mcpw, 2, 100);
    res |= mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0);

	for (;;)
	{
		vTaskDelay(1000UL / portTICK_RATE_MS);
	}

}

