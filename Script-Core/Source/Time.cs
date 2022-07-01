using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chroma
{
    public static class Time
    {
        public static double Delta { get; internal set; }
        public static float DeltaF { get; internal set; }
        public static double FixedDelta { get; internal set; }
        public static float FixedDeltaF { get; internal set; }
    }
}
