// DttReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Archive.h"
#include <fstream>
#include <vector>
#include <windows.h>
#include <shlwapi.h>

static unsigned __int32 CRCTablesSB8[] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

unsigned __int32 crc32(char * pData, unsigned __int32 nLength, unsigned __int32 nInit)
{
	unsigned __int32 CRC = nInit;
	CRC = ~CRC;
	/*for (int i = 0; i < nLength / 4; i++)
	{
		CRC = (CRC >> 8) ^ CRCTablesSB8[(CRC ^ pData[i * 4 + 0]) & 0xFF];
		CRC = (CRC >> 8) ^ CRCTablesSB8[(CRC ^ pData[i * 4 + 1]) & 0xFF];
		CRC = (CRC >> 8) ^ CRCTablesSB8[(CRC ^ pData[i * 4 + 2]) & 0xFF];
		CRC = (CRC >> 8) ^ CRCTablesSB8[(CRC ^ pData[i * 4 + 3]) & 0xFF];
	}*/
	for (int i = 0; i < nLength; i++)
		CRC = CRCTablesSB8[((__int32)CRC ^ pData[i]) & 0xFF] ^ (CRC >> 8);

	return ~CRC;
}

class FDTT
{
public:
	unsigned __int32 nSignature;
	unsigned __int32 nCount;
	unsigned __int32 nStartFileOffsetsOffset;
	unsigned __int32 nTypesOffset;
	unsigned __int32 nFileNamesOffset;
	unsigned __int32 nFileLengthsOffset;
	unsigned __int32 nOffset5;
	__int32 nNull;
	std::vector<unsigned __int32> tStartFileOffsets;
	std::vector<unsigned __int32> tTypes;
	std::vector<std::string> tFileNames;
	std::vector<unsigned __int32> tFileLengths;
	unsigned __int32 nSomething1;
	unsigned __int32 nSomethingArray1Offset;
	unsigned __int32 nCRCsOffset;
	unsigned __int32 nSomethingArray2Offset;
	std::shared_ptr<char> tSomethingArray1;
	std::vector<unsigned __int32> tCRCs;
	std::shared_ptr<char> tSomethingArray2;
	std::vector<std::vector<std::shared_ptr<char>>> tDATAs;
	std::vector<std::vector<unsigned __int32>> tDATALengths;


	void Shift(FArchive & ar, unsigned __int32 nPos)
	{
		if (ar.Position() > nPos)
			throw new std::exception("Bad read exception!");
		while (ar.Position() < nPos)
		{
			if (ar.IsReading())
			{
				if (ar.Read(1).get()[0] != 0)
					throw new std::exception("Bad read exception!");
			}
			else if (ar.IsWriting())
			{
				ar.Write(std::shared_ptr<char>(new char[1]{ 0 }).get(), 1);
			}
		}
	}

