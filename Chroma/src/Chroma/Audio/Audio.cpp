#include "chromapch.h"
#include "Audio.h"

#include "Chroma/Audio/ChromaFMOD.h"

namespace Chroma
{

	static ChromaFMOD* s_FMOD;

	void Audio::Init()
	{
		s_FMOD = new ChromaFMOD();
	}

	void Chroma::Audio::Update()
	{
		s_FMOD->Update();
	}

	void Audio::Shutdown()
	{
		delete s_FMOD;
	}

	void Audio::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto found = s_FMOD->m_Banks.find(bankName);
		if (found != s_FMOD->m_Banks.end())
			return;

		FMOD::Studio::Bank* bank;
		Audio::ErrorCheck(s_FMOD->m_StudioSystem->loadBankFile(bankName.c_str(), flags, &bank));
		if (bank)
			s_FMOD->m_Banks[bankName] = bank;
	}

	void Audio::LoadEvent(const std::string& eventName)
	{

		auto found = s_FMOD->m_Events.find(eventName);
		if (found != s_FMOD->m_Events.end())
			return;

		FMOD::Studio::EventDescription* eventDesc = nullptr;
		Audio::ErrorCheck(s_FMOD->m_StudioSystem->getEvent(eventName.c_str(), &eventDesc));
		if (eventDesc)
		{
			FMOD::Studio::EventInstance* eventInstance = nullptr;
			Audio::ErrorCheck(eventDesc->createInstance(&eventInstance));
			if (eventInstance)
			{
				s_FMOD->m_Events[eventName] = eventInstance;
			}
		}
	}


	void Audio::Set3dListenerAndOrientation(const Math::vec3& position, float volume_dB)
	{

	}

	void Audio::PlayEvent(const std::string& eventName)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
		{
			LoadEvent(eventName);
			found = s_FMOD->m_Events.find(eventName);
			if (found == s_FMOD->m_Events.end())
				return;
		}
		Audio::ErrorCheck(found->second->start());
	}


	void Audio::PlayEventIfStopped(const std::string& eventName)
	{
		if (!Audio::IsEventStopped(eventName))
			return;
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
		{
			LoadEvent(eventName);
			found = s_FMOD->m_Events.find(eventName);
			if (found == s_FMOD->m_Events.end())
				return;
		}
		Audio::ErrorCheck(found->second->start());
	}

	void Audio::StopChannel(int channelId)
	{
		auto found = s_FMOD->m_Channels.find(channelId);
		if (found == s_FMOD->m_Channels.end())
			return;

		Audio::ErrorCheck(found->second->stop());
	}

	void Audio::StopEvent(const std::string& eventName, bool immediate)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
			return;

		FMOD_STUDIO_STOP_MODE mode;
		mode = immediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
		Audio::ErrorCheck(found->second->stop(mode));
	}

	void Audio::GetEventParameter(const std::string& eventName, const std::string& parameter, float* value)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
			return;

		Audio::ErrorCheck(found->second->getParameterByName(parameter.c_str(), value));
	}

	void Audio::GetEventParameter(const std::string& eventName, FMOD_STUDIO_PARAMETER_ID parameter, float* value)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
			return;

		Audio::ErrorCheck(found->second->getParameterByID(parameter, value));
	}

	void Audio::SetEventParameter(const std::string& eventName, const std::string& parameter, float value, bool ignoreSeekSpeed)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
			return;

		Audio::ErrorCheck(found->second->setParameterByName(parameter.c_str(), value, ignoreSeekSpeed));
	}

	void Audio::SetEventParameter(const std::string& eventName, FMOD_STUDIO_PARAMETER_ID parameter, float value, bool ignoreSeekSpeed)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
			return;

		Audio::ErrorCheck(found->second->setParameterByID(parameter, value, ignoreSeekSpeed));
	}

	void Audio::StopAllChannels()
	{
		for (auto& [id, channel] : s_FMOD->m_Channels)
		{
			channel->stop();
		}
	}

	void Audio::SetChannel3dPosition(int channelId, const Math::vec3& position)
	{
		auto found = s_FMOD->m_Channels.find(channelId);
		if (found == s_FMOD->m_Channels.end())
			return;

		FMOD_VECTOR fmod_position = VectorToFmod(position);
		Audio::ErrorCheck(found->second->set3DAttributes(&fmod_position, nullptr));
	}

	void Audio::SetChannelVolume(int channelId, float volume_db)
	{
		auto found = s_FMOD->m_Channels.find(channelId);
		if (found == s_FMOD->m_Channels.end())
			return;

		Audio::ErrorCheck(found->second->setVolume(dbToVolume(volume_db)));
	}

	bool Audio::IsPlaying(int channelId)
	{
		auto found = s_FMOD->m_Channels.find(channelId);
		if (found == s_FMOD->m_Channels.end())
			return false;

		bool isPlaying;
		Audio::ErrorCheck(found->second->isPlaying(&isPlaying));
		return isPlaying;
	}

	bool Audio::IsEventPlaying(const std::string& eventName)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
			return false;

		FMOD_STUDIO_PLAYBACK_STATE state;
		Audio::ErrorCheck(found->second->getPlaybackState(&state));

		if (state == FMOD_STUDIO_PLAYBACK_PLAYING)
		{
			return true;
		}
		return false;
	}

	bool Audio::IsEventStopped(const std::string& eventName)
	{
		auto found = s_FMOD->m_Events.find(eventName);
		if (found == s_FMOD->m_Events.end())
			return true;

		FMOD_STUDIO_PLAYBACK_STATE state;
		Audio::ErrorCheck(found->second->getPlaybackState(&state));

		if (state != FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			return false;
		}
		return true;
	}

	float Audio::dbToVolume(float db)
	{
		return Math::pow(10.0f, 0.05f * db);
	}

	float Audio::volumeTodb(float volume)
	{
		return 20.0f * (Math::log(volume) / Math::log(10));
	}

	FMOD_VECTOR Audio::VectorToFmod(const Math::vec3& vector)
	{
		FMOD_VECTOR fVec;
		fVec.x = vector.x;
		fVec.y = vector.y;
		fVec.z = vector.z;
		return fVec;

	}

	void Audio::ErrorCheck(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			switch (result)
			{
				case FMOD_ERR_BADCOMMAND: 					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_BADCOMMAND"); break;
				case FMOD_ERR_CHANNEL_ALLOC:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_CHANNEL_ALLOC"); break;
				case FMOD_ERR_CHANNEL_STOLEN:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_CHANNEL_STOLEN"); break;
				case FMOD_ERR_DMA:							CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DMA"); break;
				case FMOD_ERR_DSP_CONNECTION:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_CONNECTION"); break;
				case FMOD_ERR_DSP_DONTPROCESS:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_DONTPROCESS"); break;
				case FMOD_ERR_DSP_FORMAT:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_FORMAT"); break;
				case FMOD_ERR_DSP_INUSE:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_INUSE"); break;
				case FMOD_ERR_DSP_NOTFOUND:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_NOTFOUND"); break;
				case FMOD_ERR_DSP_RESERVED:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_RESERVED"); break;
				case FMOD_ERR_DSP_SILENCE:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_SILENCE"); break;
				case FMOD_ERR_DSP_TYPE:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_DSP_TYPE"); break;
				case FMOD_ERR_FILE_BAD:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_FILE_BAD"); break;
				case FMOD_ERR_FILE_COULDNOTSEEK:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_FILE_COULDNOTSEEK"); break;
				case FMOD_ERR_FILE_DISKEJECTED:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_FILE_DISKEJECTED"); break;
				case FMOD_ERR_FILE_EOF:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_FILE_EOF"); break;
				case FMOD_ERR_FILE_ENDOFDATA:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_FILE_ENDOFDATA"); break;
				case FMOD_ERR_FILE_NOTFOUND:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_FILE_NOTFOUND"); break;
				case FMOD_ERR_FORMAT:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_FORMAT"); break;
				case FMOD_ERR_HEADER_MISMATCH:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_HEADER_MISMATCH"); break;
				case FMOD_ERR_HTTP:							CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_HTTP"); break;
				case FMOD_ERR_HTTP_ACCESS:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_HTTP_ACCESS"); break;
				case FMOD_ERR_HTTP_PROXY_AUTH:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_HTTP_PROXY_AUTH"); break;
				case FMOD_ERR_HTTP_SERVER_ERROR:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_HTTP_SERVER_ERROR"); break;
				case FMOD_ERR_HTTP_TIMEOUT:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_HTTP_TIMEOUT"); break;
				case FMOD_ERR_INITIALIZATION:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INITIALIZATION"); break;
				case FMOD_ERR_INITIALIZED:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INITIALIZED"); break;
				case FMOD_ERR_INTERNAL:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INTERNAL"); break;
				case FMOD_ERR_INVALID_FLOAT:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_FLOAT"); break;
				case FMOD_ERR_INVALID_HANDLE:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_HANDLE"); break;
				case FMOD_ERR_INVALID_PARAM:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_PARAM"); break;
				case FMOD_ERR_INVALID_POSITION:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_POSITION"); break;
				case FMOD_ERR_INVALID_SPEAKER:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_SPEAKER"); break;
				case FMOD_ERR_INVALID_SYNCPOINT:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_SYNCPOINT"); break;
				case FMOD_ERR_INVALID_THREAD:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_THREAD"); break;
				case FMOD_ERR_INVALID_VECTOR:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_VECTOR"); break;
				case FMOD_ERR_MAXAUDIBLE:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_MAXAUDIBLE"); break;
				case FMOD_ERR_MEMORY:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_MEMORY"); break;
				case FMOD_ERR_MEMORY_CANTPOINT:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_MEMORY_CANTPOINT"); break;
				case FMOD_ERR_NEEDS3D:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NEEDS3D"); break;
				case FMOD_ERR_NEEDSHARDWARE:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NEEDSHARDWARE"); break;
				case FMOD_ERR_NET_CONNECT:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NET_CONNECT"); break;
				case FMOD_ERR_NET_SOCKET_ERROR:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NET_SOCKET_ERROR"); break;
				case FMOD_ERR_NET_URL:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NET_URL"); break;
				case FMOD_ERR_NET_WOULD_BLOCK:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NET_WOULD_BLOCK"); break;
				case FMOD_ERR_NOTREADY:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NOTREADY"); break;
				case FMOD_ERR_OUTPUT_ALLOCATED:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_OUTPUT_ALLOCATED"); break;
				case FMOD_ERR_OUTPUT_CREATEBUFFER:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_OUTPUT_CREATEBUFFER"); break;
				case FMOD_ERR_OUTPUT_DRIVERCALL:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_OUTPUT_DRIVERCALL"); break;
				case FMOD_ERR_OUTPUT_FORMAT:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_OUTPUT_FORMAT"); break;
				case FMOD_ERR_OUTPUT_INIT:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_OUTPUT_INIT"); break;
				case FMOD_ERR_OUTPUT_NODRIVERS:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_OUTPUT_NODRIVERS"); break;
				case FMOD_ERR_PLUGIN:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_PLUGIN"); break;
				case FMOD_ERR_PLUGIN_MISSING:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_PLUGIN_MISSING"); break;
				case FMOD_ERR_PLUGIN_RESOURCE:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_PLUGIN_RESOURCE"); break;
				case FMOD_ERR_PLUGIN_VERSION:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_PLUGIN_VERSION"); break;
				case FMOD_ERR_RECORD:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_RECORD"); break;
				case FMOD_ERR_REVERB_CHANNELGROUP:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_REVERB_CHANNELGROUP"); break;
				case FMOD_ERR_REVERB_INSTANCE:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_REVERB_INSTANCE"); break;
				case FMOD_ERR_SUBSOUNDS:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_SUBSOUNDS"); break;
				case FMOD_ERR_SUBSOUND_ALLOCATED:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_SUBSOUND_ALLOCATED"); break;
				case FMOD_ERR_SUBSOUND_CANTMOVE:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_SUBSOUND_CANTMOVE"); break;
				case FMOD_ERR_TAGNOTFOUND:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_TAGNOTFOUND"); break;
				case FMOD_ERR_TOOMANYCHANNELS:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_TOOMANYCHANNELS"); break;
				case FMOD_ERR_TRUNCATED:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_TRUNCATED"); break;
				case FMOD_ERR_UNIMPLEMENTED:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_UNIMPLEMENTED"); break;
				case FMOD_ERR_UNINITIALIZED:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_UNINITIALIZED"); break;
				case FMOD_ERR_UNSUPPORTED:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_UNSUPPORTED"); break;
				case FMOD_ERR_VERSION:						CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_VERSION"); break;
				case FMOD_ERR_EVENT_ALREADY_LOADED:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_EVENT_ALREADY_LOADED"); break;
				case FMOD_ERR_EVENT_LIVEUPDATE_BUSY:		CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_EVENT_LIVEUPDATE_BUSY"); break;
				case FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH:	CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH"); break;
				case FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT:		CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT"); break;
				case FMOD_ERR_EVENT_NOTFOUND:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_EVENT_NOTFOUND"); break;
				case FMOD_ERR_STUDIO_UNINITIALIZED:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_STUDIO_UNINITIALIZED"); break;
				case FMOD_ERR_STUDIO_NOT_LOADED:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_STUDIO_NOT_LOADED"); break;
				case FMOD_ERR_INVALID_STRING:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_INVALID_STRING"); break;
				case FMOD_ERR_ALREADY_LOCKED:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_ALREADY_LOCKED"); break;
				case FMOD_ERR_NOT_LOCKED:					CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_NOT_LOCKED"); break;
				case FMOD_ERR_RECORD_DISCONNECTED:			CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_RECORD_DISCONNECTED"); break;
				case FMOD_ERR_TOOMANYSAMPLES:				CHROMA_CORE_ERROR("FMOD Error: FMOD_ERR_TOOMANYSAMPLES"); break;
				default:									CHROMA_CORE_ERROR("FMOD Error: Unknown"); break;
			}
		}
	}

}