using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Chroma
{

    [StructLayout(LayoutKind.Sequential)]
    public struct ContactImpulse
    {
        public readonly List<float> NormalImpulses;
        public readonly List<float> TangentImpulses;
    }
}