	void Serialize(FArchive & ar)
	{
		ar << nSignature;
		if (nSignature != 0x00544144) // DAT\0
			throw new std::exception("Bad signature found!");
		ar << nCount;
		ar << nStartFileOffsetsOffset;
		ar << nTypesOffset;
		ar << nFileNamesOffset;
		ar << nFileLengthsOffset;
		ar << nOffset5;
		ar << nNull;
		Shift(ar, nStartFileOffsetsOffset);
		for (unsigned __int32 i = 0; i < nCount; i++)
		{
			if (ar.IsReading())
				tStartFileOffsets.emplace_back();
			ar << tStartFileOffsets[i];
		}
		Shift(ar, nTypesOffset);
		for (unsigned __int32 i = 0; i < nCount; i++)
		{
			if (ar.IsReading())
				tTypes.emplace_back();
			ar << tTypes[i];
			if (tTypes[i] != 0x00707477) // wtp\0
				throw new std::exception("Only wtp type supported!");
		}
		Shift(ar, nFileNamesOffset);
		unsigned __int32 nFileNamesLength = 0;
		if (ar.IsWriting())
		{
			for (unsigned __int32 i = 0; i < tFileNames.size(); i++)
				if (tFileNames[i].length() > nFileNamesLength)
					nFileNamesLength = tFileNames[i].length();
			nFileNamesLength++;
		}
		ar << nFileNamesLength;
		for (unsigned __int32 i = 0; i < nCount; i++)
		{
			if (ar.IsReading())
			{
				tFileNames.emplace_back(ar.Read(nFileNamesLength).get());
			}
			else if (ar.IsWriting())
			{
				ar.Write(tFileNames[i].c_str(), tFileNames[i].length());
				for (unsigned __int32 j = tFileNames[i].length(); j < nFileNamesLength; j++)
					ar.Write(std::shared_ptr<char>(new char[1]{ 0 }).get(), 1);
			}
		}
		Shift(ar, nFileLengthsOffset);
		for (unsigned __int32 i = 0; i < nCount; i++)
		{
			if (ar.IsReading())
				tFileLengths.emplace_back();
			ar << tFileLengths[i];
		}
		Shift(ar, nOffset5);
		unsigned __int32 nCurrentPosition = ar.Position();
		ar << nSomething1;
		ar << nSomethingArray1Offset;
		ar << nCRCsOffset;
		ar << nSomethingArray2Offset;
		Shift(ar, nCurrentPosition + nSomethingArray1Offset);
		if (ar.IsReading())
			tSomethingArray1 = ar.Read(nCRCsOffset - nSomethingArray1Offset);
		else if (ar.IsWriting())
			ar.Write(tSomethingArray1.get(), nCRCsOffset - nSomethingArray1Offset);
		Shift(ar, nCurrentPosition + nCRCsOffset);
		for (unsigned __int32 i = 0; i < nCount; i++)
		{
			if (ar.IsReading())
				tCRCs.emplace_back();
			ar << tCRCs[i];
		}
		Shift(ar, nCurrentPosition + nSomethingArray2Offset);
		if (ar.IsReading())
			tSomethingArray2 = ar.Read(tStartFileOffsets[0] - nSomethingArray2Offset - nCurrentPosition);
		else if (ar.IsWriting())
			ar.Write(tSomethingArray2.get(), tStartFileOffsets[0] - nSomethingArray2Offset - nCurrentPosition);
		for (unsigned __int32 i = 0; i < nCount; i++)
		{
			Shift(ar, tStartFileOffsets[i]);
			if (ar.IsReading())
			{
				tDATAs.push_back(std::vector<std::shared_ptr<char>>());
				tDATALengths.push_back(std::vector<unsigned __int32>());
				unsigned __int32 nLength = tFileLengths[i];
				unsigned __int32 nBegin = ar.Position();
				unsigned __int32 nStart = 0;
				if (nLength >= 4096)
				{
					while (ar.Position() <= nBegin + nLength - 4096)
					{
						unsigned __int32 nSign = 0;
						ar << nSign;
						if (nSign == 0x20534444)
						{
							if (nStart > 0)
							{
								unsigned __int32 nCurPos = ar.Position();
								ar.Seek(nStart);
								tDATAs[i].push_back(ar.Read(nCurPos - 4 - nStart));
								tDATALengths[i].push_back(nCurPos - 4 - nStart);
								ar.Seek(nCurPos);
							}
							nStart = ar.Position() - 4;
						}
						ar.Seek(ar.Position() + 4096 - 4);
					}
				}
				if (nStart > 0)
				{
					ar.Seek(nStart);
					tDATAs[i].push_back(ar.Read(nBegin + nLength - nStart));
					tDATALengths[i].push_back(nBegin + nLength - nStart);
				}
				else
				{
					ar.Seek(nBegin);
					tDATAs[i].push_back(ar.Read(nLength));
					tDATALengths[i].push_back(nLength);
				}

				// test CRC
				//unsigned __int32 crc = crc32(tDATAs[i].get(), tFileLengths[i], 0/*nSomethingFlag*/);
				//crc = ~crc;
				//crc++;
			}
			else if (ar.IsWriting())
			{
				for (unsigned __int32 j = 0; j < tDATAs[i].size(); j++)
					ar.Write(tDATAs[i][j].get(), tDATALengths[i][j]);
			}
		}
	}
};

std::wstring StupidStringToWString(const std::string & s)
{
	std::wstring tmp(s.begin(), s.end());
	return tmp;
}

