#pragma once


namespace Chroma
{

	class Time
	{
	public:
		Time(double time = 0.0f)
			: m_Time(time)
		{
			m_Instance = this;
		}

		static Time* Current() { return Time::m_Instance; }

		operator double() const { return m_Time; }

		double GetSeconds() const { return m_Time; }
		double GetMilliseconds() const { return m_Time * 1000.0; }


	private:
		double m_Time;
		static Time* m_Instance;
	};
}