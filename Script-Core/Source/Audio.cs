using System;
using System.Runtime.CompilerServices;

namespace Chroma
{
    public static class Audio
    {

        public static void PlayEvent(string event_name)
        {
            PlayEvent_Native(event_name);
        }

        public static void PlayEventIfStopped(string event_name)
        {
            PlayEventIfStopped_Native(event_name);
        }

        public static void StopEvent(string event_name, bool immediate = false)
        {
            StopEvent_Native(event_name, immediate);
        }

        public static float GetEventParameter(string event_name, string parameter)
        {
            return GetEventParameter_Native(event_name, parameter);
        }

        public static void SetEventParameter(string event_name, string parameter, float value)
        {
            SetEventParameter_Native(event_name, parameter, value);
        }

        public static bool IsEventPlaying(string event_name)
        {
            return IsEventPlaying_Native(event_name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void PlayEvent_Native(string event_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void PlayEventIfStopped_Native(string event_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void StopEvent_Native(string event_name, bool immediate);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetEventParameter_Native(string event_name, string parameter);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetEventParameter_Native(string event_name, string parameter, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool IsEventPlaying_Native(string event_name);
    }
}