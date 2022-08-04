using System;
using System.Runtime.CompilerServices;


namespace Chroma
{
    /// <summary>
    /// Rigidbody
    /// </summary>
    public class RigidBody : Component
    {

        public enum BodyType : int
        {
            Static = 0,
            Kinematic = 1,
            Dynamic = 2
        }

        public BodyType Body
        {
            get
            {
                return (BodyType)GetBodyType_Native(Entity.ID);
            }
            set
            {
                SetBodyType_Native(Entity.ID, (int)value);
            }
        }

        public Vector2 LinearVelocity
        {
            get
            {
                GetLinearVelocity_Native(Entity.ID, out Vector2 value);
                return value;
            }
            set
            {
                SetLinearVelocity_Native(Entity.ID, ref value);
            }
        }

        public float AngularVelocity
        {
            get
            {
                return GetAngularVelocity_Native(Entity.ID);
            }
            set
            {
                SetAngularVelocity_Native(Entity.ID, value);
            }
        }

        public float LinearDamping
        {
            get
            {
                return GetLinearDamping_Native(Entity.ID);
            }
            set
            {
                SetLinearDamping_Native(Entity.ID, value);
            }
        }

        public float AngularDamping
        {
            get
            {
                return GetAngularDamping_Native(Entity.ID);
            }
            set
            {
                SetAngularDamping_Native(Entity.ID, value);
            }
        }

        public bool SleepingAllowed
        {
            get
            {
                return GetSleepingAllowed_Native(Entity.ID);
            }
            set
            {
                SetSleepingAllowed_Native(Entity.ID, value);
            }
        }

        public bool Awake
        {
            get
            {
                return GetAwake_Native(Entity.ID);
            }
            set
            {
                SetAwake_Native(Entity.ID, value);
            }
        }

        public bool FixedRotation
        {
            get
            {
                return GetFixedRotation_Native(Entity.ID);
            }
            set
            {
                SetFixedRotation_Native(Entity.ID, value);
            }
        }

        public bool Bullet
        {
            get
            {
                return GetBullet_Native(Entity.ID);
            }
            set
            {
                SetBullet_Native(Entity.ID, value);
            }
        }

        public float GravityScale
        {
            get
            {
                return GetGravityScale_Native(Entity.ID);
            }
            set
            {
                SetGravityScale_Native(Entity.ID, value);
            }
        }



        public RigidBody() { }


        public void ApplyForce(Vector2 force, Vector2 point, bool wake = true)
        {
            ApplyForce_Native(Entity.ID, ref force, ref point, wake);
        }

        public void ApplyForce(Vector2 force, bool wake = true)
        {
            ApplyForceToCenter_Native(Entity.ID, ref force, wake);
        }

        public void ApplyTorque(float torque, bool wake = true)
        {
            ApplyTorque_Native(Entity.ID, torque, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, Vector2 point, bool wake = true)
        {
            ApplyLinearImpulse_Native(Entity.ID, ref impulse, ref point, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake = true)
        {
            ApplyLinearImpulseToCenter_Native(Entity.ID, ref impulse, wake);
        }

        public void ApplyAngularImpulse(float impulse, bool wake = true)
        {
            ApplyAngularImpulse_Native(Entity.ID, impulse, wake);
        }




        #region Internal Calls
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern int GetBodyType_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetBodyType_Native(ulong id, int val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetLinearVelocity_Native(ulong id, out Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetLinearVelocity_Native(ulong id, ref Vector2 val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetAngularVelocity_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetAngularVelocity_Native(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetLinearDamping_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetLinearDamping_Native(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetAngularDamping_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetAngularDamping_Native(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetSleepingAllowed_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetSleepingAllowed_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetAwake_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetAwake_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetFixedRotation_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetFixedRotation_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetBullet_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetBullet_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetGravityScale_Native(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetGravityScale_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ApplyForce_Native(ulong id, ref Vector2 force, ref Vector2 point, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ApplyForceToCenter_Native(ulong id, ref Vector2 force, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ApplyTorque_Native(ulong id, float torque, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ApplyLinearImpulse_Native(ulong id, ref Vector2 impulse, ref Vector2 point, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ApplyLinearImpulseToCenter_Native(ulong id, ref Vector2 impulse, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ApplyAngularImpulse_Native(ulong id, float impulse, bool wake);

        #endregion
    }
}