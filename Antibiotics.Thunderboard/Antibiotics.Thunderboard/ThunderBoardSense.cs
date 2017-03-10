using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;

namespace Antibiotics.Thunderboard
{
    public class ThunderBoardSensor
    {
        public static async Task<Dictionary<ThunderBoardSensorType, float>> ReadThunderBoardSensors(BluetoothLEDevice thunderBoard)
        {
            var results = new Dictionary<ThunderBoardSensorType, float>();
            foreach (var serviceToSample in ThunderBoardUuids.ServiceCombinations)
            {
                var service = thunderBoard.GetGattService(Guid.Parse(serviceToSample.ServiceUUID));
                var characteristic = service.GetCharacteristics(Guid.Parse(serviceToSample.CharacteristicUUD)).First();
                var result = await characteristic.ReadValueAsync(BluetoothCacheMode.Uncached);
                if (result.Status == GattCommunicationStatus.Success)
                {
                    var formattedResult = FormatValue(result.Value, serviceToSample.CharacteristicUUD);
                    results.Add(serviceToSample.SensorType, formattedResult);
                }
                else
                {
                    results.Add(serviceToSample.SensorType, 0);
                }
            }

            return await Task.FromResult(results);
        }

        public static float FormatValue(IBuffer buffer, string characteristicsUuid)
        {
            byte[] data;
            CryptographicBuffer.CopyToByteArray(buffer, out data);

            if (characteristicsUuid == ThunderBoardUuids.UUID_CHARACTERISTIC_SOUND_LEVEL)
            {
                return BitConverter.ToInt16(data, 0) / 100.0f;
            }
            else if (characteristicsUuid == ThunderBoardUuids.UUID_CHARACTERISTIC_AMBIENT_LIGHT_SENSE)
            {
                return BitConverter.ToInt16(data, 0) / 100.0f;
            }
            else if (characteristicsUuid == ThunderBoardUuids.UUID_CHARACTERISTIC_HUMIDITY)
            {
                return BitConverter.ToInt16(data, 0) / 100.0f;
            }
            else if (characteristicsUuid == ThunderBoardUuids.UUID_CHARACTERISTIC_TEMPERATURE)
            {
                return BitConverter.ToInt16(data, 0) / 100.0f;
            }
            else if (characteristicsUuid == ThunderBoardUuids.UUID_CHARACTERISTIC_CO2_READING)
            {
                return BitConverter.ToInt16(data, 0) / 1.0f;
            }

            return 0;
        }
    }

    public class BluetoothLEDeviceDisplay
    {
        public BluetoothLEDeviceDisplay(DeviceInformation deviceInfoIn)
        {
            DeviceInformation = deviceInfoIn;            
        }

        public DeviceInformation DeviceInformation { get; private set; }

        public string Id => DeviceInformation.Id;
        public string Name => DeviceInformation.Name;
        public bool IsPaired => DeviceInformation.Pairing.IsPaired;
        public bool IsConnected => (bool?)DeviceInformation.Properties["System.Devices.Aep.IsConnected"] == true;

        public IReadOnlyDictionary<string, object> Properties => DeviceInformation.Properties;
    }
}
