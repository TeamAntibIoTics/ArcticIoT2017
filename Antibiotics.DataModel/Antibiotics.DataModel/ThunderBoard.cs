using System;

namespace Antibiotics.DataModel
{
    public class ThunderBoard
    {
        public string DeviceId { get; set; }

        public string EventId { get; set; }

        public DateTime Timestamp { get; set; }

        public float Temperature { get; set; }

        public float AmbientLight { get; set; }

        public float Humidity { get; set; }

        public float SoundLevel { get; set; }

        public float CO2Level { get; set; }
    }
}
