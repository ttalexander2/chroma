#pragma once


namespace Chroma
{
	/// @brief Time object, for keeping track of the current time.
	///
	/// TODO: I'd prefer a static implementation of this, rather than propogated via update functions.
	///		  For example: Time::Delta could return delta time in seconds.
	class Time
	{
	public:
		/// @brief Constructs a new Time Object
		/// @param time Time in seconds.
		Time(double time = 0.0f) :
			m_Time(time)
		{
			m_Instance = this;
		}

		/// @brief Get the current time.
		/// @return Pointer to the current time.
		static Time *Current() { return m_Instance; }

		/// @brief Converts the time to a double (seconds).
		operator double() const { return m_Time; }

		/// @brief Gets the time in seconds.
		/// @return Double (seconds)
		double GetSeconds() const { return m_Time; }

		/// @brief Gets time in milliseconds.
		/// @return Double (milliseconds).
		double GetMilliseconds() const { return m_Time * 1000.0; }


	private:
		/// @brief Time in seconds
		double m_Time;

		/// @brief Current time.
		static Time *m_Instance;
	};
}
