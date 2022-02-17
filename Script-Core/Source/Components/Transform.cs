using System.Runtime.CompilerServices;

namespace Chroma
{
    /// <summary>
    /// Transform component for the entity. The engine enforces the existance of this component for all entities.
    /// </summary>
    public class Transform : Component
    {
        /// <summary>
        /// Positional component for the transform. Represents the position of the entity in the scene.
        /// </summary>
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

        /// <summary>
        /// Rotational component for the transform. Represents the euler angle in degrees of the entity.
        /// Rotation is internally stored in radians and is converted in this geter/setter.
        /// Use <c cref="RadRotation">RadRotation</c> for accessing the rotation in radians.
        /// </summary>
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

        /// <summary>
        /// Rotational component for the transform in radians.
        /// Use <c cref="Rotation">Rotation</c> for accessing rotation in degrees.
        /// </summary>
        public float RadRotation
        {
            get
            {
                return GetTransformRotation_Native(Entity.ID);
            }
            set
            {
                SetTransformRotation_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Scale component of the transform.
        /// </summary>
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

        /// <summary>
        /// Gets the transform component as a string.
        /// </summary>
        /// <returns>Formatted string containing transform data.</returns>
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
