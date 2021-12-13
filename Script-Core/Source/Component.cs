using System;
using System.Runtime.CompilerServices;


namespace Chroma
{

    public class Component
    {
        public Entity Entity { get; internal set; }

        public bool Enabled {
            get { return GetEnabled_Native(Entity.ID, this.GetType()); }
            set { SetEnabled_Native(Entity.ID, this.GetType(), value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetEnabled_Native(ulong entityID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetEnabled_Native(ulong entityID, Type type, bool val);


    }



}
