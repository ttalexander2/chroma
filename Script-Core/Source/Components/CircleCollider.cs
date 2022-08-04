using System;
using System.Runtime.CompilerServices;

namespace Chroma
{
    
    public class CircleCollider : Collider
    {
        
        public float Radius
        {
            get
            {
                return GetRadius_Native(Entity.ID);
            }
            set
            {
                SetRadius_Native(Entity.ID, value);
            }
        }

        public override ColliderType GetColliderType()
        {
            return ColliderType.Circle;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRadius_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRadius_Native(ulong id, float val);
    }

}