using System.Runtime.CompilerServices;

namespace Chroma
{
    public class Transform : Component
    {
        public Vector2 Position
        {
            get
            {
                GetTransformPosition_Native(Entity.ID, out Vector2 result);
                return result;
            }
            set 
            {
                SetTransformPosition_Native(Entity.ID, ref value);
            }
        }

        public float Rotation
        {
            get
            {
                return Mathf.RadiansToDegrees(GetTransformRotation_Native(Entity.ID));
            }
            set
            {
                SetTransformRotation_Native(Entity.ID, Mathf.DegreesToRadians(value));
            }
        }
        public Vector2 Scale
        {
            get
            {
                GetTransformScale_Native(Entity.ID, out Vector2 result);
                return result;
            }
            set
            {
                SetTransformScale_Native(Entity.ID, ref value);
            }
        }

        public override string ToString()
        {
            return string.Format("Position: {0}, Rotation: {1}, Scale: {2}", Position.ToString(), Rotation, Scale.ToString());
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransformPosition_Native(ulong id, out Vector2 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetTransformRotation_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransformScale_Native(ulong id, out Vector2 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformPosition_Native(ulong id, ref Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformRotation_Native(ulong id, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformScale_Native(ulong id, ref Vector2 value);

    }
}
