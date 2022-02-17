using System;
using System.Runtime.CompilerServices;

namespace Chroma
{
    /// <summary>
    /// Audio management class. These set of functions wrap the internal engine audio api and the FMOD api.
    /// </summary>
    public static class Audio
    {
        /// <summary>
        /// Plays an audio event.
        /// </summary>
        /// <param name="event_name">Name/ID of the event.</param>
        public static void PlayEvent(string event_name)
        {
            PlayEvent_Native(event_name);
        }

        /// <summary>
        /// Plays an audio event ONLY if it is not playing. 
        /// This function can be used if the playing state of the event us unknown.
        /// </summary>
        /// <param name="event_name">Name/ID of the event.</param>
        public static void PlayEventIfStopped(string event_name)
        {
            PlayEventIfStopped_Native(event_name);
        }

        /// <summary>
        /// Stops an audio event from playing.
        /// </summary>
        /// <param name="event_name">Name/ID of the event.</param>
        /// <param name="immediate">Whether the event should stop immediately. If not, the event will follow any ending/fading behavior specified.</param>
        public static void StopEvent(string event_name, bool immediate = false)
        {
            StopEvent_Native(event_name, immediate);
        }

        /// <summary>
        /// Get a parameter value of an event.
        /// </summary>
        /// <param name="event_name">Name/ID of the event.</param>
        /// <param name="parameter">Name/ID of the parameter.</param>
        /// <returns></returns>
        public static float GetEventParameter(string event_name, string parameter)
        {
            return GetEventParameter_Native(event_name, parameter);
        }

        /// <summary>
        /// Set a parameter value of an event.
        /// </summary>
        /// <param name="event_name">Name/ID of the event.</param>
        /// <param name="parameter">Name/ID of the parameter to set.</param>
        /// <param name="value">Value to set the parameter</param>
        public static void SetEventParameter(string event_name, string parameter, float value)
        {
            SetEventParameter_Native(event_name, parameter, value);
        }

        /// <summary>
        /// Checks whether the event is currently playing.
        /// </summary>
        /// <param name="event_name">Name/ID of the event.</param>
        /// <returns>Playing status.</returns>
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