#pragma once

#include <iostream>
#include <memory>

class FArchive
{
public:
	enum class Type
	{
		Read,
		Write
	};

private:
	Type m_type;
	std::iostream * m_stream;

public:
	FArchive(std::iostream * stream, Type type) : m_type(type), m_stream(stream)
	{

	}

	bool IsReading()
	{
		return m_type == Type::Read;
	}

	bool IsWriting()
	{
		return m_type == Type::Write;
	}

	unsigned __int64 Position()
	{
		return m_stream->tellg();
	}

	unsigned __int64 Length()
	{
		unsigned __int64 nPos = Position();
		m_stream->seekg(0, m_stream->end);
		unsigned __int64 nLength = Position();
		Seek(nPos);
		return nLength;
	}

	void Seek(unsigned __int64 nPos)
	{
		m_stream->seekg(nPos, m_stream->beg);
	}

	template<typename T>
	void Serialize(T & val)
	{
		if (IsReading())
			m_stream->read((char *)&val, sizeof(val));
		else if (IsWriting())
			m_stream->write((char *)&val, sizeof(val));
	}

	std::shared_ptr<char> Read(unsigned __int32 nCount)
	{
		std::shared_ptr<char> buf(new char[nCount]);
		m_stream->read(buf.get(), nCount);
		return buf;
	}

	void Write(const char * pBuf, unsigned __int32 nCount)
	{
		m_stream->write(pBuf, nCount);
	}
};

void operator << (FArchive & ar, __int32 & val)
{
	ar.Serialize(val);
}

void operator << (FArchive & ar, unsigned __int32 & val)
{
	ar.Serialize(val);
}
