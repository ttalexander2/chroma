using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Chroma
{
    /// <summary>
    /// Sprite Renderer Component. Controls rendering a sprite for an entity.
    /// </summary>
    public class SpriteRenderer : Component
    {
        /// <summary>
        /// Color of the sprite. Default is white.
        /// </summary>
        /// <remarks>
        /// Color is multiplicatively blended with the texture in the fragment shader.
        /// </remarks>
        public Vector4 Color
        {
            get
            {
                GetColor_Native(Entity.ID, out Vector4 result);
                return result;
            }
            set
            {
                SetColor_Native(Entity.ID, ref value);
            }
        }

        /// <summary>
        /// Offset of the sprite. By default the sprite renders starting at the entity's transform position. The offset is added to the transform.
        /// </summary>
        public Vector2 Offset
        {
            get
            {
                GetOffset_Native(Entity.ID, out Vector2 result);
                return result;
            }
            set
            {
                SetOffset_Native(Entity.ID, ref value);
            }
        }

        /// <summary>
        /// [DEPRICATED] Layer of the sprite.
        /// </summary>
        public string Layer
        {
            get
            {
                return GetLayer_Native(Entity.ID);
            }
            set
            {
                SetLayer_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Whether the sprite should begin playing at the start of the scene.
        /// If not, setting <c cref="Playing">Playing</c> to true will begin the sprite's animation.
        /// Only applies if the sprite is animated.
        /// </summary>
        public bool PlayOnStart
        {
            get
            {
                return GetPlayOnStart_Native(Entity.ID);
            }
            set
            {
                SetPlayOnStart_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Bool representing whether the sprite is playing.
        /// Only applies if the sprite is animated.
        /// </summary>
        public bool Playing
        {
            get
            {
                return GetPlaying_Native(Entity.ID);
            }
            set
            {
                SetPlaying_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Whether the sprite animation should loop.
        /// If false, the animation will stop when it hits the last frame.
        /// Only applies if the sprite is animated.
        /// </summary>
        public bool Loop
        {
            get
            {
                return GetLoop_Native(Entity.ID);
            }
            set
            {
                SetLoop_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Speed multiplier of the sprite. Default is 1.0.
        /// Only applies if the sprite is animated.
        /// </summary>
        public float Speed
        {
            get
            {
                return GetSpeedMultiplier_Native(Entity.ID);
            }
            set
            {
                SetSpeedMultiplier_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Sprite to render. Setting this to an ID of a sprite that does not exist will result in a warning.
        /// </summary>
        public string SpriteID
        {
            get
            {
                return GetSprite_Native(Entity.ID);
            }
            set
            {
                SetSprite_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Set sprite animation by index. Index will be bounded by number of animations the sprite contains.
        /// Only applies to animated sprites.
        /// </summary>
        public uint Animation
        {
            get
            {
                return GetAnimation_Native(Entity.ID);
            }
            set
            {
                SetAnimation_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// The current frame of the animated sprite. This frame is independent of the selected animation (ie spans frames of all animations).
        /// Only applies to animated sprites.
        /// </summary>
        public uint Frame
        {
            get
            {
                return GetFrame_Native(Entity.ID);
            }
            set
            {
                SetFrame_Native(Entity.ID, value);
            }
        }

        /// <summary>
        /// Sets the sprite animation by the animation's tag.
        /// </summary>
        /// <param name="tag">Name of the sprite's animation. This is case sensitive.</param>
        public void SetAnimationByTag(string tag) 
        {
            SetAnimationByTag_Native(Entity.ID, tag);
        }

        /// <summary>
        /// Gets the name of the animation from the numerical ID.
        /// </summary>
        /// <param name="animation">Numerical ID of the animation </param>
        /// <returns>Name of the animation.</returns>
        public string GetAnimationTag(uint animation) 
        {
            return GetAnimationTag_Native(Entity.ID, animation);
        }

        /// <summary>
        /// Gets the current animation's tag.
        /// </summary>
        /// <returns>Name/Tag of the current animation</returns>
        public string GetAnimationTag()
        {
            return GetAnimationTag(Animation);
        }

        /// <summary>
        /// Restarts the current animation.
        /// </summary>
        public void RestartAnimation() 
        {
            RestartAnimation_Native(Entity.ID);
        }


        //SpriteRenderer

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetColor_Native(ulong id, out Vector4 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetColor_Native(ulong id, ref Vector4 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetOffset_Native(ulong id, out Vector2 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetOffset_Native(ulong id, ref Vector2 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GetLayer_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetLayer_Native(ulong id, string val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetPlayOnStart_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetPlayOnStart_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetPlaying_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetPlaying_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetLoop_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetLoop_Native(ulong id, bool val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetSpeedMultiplier_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetSpeedMultiplier_Native(ulong id, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GetSprite_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetSprite_Native(ulong id, string val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint GetAnimation_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetAnimation_Native(ulong id, uint val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint GetFrame_Native(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetFrame_Native(ulong id, uint val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string GetAnimationTag_Native(ulong id, uint val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetAnimationByTag_Native(ulong id, string val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void RestartAnimation_Native(ulong id);

    }
}
