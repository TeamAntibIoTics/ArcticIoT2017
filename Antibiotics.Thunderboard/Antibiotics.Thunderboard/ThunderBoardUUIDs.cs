using System.Collections.Generic;

namespace Antibiotics.Thunderboard
{
    public enum ThunderBoardSensorType
    {
        Temperature, SoundLevel, Humidity, Carbondioxide, AmbientLight
    }

    public class ThunderBoardServiceCombination
    {
        public ThunderBoardSensorType SensorType { get; set; }

        public string ServiceUUID { get; set; }

        public string CharacteristicUUD { get; set; }
    }

    public class ThunderBoardUuids
    {
        public const string UUID_SERVICE_GENERIC_ACCESS = "00001800-0000-1000-8000-00805f9b34fb";
        public const string UUID_SERVICE_GENERIC_ATTRIBUTE = "00001801-0000-1000-8000-00805f9b34fb";
        public const string UUID_SERVICE_DEVICE_INFORMATION = "0000180a-0000-1000-8000-00805f9b34fb";
        public const string UUID_SERVICE_BATTERY = "0000180f-0000-1000-8000-00805f9b34fb";
        public const string UUID_SERVICE_AUTOMATION_IO = "00001815-0000-1000-8000-00805f9b34fb";
        public const string UUID_SERVICE_CSC = "00001816-0000-1000-8000-00805f9b34fb";
        public const string UUID_SERVICE_ENVIRONMENT_SENSING = "0000181a-0000-1000-8000-00805f9b34fb";
        public const string UUID_SERVICE_ACCELERATION_ORIENTATION = "a4e649f4-4be5-11e5-885d-feff819cdc9f";
        public const string UUID_SERVICE_AMBIENT_LIGHT = "d24c4f4e-17a7-4548-852c-abf51127368b";
        public const string UUID_SERVICE_INDOOR_AIR_QUALITY = "efd658ae-c400-ef33-76e7-91b00019103b";
        public const string UUID_SERVICE_USER_INTERFACE = "fcb89c40-c600-59f3-7dc3-5ece444a401b";
        public const string UUID_SERVICE_POWER_MANAGEMENT = "ec61a454-ed00-a5e8-b8f9-de9ec026ec51";
        public const string UUID_CHARACTERISTIC_DEVICE_NAME = "00002a00-0000-1000-8000-00805f9b34fb"; // Generic Access Service
        public const string UUID_CHARACTERISTIC_APPEARANCE = "00002a01-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_ATTRIBUTE_CHANGED = "00002a05-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_SYSTEM_ID = "00002a23-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_MODEL_NUMBER = "00002a24-0000-1000-8000-00805f9b34fb";     // Device Information Service
        public const string UUID_CHARACTERISTIC_SERIAL_NUMBER = "00002a25-0000-1000-8000-00805f9b34fb";     // Device Information Service
        public const string UUID_CHARACTERISTIC_FIRMWARE_REVISION = "00002a26-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_HARDWARE_REVISION = "00002a27-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_MANUFACTURER_NAME = "00002a29-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_BATTERY_LEVEL = "00002a19-0000-1000-8000-00805f9b34fb"; // Battery Service
        public const string UUID_CHARACTERISTIC_POWER_SOURCE = "EC61A454-ED01-A5E8-B8F9-DE9EC026EC51";
        public const string UUID_CHARACTERISTIC_CSC_CONTROL_POINT = "00002a55-0000-1000-8000-00805f9b34fb"; // CSC Service
        public const string UUID_CHARACTERISTIC_CSC_MEASUREMENT = "00002a5b-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_CSC_FEATURE = "00002a5c-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_CSC_UNKNOWN = "9f70a8fc-826c-4c6f-9c72-41b81d1c9561";
        public const string UUID_CHARACTERISTIC_UV_INDEX = "00002a76-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_PRESSURE = "00002a6d-0000-1000-8000-00805f9b34fb"; // Environment Service
        public const string UUID_CHARACTERISTIC_TEMPERATURE = "00002a6e-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_HUMIDITY = "00002a6f-0000-1000-8000-00805f9b34fb"; // Environment Service
        public const string UUID_CHARACTERISTIC_AMBIENT_LIGHT_REACT = "c8546913-bfd9-45eb-8dde-9f8754f4a32e"; // Ambient Light Service for React board
        public const string UUID_CHARACTERISTIC_AMBIENT_LIGHT_SENSE = "c8546913-bf01-45eb-8dde-9f8754f4a32e"; // Ambient Light Service for Sense board
        public const string UUID_CHARACTERISTIC_SOUND_LEVEL = "c8546913-bf02-45eb-8dde-9f8754f4a32e";
        public const string UUID_CHARACTERISTIC_ENV_CONTROL_POINT = "c8546913-bf03-45eb-8dde-9f8754f4a32e";
        public const string UUID_CHARACTERISTIC_CO2_READING = "efd658ae-c401-ef33-76e7-91b00019103b";
        public const string UUID_CHARACTERISTIC_TVOC_READING = "efd658ae-c402-ef33-76e7-91b00019103b";
        public const string UUID_CHARACTERISTIC_AIR_QUALITY_CONTROL_POINT = "efd658ae-c403-ef33-76e7-91b00019103b";
        public const string UUID_CHARACTERISTIC_ACCELERATION = "c4c1f6e2-4be5-11e5-885d-feff819cdc9f"; // Accelarion and Orientation Service
        public const string UUID_CHARACTERISTIC_ORIENTATION = "b7c4b694-bee3-45dd-ba9f-f3b5e994f49a";
        public const string UUID_CHARACTERISTIC_CALIBRATE = "71e30b8c-4131-4703-b0a0-b0bbba75856b";
        public const string UUID_CHARACTERISTIC_PUSH_BUTTONS = "fcb89c40-c601-59f3-7dc3-5ece444a401b";
        public const string UUID_CHARACTERISTIC_LEDS = "fcb89c40-c602-59f3-7dc3-5ece444a401b";
        public const string UUID_CHARACTERISTIC_RGB_LEDS = "fcb89c40-c603-59f3-7dc3-5ece444a401b";
        public const string UUID_CHARACTERISTIC_UI_CONTROL_POINT = "fcb89c40-c604-59f3-7dc3-5ece444a401b";
        public const string UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION = "00002902-0000-1000-8000-00805f9b34fb"; // Descriptors
        public const string UUID_DESCRIPTOR_CHARACTERISTIC_PRESENTATION_FORMAT = "00002904-0000-1000-8000-00805f9b34fb";
        public const string UUID_CHARACTERISTIC_DIGITAL = "00002a56-0000-1000-8000-00805f9b34fb"; // Automation IO Service

