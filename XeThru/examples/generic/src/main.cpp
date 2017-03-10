#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ModuleIoFactory.hpp"
#include <stdio.h>
#include "mcp_wrapper.h"
#include "Objbase.h"

using namespace std;

char* directoryForSampleMessage;
std::mutex moduleIoMutex;
std::mutex mcpw_sync_response_mutex;
std::condition_variable mcpw_sync_response_condition;

//! [MCPW Platform Methods]
bool mcpw_send_bytes(void* mcpw_void); // Send bytes over serial port
bool mcpw_wait_for_response(uint32_t timeout); // Task synchronization, wait for response
void mcpw_response_ready(void); // Task synchronization, response ready
void mcpw_on_host_parser_sleep(SleepData sleep_data, void * user_data); // Data parsed: Sleep message
void mcpw_on_host_parser_respiration(RespirationData respiration_data, void * user_data); // Data parsed: Respiration message
void mcpw_on_host_parser_presence_single(PresenceSingleData presence_single_data, void * user_data); // Data parsed: PresenceSingle message

bool mcpw_send_bytes(void *mcpw_void)
{
	mcp_wrapper_t* mcpw = (mcp_wrapper_t*)mcpw_void;
	ModuleIo* moduleIo = (ModuleIo*)mcpw->user_reference;
	moduleIoMutex.lock();
	moduleIo->write(mcpw->tx_buf, mcpw->tx_buf_index);
	moduleIoMutex.unlock();
	return true;
}

bool mcpw_wait_for_response(uint32_t timeout)
{
	std::unique_lock<std::mutex> lock(mcpw_sync_response_mutex);
	bool res = (mcpw_sync_response_condition.wait_for(lock, std::chrono::milliseconds(timeout)) != std::cv_status::timeout);
	return res;
}

void mcpw_response_ready(void)
{
	mcpw_sync_response_condition.notify_one();
}

void mcpw_on_host_parser_respiration(RespirationData respiration_data, void * user_data)
{
	cout << "Respiration: Counter=" << respiration_data.frame_counter;
	cout << ", State=" << respiration_data.sensor_state;
	cout << ", RPM=" << respiration_data.respiration_rate;
	cout << ", Distance=" << respiration_data.distance;
	cout << ", Movement (breathing pattern)=" << respiration_data.movement;
	cout << ", SigQ=" << respiration_data.signal_quality;
	cout << endl;
}

void writeSampleData(char* data)
{
	char szGuid[40] = { 0 };
	GUID gidReference;
	HRESULT hCreateGuid = CoCreateGuid(&gidReference);
	sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", gidReference.Data1, gidReference.Data2, gidReference.Data3,
		gidReference.Data4[0], gidReference.Data4[1], gidReference.Data4[2], gidReference.Data4[3], gidReference.Data4[4], gidReference.Data4[5], gidReference.Data4[6], gidReference.Data4[7]);

	char str[80];
	strcpy(str, directoryForSampleMessage);
	strcat(str, szGuid);
	strcat(str, ".csv");

	FILE * pFile;
	pFile = fopen(str, "a");
	if (pFile != NULL)
	{
		fputs(data, pFile);
		fclose(pFile);
	}	
}

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}


void mcpw_on_host_parser_sleep(SleepData sleep_data, void * user_data)
{
	cout << "Sleep: Counter=" << sleep_data.frame_counter;
	cout << ", State=" << sleep_data.sensor_state;
	cout << ", RPM=" << sleep_data.respiration_rate;
	cout << ", Distance=" << sleep_data.distance;
	cout << ", SigQ=" << sleep_data.signal_quality;
	cout << ", MovementSlow=" << sleep_data.movement_slow;
	cout << ", MovementFast=" << sleep_data.movement_fast;
	cout << endl;

	char buffer[400];
	sprintf(buffer, "TimeStamp; FrameCounter; State; RPM; ObjectDistance; SignalQuality; MovementSlow; MovementFast\n%s;%u;%u;%f;%f;%u;%f;%f\n", currentDateTime().c_str(), sleep_data.frame_counter, sleep_data.sensor_state, sleep_data.respiration_rate, sleep_data.distance, sleep_data.signal_quality, sleep_data.movement_slow, sleep_data.movement_fast);

	writeSampleData(buffer);
}

void mcpw_on_host_parser_presence_single(PresenceSingleData presence_single_data, void * user_data)
{
	cout << "PresenceSingle: Counter=" << presence_single_data.frame_counter;
	cout << ", State=";
	if (presence_single_data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_PRESENCE)
		cout << "Presence";
	else if (presence_single_data.presence_state == XTS_VAL_PRESENCE_PRESENCESTATE_NO_PRESENCE)
		cout << "No presence";
	else
		cout << "Initializing";
	if (presence_single_data.distance>0)
		cout << ", Distance=" << presence_single_data.distance;

	cout << endl;
}

