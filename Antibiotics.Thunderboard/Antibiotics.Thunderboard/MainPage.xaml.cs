using System;
using System.Collections.Generic;
using System.Text;
using Windows.Devices.Bluetooth;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using System.Diagnostics;
using Microsoft.Azure.EventHubs;
using System.Threading.Tasks;
using Antibiotics.DataModel;
using Newtonsoft.Json;
using System.Linq;
using Antibiotics.Thunderboard;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace ThunderboardTest
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private bool foundTheSense = false;
        private Dictionary<string, DeviceInformation> BlueToothSensesNearby = new Dictionary<string, DeviceInformation>();
        private DeviceWatcher deviceWatcher;
        private BluetoothLEDevice bluetoothLeDevice = null;
        private DispatcherTimer _timer;

        private static EventHubClient eventHubClient;
        private const string EhConnectionString = "";
        private const string EhEntityPath = "sensordata";

        private int totalSent = 0;

        public MainPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {

        }

        private void ClearBluetoothLEDevice()
        {
            bluetoothLeDevice?.Dispose();
            bluetoothLeDevice = null;
        }

        private void StartBleDeviceWatcher()
        {
            foundTheSense = false;
            ClearBluetoothLEDevice();
            // Additional properties we would like about the device.
            string[] requestedProperties = { "System.Devices.Aep.DeviceAddress", "System.Devices.Aep.IsConnected" };

            // BT_Code: Currently Bluetooth APIs don't provide a selector to get ALL devices that are both paired and non-paired.
            deviceWatcher =
                    DeviceInformation.CreateWatcher(
                        "(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\")",
                        requestedProperties,
                        DeviceInformationKind.AssociationEndpoint);
            
            deviceWatcher.Added += DeviceWatcher_Added;           
            BlueToothSensesNearby.Clear();            
            deviceWatcher.Start();
        }

        private async void DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation deviceInfo)
        {         
            if (sender == deviceWatcher)
            {         
                if (deviceInfo.Name != string.Empty && !BlueToothSensesNearby.ContainsKey(deviceInfo.Name) && !foundTheSense)
                {
                    if (deviceInfo.Name.Contains("Thunder Sense"))
                    {
                        foundTheSense = true;
                        BlueToothSensesNearby.Add(deviceInfo.Name, deviceInfo);
                        if (deviceWatcher != null)
                        {             
                            deviceWatcher.Added -= DeviceWatcher_Added;                                                     
                            deviceWatcher.Stop();
                            deviceWatcher = null;                            
                        }                        
                    }
                }
            }

            await Task.FromResult(true);
        }

        private async void ConnectoToTheSense()
        {
            try
            {
                var senseDevice = BlueToothSensesNearby.First();                
                if (!senseDevice.Value.Pairing.IsPaired)
                {
                    var result = senseDevice.Value.Pairing.PairAsync().GetResults();
                }
                bluetoothLeDevice = await BluetoothLEDevice.FromIdAsync(senseDevice.Value.Id);                
            }
            catch (Exception ex) when ((uint)ex.HResult == 0x800710df)
            {
                // ERROR_DEVICE_NOT_AVAILABLE because the Bluetooth radio is not on.
                Debug.WriteLine("Failed to connect");
            }
        }

        private async void Connect_Click(object sender, RoutedEventArgs e)
        {
            StartBleDeviceWatcher();
                   
            while (!foundTheSense)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    Message.Text = "Searching for the Sense...";
                });
            }

            ConnectoToTheSense();

            _timer = new DispatcherTimer();
            _timer.Tick += _timer_Tick;
            _timer.Interval = new TimeSpan(0, 0, 0, 10);
            _timer.Start();
        }

        private async void _timer_Tick(object sender, object e)
        {
            Debug.WriteLine("Timer Tick");
            await SampleDataAndSendToEventHub();
        }

        private async Task SampleDataAndSendToEventHub()
        {
            var sensorReadings = await ThunderBoardSensor.ReadThunderBoardSensors(bluetoothLeDevice);
            
            var connectionStringBuilder = new EventHubsConnectionStringBuilder(EhConnectionString)
            {
                EntityPath = EhEntityPath
            };

            eventHubClient = EventHubClient.CreateFromConnectionString(connectionStringBuilder.ToString());

            await SendMessageToEventHub(sensorReadings);

            await eventHubClient.CloseAsync();
        }

        private async Task SendMessageToEventHub(Dictionary<ThunderBoardSensorType, float> sensorReadings)
        {
            try
            {   
                var thunderBoardMsg = new ThunderBoard();
                thunderBoardMsg.DeviceId = "123456";
                thunderBoardMsg.EventId = Guid.NewGuid().ToString();
                thunderBoardMsg.Timestamp = DateTime.Now;
                thunderBoardMsg.Temperature = sensorReadings[ThunderBoardSensorType.Temperature];
                thunderBoardMsg.Humidity = sensorReadings[ThunderBoardSensorType.Humidity];
                thunderBoardMsg.AmbientLight = sensorReadings[ThunderBoardSensorType.AmbientLight];
                thunderBoardMsg.CO2Level = sensorReadings[ThunderBoardSensorType.Carbondioxide];
                thunderBoardMsg.SoundLevel = sensorReadings[ThunderBoardSensorType.SoundLevel];
                var message = JsonConvert.SerializeObject(thunderBoardMsg);
                Debug.WriteLine($"Sending message: {message}");
                await eventHubClient.SendAsync(new EventData(Encoding.UTF8.GetBytes(message)));
                totalSent++;
                Message.Text = DateTime.Now.ToString() + " Melding sendt. Totalt antall: " + totalSent;
            }
            catch (Exception exception)
            {
                Debug.WriteLine($"{DateTime.Now} > Exception: {exception.Message}");
            }
        }
    }
}