        public static List<ThunderBoardServiceCombination> ServiceCombinations =
            new List<ThunderBoardServiceCombination>
            {
                new ThunderBoardServiceCombination { SensorType = ThunderBoardSensorType.Carbondioxide, ServiceUUID = UUID_SERVICE_INDOOR_AIR_QUALITY, CharacteristicUUD = UUID_CHARACTERISTIC_CO2_READING},
                new ThunderBoardServiceCombination { SensorType = ThunderBoardSensorType.SoundLevel, ServiceUUID = UUID_SERVICE_ENVIRONMENT_SENSING, CharacteristicUUD = UUID_CHARACTERISTIC_SOUND_LEVEL},
                new ThunderBoardServiceCombination { SensorType = ThunderBoardSensorType.Humidity, ServiceUUID = UUID_SERVICE_ENVIRONMENT_SENSING, CharacteristicUUD = UUID_CHARACTERISTIC_HUMIDITY},
                new ThunderBoardServiceCombination { SensorType = ThunderBoardSensorType.Temperature, ServiceUUID = UUID_SERVICE_ENVIRONMENT_SENSING, CharacteristicUUD = UUID_CHARACTERISTIC_TEMPERATURE},
                new ThunderBoardServiceCombination { SensorType = ThunderBoardSensorType.AmbientLight, ServiceUUID = UUID_SERVICE_ENVIRONMENT_SENSING, CharacteristicUUD = UUID_CHARACTERISTIC_AMBIENT_LIGHT_REACT},

            };

