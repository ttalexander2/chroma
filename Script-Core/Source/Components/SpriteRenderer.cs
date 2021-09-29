using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Chroma
{
    public class SpriteRenderer : Component
    {
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

        public void SetAnimationByTag(string tag) 
        {
            SetAnimationByTag_Native(Entity.ID, tag);
        }
        public string GetAnimationTag(uint animation) 
        {
            return GetAnimationTag_Native(Entity.ID, animation);
        }
        public string GetAnimationTag()
        {
            return GetAnimationTag(Animation);
        }

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
