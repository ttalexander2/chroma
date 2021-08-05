#include "chromapch.h"
#include "ChromaFMOD.h"
#include "Audio.h"
#include <set>

namespace Chroma
{
	/// @brief Constructs an FMOD Studio instance.
	ChromaFMOD::ChromaFMOD()
		: m_NextChanelID(0)
	{
		m_StudioSystem = nullptr;
		Audio::ErrorCheck(FMOD::Studio::System::create(&m_StudioSystem));
		Audio::ErrorCheck(m_StudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_STUDIO_INIT_NORMAL, NULL));

		m_System = nullptr;
		Audio::ErrorCheck(m_StudioSystem->getCoreSystem(&m_System));
	}

	/// @brief Destroys an FMOD Studio instance.
	ChromaFMOD::~ChromaFMOD()
	{
		Audio::ErrorCheck(m_StudioSystem->unloadAll());
		Audio::ErrorCheck(m_StudioSystem->release());
	}

	/// @brief Updates an FMOD Studio instance.
	///
	/// This should be called once a frame.
	void ChromaFMOD::Update()
	{
		std::vector<int> stoppedChannels;
		for (auto& [num, channel] : m_Channels)
		{
			bool isPlaying = false;
			channel->isPlaying(&isPlaying);
			if (!isPlaying)
			{
				stoppedChannels.push_back(num);
			}
		}

		for (int num : stoppedChannels)
		{
			m_Channels.erase(num);
		}

		Audio::ErrorCheck(m_StudioSystem->update());
	}
}