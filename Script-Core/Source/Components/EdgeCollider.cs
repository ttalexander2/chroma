using System;
using System.Runtime.CompilerServices;


namespace Chroma
{
    public class EdgeCollider : Collider
    {

        public bool OneSided
        {
            get
            {
                return GetOneSided_Native(Entity.ID);
            }
            set
            {
                SetOneSided_Native(Entity.ID, value);
            }
        }

        public Vector2 V0
        {
            get
            {
                GetV0_Native(Entity.ID, out Vector2 value);
                return value;
            }
            set
            {
                SetV0_Native(Entity.ID, ref value);
            }
        }

        public Vector2 V1
        {
            get
            {
                GetV1_Native(Entity.ID, out Vector2 value);
                return value;
            }
            set
            {
                SetV1_Native(Entity.ID, ref value);
            }
        }

        public Vector2 V2
        {
            get
            {
                GetV2_Native(Entity.ID, out Vector2 value);
                return value;
            }
            set
            {
                SetV2_Native(Entity.ID, ref value);
            }
        }

        public Vector2 V3
        {
            get
            {
                GetV3_Native(Entity.ID, out Vector2 value);
                return value;
            }
            set
            {
                SetV3_Native(Entity.ID, ref value);
            }
        }

        public override ColliderType GetColliderType()
        {
            return ColliderType.Edge;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetOneSided_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetOneSided_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetV0_Native(ulong id, out Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetV0_Native(ulong id, ref Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetV1_Native(ulong id, out Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetV1_Native(ulong id, ref Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetV2_Native(ulong id, out Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetV2_Native(ulong id, ref Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetV3_Native(ulong id, out Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetV3_Native(ulong id, ref Vector2 val);

    }
}