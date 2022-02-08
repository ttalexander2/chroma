#pragma once

#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "fmod_studio.hpp"
#include <string>
#include <map>
#include <iostream>

#endif

namespace Chroma
{

	/// @brief Struct to interface with a loaded FMOD Studio instance.
	struct ChromaFMOD
	{
		ChromaFMOD();
		~ChromaFMOD();


		void Update();

		/// @brief FMOD Studio System instance.
		FMOD::Studio::System* m_StudioSystem;

		/// @brief FMOD System instance.
		FMOD::System* m_System;

		/// @brief Counter for keeping track of FMOD channels.
		int m_NextChanelID;

		/// @brief Type for a map of FMOD Sounds.
		typedef std::map<std::string, FMOD::Sound*> SoundMap;
		/// @brief Type for a map of FMOD Channels.
		typedef std::map<int, FMOD::Channel*> ChannelMap;
		/// @brief Type for a map of FMOD Event Instances.
		typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
		/// @brief Type for a map of FMOD Banks.
		typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

		/// @brief Map of FMOD banks.
		BankMap m_Banks;
		/// @brief Map of FMOD events.
		EventMap m_Events;
		/// @brief Map of FMOD asynchronous events.
		std::map<int, FMOD::Studio::EventInstance*> m_AsyncEvents;
		/// @brief Map of FMOD sounds.
		SoundMap m_Sounds;
		/// @brief Map of FMOD channels.
		ChannelMap m_Channels;

	};
}


