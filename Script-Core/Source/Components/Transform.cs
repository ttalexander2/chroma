using System.Runtime.CompilerServices;

namespace Chroma
{
    public class Transform : Component
    {
        public Vector2 Position
        {
            get
            {
                Vector2 result;
                GetTransformPosition_Native(Entity.ID, out result);
                return result;
            }
            set 
            {
                SetTransformPosition_Native(Entity.ID, value);
            }
        }
        public float Rotation;
        public Vector2 Scale
        {
            get
            {
                Vector2 result;
                GetTransformScale_Native(Entity.ID, out result);
                return result;
            }
            set
            {
                SetTransformScale_Native(Entity.ID, value);
            }
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransformPosition_Native(ulong id, out Vector2 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetTransformRotation_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransformScale_Native(ulong id, out Vector2 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformPosition_Native(ulong id, Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformRotation_Native(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformScale_Native(ulong id, Vector2 value);

    }
}
