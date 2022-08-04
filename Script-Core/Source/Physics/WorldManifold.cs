using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


namespace Chroma
{
    [StructLayout(LayoutKind.Sequential)]
    public struct WorldManifold
    {
        public Vector2 Normal { get; private set; }
        public List<Vector2> Points { get; private set; }
        public List<float> Separations { get; private set; }
    }

}
