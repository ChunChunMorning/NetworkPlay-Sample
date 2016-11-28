# pragma once
# include <Siv3D.hpp>

namespace asc
{
	using namespace s3d;

	/// <summary>
	/// 文字列を送受信可能な TCPServer
	/// </summary>
	/// <remarks>
	/// 送受信は UTF-8 で行います。
	/// </remarks>
	class TCPStringServer : public TCPServer
	{
	private:

		std::string m_buffer;

	public:

		/// <summary>
		/// 1 文字読み込みます。
		/// </summary>
		/// <param name="to">
		/// 読み込み先
		/// </param>
		/// <returns>
		/// 読み込みに成功すれば true
		/// </returns>
		/// <remarks>
		/// 日本語などの 1 バイトではない文字は正しく扱えません。
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
		/// 文字数を指定して文字列を読み込みます。
		/// </summary>
		/// <param name="length">
		/// 読み込む文字数
		/// </param>
		/// <param name="to">
		/// 読み込み先
		/// </param>
		/// <returns>
		/// 読み込みに成功すれば true
		/// </returns>
		/// <remarks>
		/// 日本語などの 1 バイトではない文字は正しく扱えません。
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
		/// 1 行読み込みます。
		/// </summary>
		/// <param name="to">
		/// 読み込み先
		/// </param>
		/// <returns>
		/// 読み込みに成功すれば true
		/// </returns>
		/// <remarks>
		/// 日本語などの 1 バイトではない文字も扱えます。
		/// </remarks>
		bool readLine(String& to)
		{
			return readUntil('\n', to);
		}

		/// <summary>
		/// 指定した文字が来るまで読み込みます。
		/// </summary>
		/// <param name="end">
		/// 指定する文字
		/// </param>
		/// <param name="to">
		/// 読み込み先
		/// </param>
		/// <returns>
		/// 読み込みに成功すれば true
		/// </returns>
		/// <remarks>
		/// 日本語などの 1 バイトではない文字も扱えます。
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
		/// 文字を可能な限り読み込みます。
		/// </summary>
		/// <param name="to">
		/// 読み込み先
		/// </param>
		/// <returns>
		/// 1 文字でも読み込みに成功すれば true
		/// </returns>
		/// <remarks>
		/// 日本語などの 1 バイトではない文字も扱えます。
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
		/// 文字列を送信します。
		/// </summary>
		/// <param name="data">
		/// 送信する文字列
		/// </param>
		/// <returns>
		/// 送信に成功すれば true
		/// </returns>
		/// <remarks>
		/// 日本語などの 1 バイトではない文字も扱えます。
		/// </remarks>
		bool sendString(const String& data)
		{
			const auto str = ToUTF8(data);
			return send(str.data(), sizeof(char) * str.length());
		}

		/// <summary>
		/// readUntil関数で使用するバッファの中身を削除します。
		/// </summary>
		/// <returns>
		/// 削除したバッファの中身
		/// </returns>
		String clearBuffer()
		{
			const auto buffer = FromUTF8(std::move(m_buffer));
			m_buffer.clear();
			return buffer;
		}
	};
}