// -u "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures\subtitle_7030_us.dtt" "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures - unpacked"
// -u "C:\Users\vladi\Downloads\����� �����\dtt textures (1)\ui_core_pc.dtt" "C:\Users\vladi\Downloads\����� �����\dtt textures (1)"
// -u "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures" "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures - unpacked"
// -p "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures\subtitle_7030_us.dtt" "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures - unpacked\subtitle_7030_us.dtt"
// -p "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures\ui_hud_us.dtt" "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures - unpacked\ui_hud_us.dtt"
// -p "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures" "C:\Users\vladi\Downloads\����� �����\dtt textures\.dtt textures - unpacked"

// -u "D:\Downloads\ui_loading_us\ui_loading_us\RU_HEX_ui_title_us.dtt" "D:\Downloads\ui_loading_us\ui_loading_us\test"
// -p "D:\Downloads\ui_loading_us\ui_loading_us\RU_HEX_ui_title_us.dtt" "D:\Downloads\ui_loading_us\ui_loading_us\test"
// -u "D:\Downloads\ui_loading_us\ui_loading_us" "D:\Downloads\ui_loading_us\ui_loading_us\test"
// -p "D:\Downloads\ui_loading_us\ui_loading_us" "D:\Downloads\ui_loading_us\ui_loading_us\test"

