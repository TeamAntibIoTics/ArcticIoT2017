using System;

namespace Antibiotics.Thunderboard
{
    public class RandomData
    {
        private Random _random = new Random();

        public float Temperature { get {return _random.Next(15,25); } }

        public float Humidity { get { return _random.Next(0, 100); } }

        public float AmbientLight { get { return _random.Next(0, 10); } }

        public float CO2Level { get { return _random.Next(20, 1000); } }

        public float SoundLevel { get { return _random.Next(50, 70); } }
    }
}
