#pragma once

namespace Chroma
{
	class Asset
	{
	public:
		const std::string Name;
		const std::string GetID() const { return m_Path; }
		virtual void Load() = 0;
	private:
		std::string m_Path;
	};
}