        public static Dictionary<string, string> AllThunderBoardUuids =
            new Dictionary<string, string>
            {
                { UUID_SERVICE_GENERIC_ACCESS, "ServiceGenericAccess" } ,
         {UUID_SERVICE_GENERIC_ATTRIBUTE , "ServiceGenericAttribute"},
         {UUID_SERVICE_DEVICE_INFORMATION , "ServiceDeviceInformation"},
         {UUID_SERVICE_BATTERY , "ServiceBattery"},
         {UUID_SERVICE_AUTOMATION_IO , "ServiceAutomation"},
         {UUID_SERVICE_CSC , "ServiceCsc"},
         {UUID_SERVICE_ENVIRONMENT_SENSING , "ServiceEnvironmentSensing"},
         {UUID_SERVICE_ACCELERATION_ORIENTATION , "ServiceAccelerationOrientation"},
         {UUID_SERVICE_AMBIENT_LIGHT , "ServiceAmbientLight"},
         {UUID_SERVICE_INDOOR_AIR_QUALITY , "ServiceIndoorAirQuality"},
         {UUID_SERVICE_USER_INTERFACE , "ServiceUserInterface"},
         {UUID_SERVICE_POWER_MANAGEMENT , "ServicePowerManagement"},
         {UUID_CHARACTERISTIC_DEVICE_NAME , "CharacteristicDeviceName"}, // Generic Access Service
         {UUID_CHARACTERISTIC_APPEARANCE , "CharacteristicAppearance"},
         {UUID_CHARACTERISTIC_ATTRIBUTE_CHANGED , "CharacteristicAttributeChanged"},
         {UUID_CHARACTERISTIC_SYSTEM_ID , "CharacteristicSystemId"},
         {UUID_CHARACTERISTIC_MODEL_NUMBER , "CharacteristicModelNumber"},     // Device Information Service
         {UUID_CHARACTERISTIC_SERIAL_NUMBER , "CharacteristicSerialNumber"},     // Device Information Service
         {UUID_CHARACTERISTIC_FIRMWARE_REVISION , "CharacteristicFirmwareRevision"},
         {UUID_CHARACTERISTIC_HARDWARE_REVISION , "CharacteristicHardwareRevision"},
         {UUID_CHARACTERISTIC_MANUFACTURER_NAME , "CharacteristicManufacturerName"},
         {UUID_CHARACTERISTIC_BATTERY_LEVEL , "CharacteristicBatteryLevel"}, // Battery Service
         {UUID_CHARACTERISTIC_POWER_SOURCE , "CharacteristicPowerSource"},
         {UUID_CHARACTERISTIC_CSC_CONTROL_POINT , "CharacteristicCscControlPoint"}, // CSC Service
         {UUID_CHARACTERISTIC_CSC_MEASUREMENT , "CharacteristicCscMeasurement"},
         {UUID_CHARACTERISTIC_CSC_FEATURE , "CharacteristicCscFeature"},
         {UUID_CHARACTERISTIC_CSC_UNKNOWN , "CharacteristicCscUnknown"},
         {UUID_CHARACTERISTIC_UV_INDEX , "CharacteristicUvIndex"},
         {UUID_CHARACTERISTIC_PRESSURE , "CharacteristicPressure"}, // Environment Service
         {UUID_CHARACTERISTIC_TEMPERATURE , "CharacteristicTemperature"},
         {UUID_CHARACTERISTIC_HUMIDITY , "CharacteristicHumidity"}, // Environment Service
         {UUID_CHARACTERISTIC_AMBIENT_LIGHT_REACT , "CharacteristicAmbientLightReact"}, // Ambient Light Service for React board
         {UUID_CHARACTERISTIC_AMBIENT_LIGHT_SENSE , "CharacteristicAmbientLightSense"}, // Ambient Light Service for Sense board
         {UUID_CHARACTERISTIC_SOUND_LEVEL , "CharacteristicSoundLevel"},
         {UUID_CHARACTERISTIC_ENV_CONTROL_POINT , "CharacteristicEnvControlPoint"},
         {UUID_CHARACTERISTIC_CO2_READING , "CharacteristicCO2Reading"},
         {UUID_CHARACTERISTIC_TVOC_READING , "CharacteristicTvocReading"},
         {UUID_CHARACTERISTIC_AIR_QUALITY_CONTROL_POINT , "CharacteristicAirQualityControlPoint"},
         {UUID_CHARACTERISTIC_ACCELERATION , "CharacteristicAcceleration"}, // Accelarion and Orientation Service
         {UUID_CHARACTERISTIC_ORIENTATION , "CharacteristicOrientation"},
         {UUID_CHARACTERISTIC_CALIBRATE , "CharacteristicCalibrate"},
         {UUID_CHARACTERISTIC_PUSH_BUTTONS , "CharacteristicPushButtons"},
         {UUID_CHARACTERISTIC_LEDS , "CharacteristicLeds"},
         {UUID_CHARACTERISTIC_RGB_LEDS , "CharacteristicRgbLeds"},
         {UUID_CHARACTERISTIC_UI_CONTROL_POINT , "CharacteristicUIControlPoint"},
         {UUID_DESCRIPTOR_CLIENT_CHARACTERISTIC_CONFIGURATION , "DescriptorClientCharacteristicConfiguration"}, // Descriptors
         {UUID_DESCRIPTOR_CHARACTERISTIC_PRESENTATION_FORMAT , "DescriptorCharacteristicPresentationFormat"},
         {UUID_CHARACTERISTIC_DIGITAL, "CharacteristicDigital"}, // Automation IO Service
            };
    }
}
