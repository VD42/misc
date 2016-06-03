// GxtReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <memory>
#include "Archive.h"
#include <windows.h>
#include <fstream>
#include "Structs.h"

void Header::Correction(const File & file)
{
	nCount = file.data.size();
	nSize = (3 + nCount) * 4;
	nTotalSize = nSize;
	unsigned __int32 nCurrentOffset = 0;
	for (unsigned __int32 i = 0; i < nCount; i++)
	{
		nTotalSize += file.length[i];
		if (tOffsets.size() <= i)
			tOffsets.push_back(0);
		tOffsets[i] = nCurrentOffset;
		nCurrentOffset += file.length[i];
	}
}

int wmain(int argc, wchar_t ** argv)
{
	try
	{
		if (argc < 4)
		{
			wprintf_s(L"unpack:\r\n");
			wprintf_s(L"-u <gxt_file> <folder_for_unpacked_files>\r\n");
			wprintf_s(L"-u <folder_with_gxt_files> <folder_for_unpacked_files>\r\n");
			wprintf_s(L"pack:\r\n");
			wprintf_s(L"-p <gxt_file> <folder_for_unpacked_files>\r\n");
			wprintf_s(L"-p <folder_with_gxt_files> <folder_for_unpacked_files>\r\n");
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

				std::vector<std::wstring> m_tFiles;
				{
					std::wstring search_path(in);
					search_path += L"\\*.gxt";
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
				}
				{
					std::wstring search_path(in);
					search_path += L"\\*.lds";
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
				}

				for (unsigned __int32 i = 0; i < m_tFiles.size(); i++)
				{
					std::fstream f(in + L"\\" + m_tFiles[i], std::ios::in | std::ios::binary);
					FArchive ar(&f, FArchive::Type::Read);

					File file;
					file.Serialize(ar);

					f.close();

					std::wstring out_path(out + L"\\" + m_tFiles[i]);
					_wmkdir(out_path.c_str());

					for (unsigned __int32 j = 0; j < file.data.size(); j++)
					{
						wchar_t index[10];
						swprintf_s(index, L"%d", j + 1);
						std::fstream f_out(out_path + L"\\" + index + L".dds", std::ios::out | std::ios::binary);
						f_out.write(file.data[j].get(), file.length[j]);
						f_out.close();
					}
				}
			}
			else
			{
				// file

				std::fstream f(in, std::ios::in | std::ios::binary);
				FArchive ar(&f, FArchive::Type::Read);

				File file;
				file.Serialize(ar);

				f.close();

				for (unsigned __int32 j = 0; j < file.data.size(); j++)
				{
					wchar_t index[10];
					swprintf_s(index, L"%d", j + 1);
					std::fstream f_out(out + L"\\" + index + L".dds", std::ios::out | std::ios::binary);
					f_out.write(file.data[j].get(), file.length[j]);
					f_out.close();
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

				std::vector<std::wstring> m_tFiles;

				{
					std::wstring search_path(in);
					search_path += L"\\*.gxt";
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
				}
				{
					std::wstring search_path(in);
					search_path += L"\\*.lds";
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
				}

				for (unsigned __int32 i = 0; i < m_tFiles.size(); i++)
				{
					std::fstream f(in + L"\\" + m_tFiles[i], std::ios::in | std::ios::binary);
					FArchive ar(&f, FArchive::Type::Read);

					File file;
					file.Serialize(ar);

					f.close();

					std::wstring out_path(out + L"\\" + m_tFiles[i]);

					for (unsigned __int32 j = 0; j < file.data.size(); j++)
					{
						wchar_t index[10];
						swprintf_s(index, L"%d", j + 1);
						std::wstring file_path = out_path + L"\\" + index + L".dds";
						DWORD dwAttrib2 = GetFileAttributes(file_path.c_str());
						if (dwAttrib2 == INVALID_FILE_ATTRIBUTES || dwAttrib2 & FILE_ATTRIBUTE_DIRECTORY)
							continue;
						std::ifstream f3(file_path, std::ios::in | std::ios::binary);
						f3.seekg(0, f3.end);
						unsigned __int64 nRealLength = f3.tellg();
						f3.seekg(0, f3.beg);
						unsigned __int64 nAlignedLength = nRealLength;
						file.data[j] = std::shared_ptr<char>(new char[nAlignedLength]);
						memset(file.data[j].get(), 0, nAlignedLength);
						f3.read(file.data[j].get(), nRealLength);
						f3.close();
						file.length[j] = nAlignedLength;
					}

					file.header.Correction(file);

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

				File file;
				file.Serialize(ar);

				f.close();

				for (unsigned __int32 j = 0; j < file.data.size(); j++)
				{
					wchar_t index[10];
					swprintf_s(index, L"%d", j + 1);
					std::wstring file_path = out + L"\\" + index + L".dds";
					DWORD dwAttrib2 = GetFileAttributes(file_path.c_str());
					if (dwAttrib2 == INVALID_FILE_ATTRIBUTES || dwAttrib2 & FILE_ATTRIBUTE_DIRECTORY)
						continue;
					std::ifstream f3(file_path, std::ios::in | std::ios::binary);
					f3.seekg(0, f3.end);
					unsigned __int64 nRealLength = f3.tellg();
					f3.seekg(0, f3.beg);
					unsigned __int64 nAlignedLength = nRealLength;
					file.data[j] = std::shared_ptr<char>(new char[nAlignedLength]);
					memset(file.data[j].get(), 0, nAlignedLength);
					f3.read(file.data[j].get(), nRealLength);
					f3.close();
					file.length[j] = nAlignedLength;
				}

				file.header.Correction(file);

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