// -u "D:\Downloads\ui_loading_us\ui_loading_us\TRANSFORMERS Devastation ddt\put_dtt" "D:\Downloads\ui_loading_us\ui_loading_us\TRANSFORMERS Devastation ddt\dtt_unpacked"
// -p "D:\Downloads\ui_loading_us\ui_loading_us\TRANSFORMERS Devastation ddt\put_dtt" "D:\Downloads\ui_loading_us\ui_loading_us\TRANSFORMERS Devastation ddt\dtt_unpacked"
int wmain(int argc, wchar_t ** argv)
{
	try
	{
		if (argc < 4)
		{
			wprintf_s(L"unpack:\r\n");
			wprintf_s(L"-u <dtt_file> <folder_for_unpacked_files>\r\n");
			wprintf_s(L"-u <folder_with_dtt_files> <folder_for_unpacked_files>\r\n");
			wprintf_s(L"pack:\r\n");
			wprintf_s(L"-p <ddt_file> <folder_for_unpacked_files>\r\n");
			wprintf_s(L"-p <folder_with_dtt_files> <folder_for_unpacked_files>\r\n");
			wprintf_s(L"\r\n");
			wprintf_s(L"note: path without trailing slashes!\r\n");
			return 0;
		}

		if (std::wstring(argv[1]) == L"-u")
		{
			std::wstring in(argv[2]);
			std::wstring out(argv[3]);

			DWORD dwAttrib = GetFileAttributes(in.c_str());
			if (dwAttrib == INVALID_FILE_ATTRIBUTES)
			{
				printf_s("Can't get file attributes! Error code: %d.\r\n", GetLastError());
				throw new std::exception("Can't get file attributes!");
			}
			if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
			{
				// directory

				std::wstring search_path(in);
				search_path += L"\\*.dtt";

				std::vector<std::wstring> m_tFiles;
				HANDLE hFind = INVALID_HANDLE_VALUE;
				WIN32_FIND_DATA ffd;
				hFind = FindFirstFile(search_path.c_str(), &ffd);
				if (hFind == INVALID_HANDLE_VALUE)
				{
					printf_s("Can't scan path! Error code: %d.\r\n", GetLastError());
					throw new std::exception("Can't scan path!");
				}
				do
				{
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						continue;
					m_tFiles.push_back(ffd.cFileName);
				} while (FindNextFile(hFind, &ffd) != 0);

				for (unsigned __int32 i = 0; i < m_tFiles.size(); i++)
				{
					std::fstream f(in + L"\\" + m_tFiles[i], std::ios::in | std::ios::binary);
					FArchive ar(&f, FArchive::Type::Read);

					FDTT file;
					file.Serialize(ar);

					f.close();

					std::wstring out_path(out + L"\\" + m_tFiles[i]);

					for (unsigned __int32 j = 0; j < file.tDATAs.size(); j++)
					{
						_wmkdir(out_path.c_str());
						for (unsigned __int32 k = 0; k < file.tDATAs[j].size(); k++)
						{
							wchar_t index[10];
							swprintf_s(index, L"%d", k + 1);
							std::fstream f_out(out_path + +L"\\" + StupidStringToWString(file.tFileNames[j]) + L"." + index + L".dds", std::ios::out | std::ios::binary);
							f_out.write(file.tDATAs[j][k].get(), file.tDATALengths[j][k]);
							f_out.close();
						}
					}
				}
			}
			else
			{
				// file

				std::fstream f(in, std::ios::in | std::ios::binary);
				FArchive ar(&f, FArchive::Type::Read);

				FDTT file;
				file.Serialize(ar);

				f.close();

				for (unsigned __int32 j = 0; j < file.tDATAs.size(); j++)
				{
					_wmkdir(out.c_str());
					for (unsigned __int32 k = 0; k < file.tDATAs[j].size(); k++)
					{
						wchar_t index[10];
						swprintf_s(index, L"%d", k + 1);
						std::fstream f_out(out + +L"\\" + StupidStringToWString(file.tFileNames[j]) + L"." + index + L".dds", std::ios::out | std::ios::binary);
						f_out.write(file.tDATAs[j][k].get(), file.tDATALengths[j][k]);
						f_out.close();
					}
				}
			}

			return 0;
		}

		if (std::wstring(argv[1]) == L"-p")
		{
			std::wstring in(argv[2]);
			std::wstring out(argv[3]);

			DWORD dwAttrib = GetFileAttributes(in.c_str());
			if (dwAttrib == INVALID_FILE_ATTRIBUTES)
			{
				printf_s("Can't get file attributes! Error code: %d.\r\n", GetLastError());
				throw new std::exception("Can't get file attributes!");
			}
			if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
			{
				// directory

				std::wstring search_path(in);
				search_path += L"\\*.dtt";

				std::vector<std::wstring> m_tFiles;
				HANDLE hFind = INVALID_HANDLE_VALUE;
				WIN32_FIND_DATA ffd;
				hFind = FindFirstFile(search_path.c_str(), &ffd);
				if (hFind == INVALID_HANDLE_VALUE)
				{
					printf_s("Can't scan path! Error code: %d.\r\n", GetLastError());
					throw new std::exception("Can't scan path!");
				}
				do
				{
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						continue;
					m_tFiles.push_back(ffd.cFileName);
				} while (FindNextFile(hFind, &ffd) != 0);

				for (unsigned __int32 i = 0; i < m_tFiles.size(); i++)
				{
					std::fstream f(in + L"\\" + m_tFiles[i], std::ios::in | std::ios::binary);
					FArchive ar(&f, FArchive::Type::Read);

					FDTT file;
					file.Serialize(ar);

					f.close();

					unsigned __int64 nStartOffset = file.tStartFileOffsets[0];

					std::wstring out_path(out + L"\\" + m_tFiles[i]);

					for (unsigned __int32 j = 0; j < file.tDATAs.size(); j++)
					{
						file.tStartFileOffsets[j] = nStartOffset;
						for (unsigned __int32 k = 0; k < file.tDATAs[j].size(); k++)
						{
							wchar_t index[10];
							swprintf_s(index, L"%d", k + 1);
							std::wstring file_path = out_path + L"\\" + StupidStringToWString(file.tFileNames[j]) + L"." + index + L".dds";
							DWORD dwAttrib2 = GetFileAttributes(file_path.c_str());
							if (dwAttrib2 == INVALID_FILE_ATTRIBUTES || dwAttrib2 & FILE_ATTRIBUTE_DIRECTORY)
								continue;
							std::ifstream f3(file_path, std::ios::in | std::ios::binary);
							f3.seekg(0, f3.end);
							unsigned __int64 nRealLength = f3.tellg();
							f3.seekg(0, f3.beg);
							if (nRealLength < 32)
								throw new std::exception("Wrong DDS file!");
							unsigned __int32 nMimMapCount = 0;
							f3.seekg(28, f3.beg);
							f3.read((char *)&nMimMapCount, 4);
							if (nMimMapCount != *(int *)(file.tDATAs[j][k].get() + 28))
							{
								nMimMapCount = *(int *)(file.tDATAs[j][k].get() + 28);
								if (nMimMapCount == 0)
									nMimMapCount = 1;
								f3.close();
								wchar_t mms[10];
								swprintf_s(mms, L"%d", nMimMapCount);
								std::wstring sParam(L"-m " + std::wstring(mms) + L" -o \"" + out_path + L"\" \"" + file_path + L"\"");
								SHELLEXECUTEINFO info = {};
								info.cbSize = sizeof(SHELLEXECUTEINFO);
								info.fMask = SEE_MASK_NOCLOSEPROCESS;
								info.hwnd = NULL;
								info.lpVerb = NULL;
								info.lpFile = L"texconv.exe";
								info.lpParameters = sParam.c_str();
								wchar_t sPath[MAX_PATH];
								GetModuleFileName(NULL, sPath, MAX_PATH);
								PathRemoveFileSpec(sPath);
								info.lpDirectory = sPath;
								info.nShow = SW_SHOW;
								info.hInstApp = NULL;
								if (!ShellExecuteEx(&info))
									throw new std::exception("texconv.exe not found!");
								WaitForSingleObject(info.hProcess, INFINITE);
								f3.open(file_path, std::ios::in | std::ios::binary);
							}
							f3.seekg(0, f3.beg);
							unsigned __int64 nAlignedLength = nRealLength;
							if (k + 1 < file.tDATAs[j].size())
								nAlignedLength = nRealLength + (nRealLength % 4096 > 0 ? 4096 - (nRealLength % 4096) : 0);
							file.tDATAs[j][k] = std::shared_ptr<char>(new char[nAlignedLength]);
							memset(file.tDATAs[j][k].get(), 0, nAlignedLength);
							f3.read(file.tDATAs[j][k].get(), nRealLength);
							f3.close();
							file.tDATALengths[j][k] = nAlignedLength;
						}
						unsigned __int64 nLength = 0;
						for (unsigned __int32 k = 0; k < file.tDATAs[j].size(); k++)
							nLength += file.tDATALengths[j][k];
						file.tFileLengths[j] = nLength;
						nStartOffset += nLength;
					}

					std::fstream f2(in + L"\\" + m_tFiles[i], std::ios::out | std::ios::binary);
					FArchive ar2(&f2, FArchive::Type::Write);
					file.Serialize(ar2);
					f2.close();
				}
			}
			else
			{
				// file

				std::fstream f(in, std::ios::in | std::ios::binary);
				FArchive ar(&f, FArchive::Type::Read);

				FDTT file;
				file.Serialize(ar);

				f.close();

				unsigned __int64 nStartOffset = file.tStartFileOffsets[0];

				for (unsigned __int32 j = 0; j < file.tDATAs.size(); j++)
				{
					file.tStartFileOffsets[j] = nStartOffset;
					for (unsigned __int32 k = 0; k < file.tDATAs[j].size(); k++)
					{
						wchar_t index[10];
						swprintf_s(index, L"%d", k + 1);
						std::wstring file_path = out + L"\\" + StupidStringToWString(file.tFileNames[j]) + L"." + index + L".dds";
						DWORD dwAttrib2 = GetFileAttributes(file_path.c_str());
						if (dwAttrib2 == INVALID_FILE_ATTRIBUTES || dwAttrib2 & FILE_ATTRIBUTE_DIRECTORY)
							continue;
						std::ifstream f3(file_path, std::ios::in | std::ios::binary);
						f3.seekg(0, f3.end);
						unsigned __int64 nRealLength = f3.tellg();
						f3.seekg(0, f3.beg);
						unsigned __int64 nAlignedLength = nRealLength;
						if (k + 1 < file.tDATAs[j].size())
							nAlignedLength = nRealLength + (nRealLength % 4096 > 0 ? 4096 - (nRealLength % 4096) : 0);
						file.tDATAs[j][k] = std::shared_ptr<char>(new char[nAlignedLength]);
						memset(file.tDATAs[j][k].get(), 0, nAlignedLength);
						f3.read(file.tDATAs[j][k].get(), nRealLength);
						f3.close();
						file.tDATALengths[j][k] = nAlignedLength;
					}
					unsigned __int64 nLength = 0;
					for (unsigned __int32 k = 0; k < file.tDATAs[j].size(); k++)
						nLength += file.tDATALengths[j][k];
					file.tFileLengths[j] = nLength;
					nStartOffset += nLength;
				}

				std::fstream f2(in, std::ios::out | std::ios::binary);
				FArchive ar2(&f2, FArchive::Type::Write);
				file.Serialize(ar2);
				f2.close();
			}
		}
	}
	catch (const std::exception & e)
	{
		printf_s(e.what());
		throw;
	}
	catch (...)
	{
		printf_s("Everything is very bad!");
		throw;
	}
    return 0;
}

