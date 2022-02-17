using System;
using System.Runtime.CompilerServices;


namespace Chroma
{
    /// <summary>
    /// Base component class.
    /// </summary>
    public class Component
    {
        /// <summary>
        /// Reference to the entity containing this component.
        /// </summary>
        public Entity Entity { get; internal set; }

        /// <summary>
        /// Value representing whether the component is enabled.
        /// </summary>
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
