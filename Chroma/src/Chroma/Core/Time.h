#pragma once


namespace Chroma
{
	class Time
	{
	public:
		Time(float time = 0.0f)
			: m_Time(time)
		{
			m_Instance = this;
		}

		static Time* Current() { return Time::m_Instance; }

		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }


	private:
		float m_Time;
		static Time* m_Instance;
	};
}