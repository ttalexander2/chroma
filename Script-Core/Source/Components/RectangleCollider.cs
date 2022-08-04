using System;
using System.Runtime.CompilerServices;


namespace Chroma
{

    public class RectangleCollider : Collider
    {
        

        public Vector2 Size
        {
            get
            {
                GetSize_Native(Entity.ID, out Vector2 value);
                return value;
            }
            set
            {
                SetSize_Native(Entity.ID, ref value);
            }
        }

        public float Rotation
        {
            get
            {
                return GetRotation_Native(Entity.ID);
            }
            set
            {
                SetRotation_Native(Entity.ID, value);
            }
        }

        public override ColliderType GetColliderType()
        {
            return ColliderType.Rectangle;
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetSize_Native(ulong id, ref Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetSize_Native(ulong id, out Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRotation_Native(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRotation_Native(ulong id);

    }
}