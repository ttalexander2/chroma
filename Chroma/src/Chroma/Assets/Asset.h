#pragma once

namespace Chroma
{
	class Asset
	{
	public:
		const std::string GetID() const { return m_Path; }
		virtual void Load();
	private:
		std::string m_Path;
	};
}