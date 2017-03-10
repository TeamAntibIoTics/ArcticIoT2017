using System;

namespace Antibiotics.DataModel
{
    public class X2M200Respiration
    {
        public string DeviceId { get; set; }

        public string EventId { get; set; }

        public DateTime Timestamp { get; set; }

        public int FrameCounter { get; set; }

        public int SensorState { get; set; }

        public float RespirationRate { get; set; }

        public float Distance { get; set; }

        public float MovementSlow { get; set; }

        public int SignalQuality { get; set; }
    }
}
