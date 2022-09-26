#pragma once


#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "fmod_studio.hpp"
#include <string>
#include <map>
#include <iostream>

#endif

#include "Chroma/Math/Math.h"
#include "Chroma/Audio/ChromaFMOD.h"


namespace Chroma
{
	/// @brief Static class for managing and controlling FMOD audio within the engine.
	///
	/// Adapted from <a href="https://codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/">https://codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/</a>
	class Audio
	{
		friend class Application;
		friend struct ChromaFMOD;
	public:
		static void LoadBank(const std::string &bankPath, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);
		static void UnloadBank(const std::string &bankPath);
		static void LoadEvent(const std::string &eventName);
		static void Set3dListenerAndOrientation(const Math::vec3 &position = { 0, 0, 0 }, float volume_dB = 0.0f);
		static void PlayEvent(const std::string &eventName);
		static void PlayEventIfStopped(const std::string &eventName);
		static void StopChannel(int channelId);
		static void StopEvent(const std::string &eventName, bool immediate = false);
		static void GetEventParameter(const std::string &eventName, const std::string &parameter, float *value);
		static void GetEventParameter(const std::string &eventName, FMOD_STUDIO_PARAMETER_ID parameter, float *value);
		static void SetEventParameter(const std::string &eventName, const std::string &parameter, float value, bool ignoreSeekSpeed = false);
		static void SetEventParameter(const std::string &eventName, FMOD_STUDIO_PARAMETER_ID parameter, float value, bool ignoreSeekSpeed = false);
		static void StopAllChannels();
		static void SetChannel3dPosition(int channelId, const Math::vec3 &position);
		static void SetChannelVolume(int channelId, float volume_db);
		static bool IsPlaying(int channelId);
		static bool IsEventPlaying(const std::string &eventName);
		static bool IsEventStopped(const std::string &eventName);
		static float dbToVolume(float db);
		static float volumeTodb(float volume);
		static FMOD_VECTOR VectorToFmod(const Math::vec3 &vector);

		static std::vector<std::string> GetEventPathList();
		static std::string GetEventName(FMOD_GUID guid);
		static FMOD_GUID GetEventGuid(const std::string &name);

		static void UnloadAllBanks();

	private:
		static void Init();
		static void Update();
		static void Shutdown();

		static void ErrorCheck(FMOD_RESULT result);
	};
}
