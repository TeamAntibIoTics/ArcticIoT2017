#include "mcp_wrapper.h"
#include <stdio.h>
#include "mcp/protocol.h"

bool mcpw_start_send(mcp_wrapper_t *mcpw);
void mcpw_on_mcp_messagebuild(unsigned char byte, void * user_data);
void mcpw_mcp_handle_protocol_packet(void * userData, const unsigned char * data, unsigned int length);
void mcpw_mcp_handle_protocol_error(void * userData, unsigned int error);

// MCP host parser callbacks implemented here
void mcpw_on_host_parser_ack(Ack ack, void* user_data);
void mcpw_on_host_parser_error(uint32_t error, void * user_data);
//void mcpw_on_host_parser_reply(Reply reply, void * user_data);

// MCP host parser callbacks that should be implemented in user code
//void mcpw_on_host_parser_pong(uint32_t pong, void * user_data);
//void mcpw_on_host_parser_datafloat(FloatData fd, void * user_data);
//void mcpw_on_host_parser_respiration(RespirationData rd, void * user_data);
//void mcpw_on_host_parser_sleep(SleepData rd, void * user_data);
//void mcpw_on_host_parser_baseband_ap(BasebandApData bap, void * user_data);
//void mcpw_on_host_parser_baseband_iq(BasebandIqData bap, void * user_data);
//void mcpw_on_host_parser_true_presence_single(TruePresenceSingleData tps, void * user_data);
//void mcpw_on_host_parser_true_presence_movinglist(TruePresenceMovingListData tpml, void * user_data);

void mcpw_on_host_parser_ack(Ack ack, void* user_data)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*)user_data;
	mcpw->sync_response[0] = XTS_SPR_ACK;
	mcpw->sync_response_length = 1;
	mcpw->response_ready();
}

void mcpw_on_host_parser_error(uint32_t error, void * user_data)
{
	if (error == 2) return; // Unknown error in parse, too trigger happy to include here.
	
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*)user_data;
	mcpw->sync_response[0] = XTS_SPR_ERROR;
	mcpw->sync_response_length = 1;	
	mcpw->response_ready();
}

void mcpw_on_mcp_messagebuild(unsigned char byte, void * user_data)
{
	// Callback used by protocol to create message.
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*)user_data;
	if (mcpw->tx_buf_index >= MCPW_TX_BUFFER_LENGTH) return; // Unhandled error. TX buffer too small.
	mcpw->tx_buf[mcpw->tx_buf_index++] = byte;
}

uint32_t mcpw_get_instance_size(void) 
{ 
	return sizeof(mcp_wrapper_t) + getInstanceSize() + sizeof(HostParser); 
}

bool mcpw_start_send(mcp_wrapper_t *mcpw)
{
	if (mcpw->busy) return false;
	mcpw->busy = true;
	mcpw->tx_buf_index = 0;
	return true;
}

mcp_wrapper_t * mcpw_init(void *instance_memory)
{
	mcp_wrapper_t * mcpw = (mcp_wrapper_t*)instance_memory;
	void* protocol_instance_memory = (int)(void*)mcpw + sizeof(mcp_wrapper_t); // Place protocol instance right after mcp_wrapper struct in already alocated memory provided.
	void* mcp_host_parser_instance_memory = (int)(void*)protocol_instance_memory + getInstanceSize();
	mcpw->mcp = createApplicationProtocol(
		&mcpw_mcp_handle_protocol_packet,
		&mcpw_mcp_handle_protocol_error,
		(void*)mcpw,
		protocol_instance_memory, 
		mcpw->rx_buf,
		sizeof(mcpw->rx_buf));
	
	mcpw->on_mcp_messagebuild = mcpw_on_mcp_messagebuild;
	mcpw->send_bytes = NULL; // Must be implemented by user.
	mcpw->wait_for_response = NULL;
	mcpw->response_ready = NULL;
	
	mcpw->sync_response_length = 0;
	mcpw->default_timeout = 2000;

	mcpw->tx_buf_index = 0;
	mcpw->busy = false;
	mcpw->mcp_host_parser = (HostParser*)mcp_host_parser_instance_memory;
	
	init_host_parser(mcpw->mcp_host_parser);
	mcpw->mcp_host_parser->ack = mcpw_on_host_parser_ack;
	mcpw->mcp_host_parser->error = mcpw_on_host_parser_error;
	
	
	return mcpw;
}

void mcpw_process_rx_data(mcp_wrapper_t *mcpw, uint8_t *data, uint32_t length)
{
	if (!mcpw) return;
	parseData(mcpw->mcp, data, length);
}

void mcpw_mcp_handle_protocol_packet(void * userData, const unsigned char * data, unsigned int length)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*) userData;
	
    parse(mcpw->mcp_host_parser, data, length, (void*) mcpw);
}


void mcpw_mcp_handle_protocol_error(void * userData, unsigned int error)
{
	mcp_wrapper_t *mcpw = (mcp_wrapper_t*) userData;

}


int mcpw_ping(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createPingCommand(mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	mcpw->busy = false;	
	return MCPW_OK;
}

int mcpw_module_reset(mcp_wrapper_t *mcpw)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createModuleResetCommand(mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
		ret = MCPW_OK;
	mcpw->busy = false;

    return ret;
}

int mcpw_load_profile(mcp_wrapper_t *mcpw, uint32_t profileid)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createLoadProfileCommand(profileid, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_sensor_mode(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t param)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetSensorModeCommand(mode, param, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_sensitivity(mcp_wrapper_t *mcpw, uint32_t sensitivity)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetSensitivityCommand(sensitivity, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_detection_zone(mcp_wrapper_t *mcpw, float start, float end)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetDetectionZoneCommand(start, end, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

int mcpw_set_led_control(mcp_wrapper_t *mcpw, uint8_t mode, uint8_t intensity)
{
	if (!mcpw_start_send(mcpw)) return MCPW_ERROR;
	if (!mcpw->send_bytes) return MCPW_ERROR;
	createSetLedControlCommand(mode, intensity, mcpw->on_mcp_messagebuild, (void*)mcpw);
    mcpw->send_bytes(mcpw);
	if (!mcpw->wait_for_response(mcpw->default_timeout))
	{
		mcpw->busy = false;
		return MCPW_ERROR_TIMEOUT;
	}
	int ret = MCPW_ERROR;
	if ((mcpw->sync_response_length == 1) && (mcpw->sync_response[0] == XTS_SPR_ACK))
	ret = MCPW_OK;
	mcpw->busy = false;
    return ret;
}

