#pragma once

struct File;

struct Header
{
	unsigned __int32 nSize;
	unsigned __int32 nCount;
	unsigned __int32 nTotalSize;
	std::vector<unsigned __int32> tOffsets;

	void Serialize(FArchive & ar)
	{
		ar << nSize;
		ar << nCount;
		ar << nTotalSize;
		for (unsigned __int32 i = 0; i < nCount; i++)
		{
			if (ar.IsReading())
				tOffsets.push_back(0);
			ar << tOffsets[i];
		}
	}

	void Correction(const File & file);
};

struct File
{
	Header header;
	std::vector<std::shared_ptr<char>> data;
	std::vector<unsigned __int32> length;

	void Serialize(FArchive & ar)
	{
		header.Serialize(ar);
		unsigned __int32 nPrevOffset = 0;
		for (unsigned __int32 i = 0; i < header.nCount; i++)
		{
			Shift(ar, header.tOffsets[i] + header.nSize);
			if (ar.IsReading())
			{
				length.push_back((i + 1 < header.nCount ? header.tOffsets[i + 1] : header.nTotalSize - header.nSize) - header.tOffsets[i]);
				data.push_back(ar.Read(length[i]));
			}
			else
			{
				ar.Write(data[i].get(), length[i]);
			}
		}
	}

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
};