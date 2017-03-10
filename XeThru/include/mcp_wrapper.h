#ifndef MCP_WRAPPER_H
#define MCP_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

#include "mcp/protocol.h"
#include "mcp/protocol_host_parser.h"

#define MCPW_OK					0
#define MCPW_ERROR				1
#define MCPW_ERROR_TIMEOUT		2

 
#define MCPW_RX_BUFFER_LENGTH 100
#define MCPW_TX_BUFFER_LENGTH 100
#define MCPW_SYNC_RESPONSE_LENGTH 20

typedef bool (*mcpw_send_bytes_method)(void* mcpw_ref);
typedef bool (*mcpw_wait_for_response_method)(uint32_t timeout);
typedef void (*mcpw_response_ready_method)(void);

typedef struct
{
	uint32_t tx_buf_index;
	uint8_t rx_buf[MCPW_RX_BUFFER_LENGTH]; //  __attribute__((aligned(4)));
	uint8_t tx_buf[MCPW_TX_BUFFER_LENGTH]; //  __attribute__((aligned(4)));
	uint8_t sync_response[MCPW_SYNC_RESPONSE_LENGTH];
	uint32_t sync_response_length;

	xtProtocol * mcp;
	HostParser * mcp_host_parser;
	AppendCallback on_mcp_messagebuild;
	mcpw_send_bytes_method send_bytes;
	mcpw_wait_for_response_method wait_for_response;
	mcpw_response_ready_method response_ready;

	uint32_t default_timeout;

	bool busy;

    void* user_reference;
} mcp_wrapper_t;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t mcpw_get_instance_size(void);
mcp_wrapper_t * mcpw_init(void *instance_memory);
void mcpw_process_rx_data(mcp_wrapper_t *mcpw, uint8_t *data, uint32_t length);

int mcpw_ping(mcp_wrapper_t *mcpw);
int mcpw_module_reset(mcp_wrapper_t *mcpw);
int mcpw_load_profile(mcp_wrapper_t *mcpw, uint32_t profileid);
int mcpw_set_sensor_mode(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t param);
int mcpw_set_sensitivity(mcp_wrapper_t *mcpw, uint32_t sensitivity);
int mcpw_set_detection_zone(mcp_wrapper_t *mcpw, float start, float end);
int mcpw_set_led_control(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t intensity);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MCP_WRAPPER_H */
