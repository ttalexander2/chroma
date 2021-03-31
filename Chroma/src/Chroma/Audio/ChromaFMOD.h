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
	struct ChromaFMOD
	{
		ChromaFMOD();
		~ChromaFMOD();


		void Update();

		FMOD::Studio::System* m_StudioSystem;
		FMOD::System* m_System;

		int m_NextChanelID;

		typedef std::map<std::string, FMOD::Sound*> SoundMap;
		typedef std::map<int, FMOD::Channel*> ChannelMap;
		typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
		typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

		BankMap m_Banks;
		EventMap m_Events;
		std::map<int, FMOD::Studio::EventInstance*> m_AsyncEvents;
		SoundMap m_Sounds;
		ChannelMap m_Channels;

	};
}


