using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


namespace Chroma
{

    public class CollisionContact
    {
        public CollisionManifold Manifold { get; private set; }
        public WorldManifold WorldManifold { get; private set; }
        public readonly bool IsTouching;

        public Collider ColliderA { get; private set; }
        public Collider ColliderB { get; private set; }

        private IntPtr _pointer = IntPtr.Zero;

        public float Friction
        {
            get
            {
                return GetFriction_Native(_pointer);
            }
            set
            {
                SetFriction_Native(_pointer, value);
            }
        }

        public float Restitution
        {
            get
            {
                return GetRestitution_Native(_pointer);
            }
            set
            {
                SetRestitution_Native(_pointer, value);
            }
        }

        public float RestitutionThreshold
        {
            get
            {
                return GetRestitutionThreshold_Native(_pointer);
            }
            set
            {
                SetRestitutionThreshold_Native(_pointer, value);
            }
        }

        public float TangentSpeed
        {
            get
            {
                return GetTangentSpeed_Native(_pointer);
            }
            set
            {
                SetTangentSpeed_Native(_pointer, value);
            }
        }

        internal CollisionContact(IntPtr ptr, ulong colliderA, ulong colliderB, Collider.ColliderType colliderTypeA, Collider.ColliderType colliderTypeB, CollisionManifold manifold, WorldManifold worldManifold, bool isTouching)
        {
            _pointer = ptr;
            Entity entityA = new Entity(colliderA);
            Entity entityB = new Entity(colliderB);
            ColliderA = Collider.Create(entityA, colliderTypeA);
            ColliderB = Collider.Create(entityA, colliderTypeB);
            Manifold = manifold;
            WorldManifold = worldManifold;
            IsTouching = isTouching;
        }

        public void ResetFriction()
        {
            ResetFriction_Native(_pointer);
        }

        public void ResetRestitution()
        {
            ResetRestitution_Native(_pointer);
        }

        public void ResetRestitutionThreshold()
        {
            ResetRestitutionThreshold_Native(_pointer);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetFriction_Native(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetFriction_Native(IntPtr ptr, float friction);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRestitution_Native(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRestitution_Native(IntPtr ptr, float restitution);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRestitutionThreshold_Native(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRestitutionThreshold_Native(IntPtr ptr, float restitutionThreshold);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetTangentSpeed_Native(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTangentSpeed_Native(IntPtr ptr, float tangentSpeed);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ResetFriction_Native(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ResetRestitution_Native(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ResetRestitutionThreshold_Native(IntPtr ptr);


    }

}