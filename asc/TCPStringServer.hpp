# pragma once
# include <Siv3D.hpp>

namespace asc
{
	using namespace s3d;

	/// <summary>
	/// ������𑗎�M�\�� TCPServer
	/// </summary>
	/// <remarks>
	/// ����M�� UTF-8 �ōs���܂��B
	/// </remarks>
	class TCPStringServer : public TCPServer
	{
	private:

		std::string m_buffer;

	public:

		/// <summary>
		/// 1 �����ǂݍ��݂܂��B
		/// </summary>
		/// <param name="to">
		/// �ǂݍ��ݐ�
		/// </param>
		/// <returns>
		/// �ǂݍ��݂ɐ�������� true
		/// </returns>
		/// <remarks>
		/// ���{��Ȃǂ� 1 �o�C�g�ł͂Ȃ������͐����������܂���B
		/// </remarks>
		bool readChar(wchar& to)
		{
			if (m_buffer.length() > 0)
			{
				to = FromUTF8(m_buffer.substr(0, 1))[0];
				m_buffer = m_buffer.substr(1);
			}

			std::string buffer;

			if (!read(buffer[0]))
				return false;

			to = FromUTF8(std::move(buffer))[0];
			return true;
		}

		/// <summary>
		/// ���������w�肵�ĕ������ǂݍ��݂܂��B
		/// </summary>
		/// <param name="length">
		/// �ǂݍ��ޕ�����
		/// </param>
		/// <param name="to">
		/// �ǂݍ��ݐ�
		/// </param>
		/// <returns>
		/// �ǂݍ��݂ɐ�������� true
		/// </returns>
		/// <remarks>
		/// ���{��Ȃǂ� 1 �o�C�g�ł͂Ȃ������͐����������܂���B
		/// </remarks>
		bool readString(size_t length, String& to)
		{
			if (m_buffer.length() >= length)
			{
				to = FromUTF8(m_buffer.substr(0, length));
				m_buffer = m_buffer.substr(length);
				return true;
			}

			for (;;)
			{
				char character;

				if (!read(character))
					return false;

				m_buffer.push_back(character);

				if (m_buffer.length() >= length)
				{
					to = FromUTF8(m_buffer.substr(0, length));
					m_buffer = m_buffer.substr(length);
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// 1 �s�ǂݍ��݂܂��B
		/// </summary>
		/// <param name="to">
		/// �ǂݍ��ݐ�
		/// </param>
		/// <returns>
		/// �ǂݍ��݂ɐ�������� true
		/// </returns>
		/// <remarks>
		/// ���{��Ȃǂ� 1 �o�C�g�ł͂Ȃ������������܂��B
		/// </remarks>
		bool readLine(String& to)
		{
			return readUntil('\n', to);
		}

		/// <summary>
		/// �w�肵������������܂œǂݍ��݂܂��B
		/// </summary>
		/// <param name="end">
		/// �w�肷�镶��
		/// </param>
		/// <param name="to">
		/// �ǂݍ��ݐ�
		/// </param>
		/// <returns>
		/// �ǂݍ��݂ɐ�������� true
		/// </returns>
		/// <remarks>
		/// ���{��Ȃǂ� 1 �o�C�g�ł͂Ȃ������������܂��B
		/// </remarks>
		bool readUntil(char end, String& to)
		{
			const auto pos = m_buffer.find(end);

			if (pos != std::string::npos)
			{
				to = FromUTF8(m_buffer.substr(0, pos));
				m_buffer = m_buffer.substr(pos);
				return true;
			}

			for (;;)
			{
				char character;

				if (!read(character))
					return false;

				m_buffer.push_back(character);

				if (character == end)
					break;
			}

			to = FromUTF8(std::move(m_buffer));
			m_buffer.clear();
			return true;
		}

		/// <summary>
		/// �������\�Ȍ���ǂݍ��݂܂��B
		/// </summary>
		/// <param name="to">
		/// �ǂݍ��ݐ�
		/// </param>
		/// <returns>
		/// 1 �����ł��ǂݍ��݂ɐ�������� true
		/// </returns>
		/// <remarks>
		/// ���{��Ȃǂ� 1 �o�C�g�ł͂Ȃ������������܂��B
		/// </remarks>
		bool readAll(String& to)
		{
			for (;;)
			{
				char character;

				if (!read(character))
					break;

				m_buffer.push_back(character);
			}

			const auto success = m_buffer.empty();

			if (success)
			{
				to = FromUTF8(std::move(m_buffer));
				m_buffer.clear();
			}

			return success;
		}

		/// <summary>
		/// ������𑗐M���܂��B
		/// </summary>
		/// <param name="data">
		/// ���M���镶����
		/// </param>
		/// <returns>
		/// ���M�ɐ�������� true
		/// </returns>
		/// <remarks>
		/// ���{��Ȃǂ� 1 �o�C�g�ł͂Ȃ������������܂��B
		/// </remarks>
		bool sendString(const String& data)
		{
			const auto str = ToUTF8(data);
			return send(str.data(), sizeof(char) * str.length());
		}

		/// <summary>
		/// readUntil�֐��Ŏg�p����o�b�t�@�̒��g���폜���܂��B
		/// </summary>
		/// <returns>
		/// �폜�����o�b�t�@�̒��g
		/// </returns>
		String clearBuffer()
		{
			const auto buffer = FromUTF8(std::move(m_buffer));
			m_buffer.clear();
			return buffer;
		}
	};
}
