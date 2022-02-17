using System.Runtime.CompilerServices;


namespace Chroma
{
    /// <summary>
    /// [DEPRICATED] Tag component for the entity.
    /// </summary>
    internal class Tag : Component
    {
        /// <summary>
        /// Entity's name
        /// </summary>
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
