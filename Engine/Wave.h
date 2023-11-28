#pragma once

namespace Nt {
	class Wave {
	private:
		struct _Header {
			struct _Chunk {
				Char ID[4];
				uInt Size = 0;
			};

			_Chunk Chunk;
			Char Format[4];

			_Chunk Subchunk;
			Short AudioFormat;
			Short NumChannels;
			Int SampleRate;
			Int ByteRate;
			Short BlockAlign;
			Short BitsPerSample;
		};

	public:
		Wave() noexcept = default;
		Wave(const String& filePath) {
			LoadFromFile(filePath);
		}
		Wave(const Wave& wave)
		{
		}
		~Wave() {
			Release();
		}

		void LoadFromFile(const String& filePath) {
			if (m_IsLoaded) {
				Log::Warning("File \"" + Nt::String(filePath) + "\" already loaded.");
				return;
			}

			m_FilePath = filePath;
			std::ifstream file(m_FilePath, std::ios::in | std::ios::binary);
			if (file.is_open()) {
				file.seekg(0, std::ifstream::_Seekend);
				const Int fileSize = file.tellg();
				file.seekg(0, std::ifstream::_Seekbeg);

				file.read(reinterpret_cast<Char*>(&m_Header), sizeof(_Header));

				if (memcmp(m_Header.Chunk.ID, "RIFF", 4) != 0)
					Raise("Header chunk not is RIFF");
				if (memcmp(m_Header.Format, "WAVE", 4) != 0)
					Raise("Header format not is WAVE");
				if (memcmp(m_Header.Subchunk.ID, "fmt ", 4) != 0)
					Raise("Header subchunk not is fmt");

				if (m_Header.BitsPerSample != 8 && m_Header.BitsPerSample != 16)
					Raise("Wrong number of bit per sample");
				
				_Header::_Chunk chunk;
				do {
					file.seekg(chunk.Size, std::ifstream::_Seekcur);
					file.read(reinterpret_cast<Char*>(&chunk), sizeof(chunk));
				} while (memcmp(chunk.ID, "data", 4) != 0);

				m_Size = chunk.Size;
				m_SampleSize = m_Header.BitsPerSample / 8;
				m_SampleCount = m_Size / (m_Header.NumChannels * m_Header.BitsPerSample / 8);

				m_pData = new Byte[m_Size];
				file.read(reinterpret_cast<Char*>(m_pData), m_Size);
			}
			else {
				Raise(String("Failed to open file: ") + m_FilePath);
			}
			m_IsLoaded = true;
		}

		void Release() {
			if (m_IsLoaded) {
				SAFE_DELETE(&m_pData);
				m_IsLoaded = false;
			}
		}

		void* GetData() const noexcept {
			return m_pData;
		}
		_Header GetHeader() const noexcept {
			return m_Header;
		}
		uInt GetSampleCount() const noexcept {
			return m_SampleCount;
		}
		uInt GetSize() const noexcept {
			return m_Size;
		}

	private:
		_Header m_Header;
		std::string m_FilePath;
		uInt m_SampleSize = 0;
		uInt m_SampleCount = 0;
		Byte* m_pData = nullptr;
		uInt m_Size;
		Bool m_IsLoaded;
	};
}