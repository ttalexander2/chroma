using System.Runtime.CompilerServices;


namespace Chroma
{
    internal class Tag : Component
    {
        internal string Name
        {
            get
            {
                return GetTag_Native(Entity.ID);
            }
            set
            {
                SetTag_Native(Entity.ID, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GetTag_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string SetTag_Native(ulong id, string tag);

    }

}