void readThreadMethod(mcp_wrapper_t* mcpw)
{
	ModuleIo* moduleIo = (ModuleIo*)mcpw->user_reference;
	const int rx_buffer_length = 100;
	unsigned char rx_buffer[rx_buffer_length];

	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		moduleIoMutex.lock();
		int received = moduleIo->read(rx_buffer, rx_buffer_length, 100);
		moduleIoMutex.unlock();
		if (received <= 0) continue;

		mcpw_process_rx_data(mcpw, rx_buffer, received);
		//cout << "Read " << read << " bytes" << endl;
	}
}

int mcpw_demo_x4m300(char* com_port)
{
	cout << "Starting mcpw_demo_x4m300." << endl;

	ModuleIo *moduleIo = createModuleIo();
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}

	cout << "Connecting to XeThru module on " << com_port << "." << endl;

	// Configure MCPW.
	void *mcpw_instance_memory = malloc(mcpw_get_instance_size());
	mcp_wrapper_t* mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	mcpw->mcp_host_parser->presence_single = mcpw_on_host_parser_presence_single; // X4M300 presence single message
	mcpw->user_reference = (void*)moduleIo;

	cout << "Starting serial port read thread." << endl;
	std::thread readThread(readThreadMethod, mcpw);

	// X4M300 Presence profile
	int res = 0;
	// Send commands to XeThru module
	res = mcpw_set_sensor_mode(mcpw, XTS_SM_STOP, 0);
	res |= mcpw_load_profile(mcpw, XTS_ID_APP_PRESENCE_2);
	res |= mcpw_set_detection_zone(mcpw, 0.6, 3.0);
	res |= mcpw_set_sensitivity(mcpw, 9);
	res |= mcpw_set_led_control(mcpw, 2, 100);
	res |= mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0);

	// Wait indefinately for readThread to finish.
	readThread.join();

	delete moduleIo;
	moduleIo = NULL;

	free(mcpw_instance_memory);

	return 0;
}

int mcpw_demo_x2m200(char* com_port)
{
	cout << "Starting mcpw_demo_x2m200." << endl;

	ModuleIo *moduleIo = createModuleIo();
	int moduleIoOpenResult = moduleIo->open(com_port);

	if (moduleIoOpenResult != 0)
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return moduleIoOpenResult;
	}

	cout << "Connecting to XeThru module on " << com_port << "." << endl;

	// Configure MCPW.
	void* mcpw_instance_memory = malloc(mcpw_get_instance_size());
	mcp_wrapper_t* mcpw = mcpw_init(mcpw_instance_memory);
	mcpw->send_bytes = mcpw_send_bytes;
	mcpw->wait_for_response = mcpw_wait_for_response;
	mcpw->response_ready = mcpw_response_ready;
	mcpw->mcp_host_parser->sleep = mcpw_on_host_parser_sleep; // X2M200 sleep message
	//mcpw->mcp_host_parser->respiration = mcpw_on_host_parser_respiration; // X2M200 respiration message
	mcpw->user_reference = (void*)moduleIo;

	// X2M200 Sleep profile
	int res = 0;
	// Send commands to XeThru module

	// First perform module reset. Without read thread, this will timeout. Intended.
	mcpw_module_reset(mcpw);
	// In case of USB connection, because of the module reset, we need to close the serial port, then wait for the USB serial port to be available again. Then reconnect.
	// This is not needed when connected to module UART, but it will still work for both connection.
	// Could be removed if only connected using UART.
	delete moduleIo;
	moduleIo = NULL;
	cout << "Reopening " << com_port << " after reset..." << endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	moduleIo = createModuleIo(); // Reopen port.
	if (0 != moduleIo->open(com_port))
	{
		cout << "Error opening " << com_port << ". Aborting." << endl;
		return -1;
	}
	mcpw->user_reference = (void*)moduleIo;

	cout << "Starting serial port read thread." << endl;
	std::thread readThread(readThreadMethod, mcpw);

	res |= mcpw_load_profile(mcpw, XTS_ID_APP_SLEEP);
	//res |= mcpw_load_profile(mcpw, XTS_ID_APP_RESP);
	res |= mcpw_set_detection_zone(mcpw, 0.4, 2.0);
	res |= mcpw_set_sensitivity(mcpw, 5);
	res |= mcpw_set_led_control(mcpw, 2, 100);
	res |= mcpw_set_sensor_mode(mcpw, XTS_SM_RUN, 0);

	readThread.join();

	delete moduleIo;
	moduleIo = NULL;

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 1)
	{
		cout << "Add XeThru module serial port as parameter, e.g. COM1.";
		return -1;
	}
	char* com_port = argv[1];

	if (argc == 4)
	{
		std::string module(argv[2]);
		directoryForSampleMessage = argv[3];
		if ((module == "x2m200") || (module == "X2M200"))
			return mcpw_demo_x2m200(com_port);
	}

	return mcpw_demo_x4m300(com_port);

}
