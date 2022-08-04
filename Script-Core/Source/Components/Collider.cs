using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


namespace Chroma
{
    /// <summary>
    /// Camera component
    /// </summary>
    public abstract class Collider : Component
    {

        public uint Mask
        {
            get
            {
                return GetMask_Native(Entity.ID, GetType());
            }
            set
            {
                SetMask_Native(Entity.ID, value, GetType());
            }
        }

        public uint Layer
        {
            get
            {
                return GetLayer_Native(Entity.ID, GetType());
            }
            set
            {
                SetLayer_Native(Entity.ID, value, GetType());
            }
        }

        public float Friction
        {
            get
            {
                return GetFriction_Native(Entity.ID, GetType());
            }
            set
            {
                SetFriction_Native(Entity.ID, value, GetType());
            }
        }


        public float Restitution
        {
            get
            {
                return GetRestitution_Native(Entity.ID, GetType());
            }
            set
            {
                SetRestitution_Native(Entity.ID, value, GetType());
            }
        }

        public float RestitutionThreshold
        {
            get
            {
                return GetRestitutionThreshold_Native(Entity.ID, GetType());
            }
            set
            {
                SetRestitutionThreshold_Native(Entity.ID, value, GetType());
            }
        }

        public float Density
        {
            get
            {
                return GetDensity_Native(Entity.ID, GetType());
            }
            set
            {
                SetDensity_Native(Entity.ID, value, GetType());
            }
        }

        public bool Sensor
        {
            get
            {
                return GetSensor_Native(Entity.ID, GetType());
            }
            set
            {
                SetSensor_Native(Entity.ID, value, GetType());
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Vertex
        {
            public Vector2 vertex;
            public Vector2 normal;
        }

        public enum ColliderType
        {
            Circle = 0,
            Edge = 1,
            Polygon = 2,
            Rectangle = 3
        }

        public abstract ColliderType GetColliderType();

        public static Collider Create(Entity entity, ColliderType type)
        {
            switch (type)
            {
                case ColliderType.Circle:  return entity.CreateComponent<CircleCollider>();
                case ColliderType.Edge: return entity.CreateComponent<EdgeCollider>();
                case ColliderType.Polygon: return entity.CreateComponent<PolygonCollider>();
                case ColliderType.Rectangle: return entity.CreateComponent<RectangleCollider>();
                default: break;
            }
            return null;
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint GetMask_Native(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetMask_Native(ulong id, uint val, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint GetLayer_Native(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetLayer_Native(ulong id, uint val, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetFriction_Native(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetFriction_Native(ulong id, float val, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRestitution_Native(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRestitution_Native(ulong id, float val, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRestitutionThreshold_Native(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRestitutionThreshold_Native(ulong id, float val, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetDensity_Native(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetDensity_Native(ulong id, float val, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetSensor_Native(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetSensor_Native(ulong id, bool val, Type type);

    }
}