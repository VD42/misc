using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BFTReader
{
    public class BFT : BinaryHelper
    {
        public class SymbolInfo : BinaryHelper
        {
            public UInt32 m_nId;
            public sbyte m_nXAdvanced; // ?
            public sbyte m_nXOffset; // ?
            public sbyte m_nYOffset; // ?
            public byte m_nWidth;
            public byte m_nHeight;
            public UInt32 m_nOffset;
            public UInt16 m_nLength;

            public void DeSerialize(Stream s)
            {
                UInt16 nTmp1 = ReadUInt16(s);
                byte nTmp2 = ReadByte(s);
                m_nId = (UInt32)nTmp1 + ((UInt32)nTmp2 << 16);

                m_nXAdvanced = (sbyte)ReadByte(s);
                m_nXOffset = (sbyte)ReadByte(s);
                m_nYOffset = (sbyte)ReadByte(s);
                m_nWidth = ReadByte(s);
                m_nHeight = ReadByte(s);

                UInt32 nTmp3 = ReadUInt32(s);
                m_nOffset = (nTmp3 & 0x001fffff) << 3;
                m_nLength = (UInt16)((nTmp3 & 0xffe00000) >> 21);
            }

            public void Serialize(Stream s)
            {
                WriteUInt16(s, (UInt16)(m_nId & 0xffff));
                WriteByte(s, (byte)(m_nId >> 16));
                WriteByte(s, (byte)m_nXAdvanced);
                WriteByte(s, (byte)m_nXOffset);
                WriteByte(s, (byte)m_nYOffset);
                WriteByte(s, m_nWidth);
                WriteByte(s, m_nHeight);
                WriteUInt32(s, ((UInt32)m_nOffset >> 3) | ((UInt32)m_nLength << 21));
            }
        }

        public Int32 m_nSize;

        public UInt16 m_nSign;
        public byte m_nSomething10;
        public byte m_nSomething11;
        public byte m_nSomething20;
        public byte m_nSomething21;
        public byte m_nSomething30;
        public byte m_nSomething31;
        public byte m_nSomething40;
        public byte m_nSomething41;
        public byte m_nSomething50;
        public byte m_nSomething51;
        public Int32 m_nCount;

        public List<SymbolInfo> m_tSymbols;
        public byte[] m_pImage;

        public BFT()
        {
            m_tSymbols = new List<SymbolInfo>();
        }

        public void DeSerialize(Stream s)
        {
            m_nSize = ReadInt32(s);
            byte[] data = new byte[m_nSize];
            System.IO.Compression.GZipStream gzip = new System.IO.Compression.GZipStream(s, System.IO.Compression.CompressionMode.Decompress, true);
            gzip.Read(data, 0, data.Length);
            gzip.Close();
            MemoryStream ms = new MemoryStream(data);
            m_nSign = ReadUInt16(ms);
            if (m_nSign != 0x4642)
                throw new Exception("Bad read!");
            m_nSomething10 = ReadByte(ms);
            m_nSomething11 = ReadByte(ms);
            m_nSomething20 = ReadByte(ms);
            m_nSomething21 = ReadByte(ms);
            m_nSomething30 = ReadByte(ms);
            m_nSomething31 = ReadByte(ms);
            m_nSomething40 = ReadByte(ms);
            m_nSomething41 = ReadByte(ms);
            m_nSomething50 = ReadByte(ms);
            m_nSomething51 = ReadByte(ms);
            m_nCount = ReadInt32(ms);
            StreamWriter sw = new StreamWriter(Program.path + ".txt");
            sw.WriteLine(
                ((sbyte)m_nSomething10).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething11).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething20).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething21).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething30).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething31).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething40).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething41).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething50).ToString().PadLeft(5) + " " +
                ((sbyte)m_nSomething51).ToString().PadLeft(5) + " " +
                m_nCount.ToString().PadLeft(5)
            );
            /*sw.WriteLine();
            sw.WriteLine();
            sw.WriteLine(
                (m_nSomething10 + m_nSomething11 * 256).ToString().PadLeft(5) + " " +
                (m_nSomething20 + m_nSomething21 * 256).ToString().PadLeft(5) + " " +
                (m_nSomething30 + m_nSomething31 * 256).ToString().PadLeft(5) + " " +
                (m_nSomething40 + m_nSomething41 * 256).ToString().PadLeft(5) + " " +
                (m_nSomething50 + m_nSomething51 * 256).ToString().PadLeft(5)
            );
            sw.WriteLine();
            sw.WriteLine();
            sw.WriteLine(
                (m_nSomething11 + m_nSomething20 * 256).ToString().PadLeft(5) + " " +
                (m_nSomething21 + m_nSomething30 * 256).ToString().PadLeft(5) + " " +
                (m_nSomething31 + m_nSomething40 * 256).ToString().PadLeft(5) + " " +
                (m_nSomething41 + m_nSomething50 * 256).ToString().PadLeft(5)
            );*/
            sw.WriteLine();
            sw.WriteLine();
            int nWidthMax = 0;
            int nHeightMax = 0;
            for (int i = 0; i < m_nCount; i++)
            {
                m_tSymbols.Add(new SymbolInfo());
                m_tSymbols[i].DeSerialize(ms);
                sw.WriteLine(
                    m_tSymbols[i].m_nId.ToString().PadLeft(5) + " " +
                    m_tSymbols[i].m_nXAdvanced.ToString().PadLeft(5) + " " +
                    m_tSymbols[i].m_nXOffset.ToString().PadLeft(5) + " " +
                    m_tSymbols[i].m_nYOffset.ToString().PadLeft(5) + " " +
                    m_tSymbols[i].m_nWidth.ToString().PadLeft(5) + " " +
                    m_tSymbols[i].m_nHeight.ToString().PadLeft(5) + " " +
                    m_tSymbols[i].m_nOffset.ToString().PadLeft(5) + " " +
                    m_tSymbols[i].m_nLength.ToString().PadLeft(5)
                );
                if (m_tSymbols[i].m_nWidth > nWidthMax)
                    nWidthMax = m_tSymbols[i].m_nWidth;
                if (m_tSymbols[i].m_nHeight > nHeightMax)
                    nHeightMax = m_tSymbols[i].m_nHeight;
            }
            sw.Close();

            int nImageLength = (int)(ms.Length - ms.Position);
            byte[] image = new byte[nImageLength];
            ms.Read(image, 0, image.Length);
            m_pImage = image;

            int nNumber = 1;
            Stream fs = File.Open(Program.path + ".1.tga", FileMode.Create);
            int nHeight = m_nCount * m_nSomething11;
            if (nHeight > 65535)
                nHeight = (65535 / m_nSomething11) * m_nSomething11;
            fs.Write(new byte[] { 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, (byte)(m_nSomething10 / 2), 0, (byte)(nHeight % 256), (byte)(nHeight / 256), 8, 32 }, 0, 18);
            int nCurrentHeight = 0;
            for (int i = 0; i < m_nCount; i++)
            {
                nCurrentHeight += m_nSomething11;
                if (nCurrentHeight > 65535)
                {
                    fs.Close();
                    nCurrentHeight = 0;
                    nNumber++;
                    fs = File.Open(Program.path + "." + nNumber + ".tga", FileMode.Create);
                    nHeight = (m_nCount - i) * m_nSomething11;
                    if (nHeight > 65535)
                        nHeight = (65535 / m_nSomething11) * m_nSomething11;
                    fs.Write(new byte[] { 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, (byte)(m_nSomething10 / 2), 0, (byte)(nHeight % 256), (byte)(nHeight / 256), 8, 32 }, 0, 18);
                    i--;
                }
                fs.Write(image, (int)m_tSymbols[i].m_nOffset, (m_nSomething10 / 2) * m_nSomething11);
            }

            /*int nNumber = 1;
            int nTotal = m_nCount;
            while (ms.Position < ms.Length)
            {
                Stream fs = File.Open(Program.path + "." + nNumber + ".tga", FileMode.Create);
                int nHeight = nTotal * m_nSomething10;
                if (nHeight > 65535)
                    nHeight = 65535;
                nTotal -= nHeight / m_nSomething10;
                int nImageLength = nHeight * m_nSomething10 / 2;
                byte[] image = new byte[nImageLength];
                ms.Read(image, 0, image.Length);
                fs.Write(new byte[] { 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, (byte)(m_nSomething10 / 2), 0, (byte)(nHeight % 256), (byte)(nHeight / 256), 8, 32 }, 0, 18);
                fs.Write(image, 0, image.Length);
                fs.Close();
                nNumber++;
            }*/
        }

        public void Serialize(Stream s)
        {
            MemoryStream ms = new MemoryStream();
            WriteUInt16(ms, m_nSign);
            WriteByte(ms, m_nSomething10);
            WriteByte(ms, m_nSomething11);
            WriteByte(ms, m_nSomething20);
            WriteByte(ms, m_nSomething21);
            WriteByte(ms, m_nSomething30);
            WriteByte(ms, m_nSomething31);
            WriteByte(ms, m_nSomething40);
            WriteByte(ms, m_nSomething41);
            WriteByte(ms, m_nSomething50);
            WriteByte(ms, m_nSomething51);
            WriteInt32(ms, m_tSymbols.Count);
            for (int i = 0; i < m_tSymbols.Count; i++)
                m_tSymbols[i].Serialize(ms);
            ms.Write(m_pImage, 0, m_pImage.Length);
            WriteUInt32(s, (UInt32)ms.Length);
            System.IO.Compression.GZipStream gzip = new System.IO.Compression.GZipStream(s, System.IO.Compression.CompressionLevel.Optimal, true);
            ms.Seek(0, SeekOrigin.Begin);
            ms.CopyTo(gzip);
            gzip.Close();
        }
    }

    public class BMFontInfo
    {
        public class CharInfo
        {
            public UInt32 m_nId;
            public UInt32 m_nX;
            public UInt32 m_nY;
            public byte m_nWidth;
            public byte m_nHeight;
            public sbyte m_nXOffset;
            public sbyte m_nYOffset;
            public sbyte m_nXAdvance;
            public byte m_nPage;
        }

        public byte m_nLineHeight;
        public byte m_nBase;
        public UInt32 m_nScaleW;
        public UInt32 m_nScaleH;
        public byte m_nPages;
        public UInt32 m_nCount;

        public Dictionary<byte, string> m_mapPages;
        public List<CharInfo> m_tChars;
        public List<byte[]> m_tData;

        public byte m_nMaxWidth;
        public byte m_nMaxHeight;

        public BMFontInfo()
        {
            m_mapPages = new Dictionary<byte, string>();
            m_tChars = new List<CharInfo>();
            m_tData = new List<byte[]>();
        }

        public static byte ReadByteValue(string line, string name)
        {
            line = line.Substring(line.IndexOf(name + "=") + (name + "=").Length);
            return byte.Parse(line.Substring(0, line.IndexOf(" ") > -1 ? line.IndexOf(" ") : line.Length));
        }

        public static sbyte ReadSbyteValue(string line, string name)
        {
            line = line.Substring(line.IndexOf(name + "=") + (name + "=").Length);
            return sbyte.Parse(line.Substring(0, line.IndexOf(" ") > -1 ? line.IndexOf(" ") : line.Length));
        }

        public static UInt32 ReadUInt32Value(string line, string name)
        {
            line = line.Substring(line.IndexOf(name + "=") + (name + "=").Length);
            return UInt32.Parse(line.Substring(0, line.IndexOf(" ") > -1 ? line.IndexOf(" ") : line.Length));
        }

        public static string ReadStringValue(string line, string name)
        {
            line = line.Substring(line.IndexOf(name + "=\"") + (name + "=\"").Length);
            return line.Substring(0, line.IndexOf("\""));
        }

        public void Read(string file)
        {
            StreamReader sr = new StreamReader(file);
            sr.ReadLine(); // nothing
            string line = sr.ReadLine();
            m_nLineHeight = ReadByteValue(line, "lineHeight");
            m_nBase = ReadByteValue(line, "base");
            m_nScaleW = ReadUInt32Value(line, "scaleW");
            m_nScaleH = ReadUInt32Value(line, "scaleH");
            m_nPages = ReadByteValue(line, "pages");
            for (int i = 0; i < m_nPages; i++)
            {
                line = sr.ReadLine();
                m_mapPages[ReadByteValue(line, "id")] = ReadStringValue(line, "file");
            }
            line = sr.ReadLine();
            m_nCount = ReadUInt32Value(line, "count");
            for (int i = 0; i < m_nCount; i++)
            {
                line = sr.ReadLine();
                m_tChars.Add(new CharInfo()
                {
                    m_nId = ReadUInt32Value(line, "id"),
                    m_nX = ReadUInt32Value(line, "x"),
                    m_nY = ReadUInt32Value(line, "y"),
                    m_nWidth = ReadByteValue(line, "width"),
                    m_nHeight = ReadByteValue(line, "height"),
                    m_nXOffset = ReadSbyteValue(line, "xoffset"),
                    m_nYOffset = ReadSbyteValue(line, "yoffset"),
                    m_nXAdvance = ReadSbyteValue(line, "xadvance"),
                    m_nPage = ReadByteValue(line, "page")
                });
                if (m_tChars[m_tChars.Count - 1].m_nWidth > m_nMaxWidth)
                    m_nMaxWidth = m_tChars[m_tChars.Count - 1].m_nWidth;
                if (m_tChars[m_tChars.Count - 1].m_nHeight > m_nMaxHeight)
                    m_nMaxHeight = m_tChars[m_tChars.Count - 1].m_nHeight;
            }
            Dictionary<byte, Stream> mapStreams = new Dictionary<byte, Stream>();
            foreach (var item in m_mapPages)
                mapStreams[item.Key] = File.Open(Path.Combine(Path.GetDirectoryName(file), item.Value), FileMode.Open);
            for (int i = 0; i < m_tChars.Count; i++)
            {
                m_tData.Add(new byte[m_nMaxWidth * m_nMaxHeight]);
                for (int x = 0; x < Math.Min(m_tChars[i].m_nWidth, m_nMaxWidth) / 2; x++)
                {
                    for (int y = 0; y < Math.Min(m_tChars[i].m_nHeight, m_nMaxHeight); y++)
                    {
                        mapStreams[m_tChars[i].m_nPage].Seek(18 + (m_tChars[i].m_nY + y) * m_nScaleW + m_tChars[i].m_nX + x * 2, SeekOrigin.Begin);
                        byte b1 = (byte)mapStreams[m_tChars[i].m_nPage].ReadByte();
                        byte b2 = (byte)mapStreams[m_tChars[i].m_nPage].ReadByte();
                        byte r = 0;
                        r |= (byte)(b2 >> 4);
                        r |= (byte)(b1 & 0xf0);
                        m_tData[m_tData.Count - 1][y * m_nMaxWidth + x] = r;
                    }
                }
            }
            foreach (var item in mapStreams)
                item.Value.Close();
            sr.Close();
        }
    }

    public class Program
    {
        //public static string path = @"C:\Users\vladi\Downloads\Новая папка\fonts (Поправить кернинг)\font\console_f.bft";
        public static string path = @"C:\Users\vladi\Documents\main_font_vita_en.bft";

        static void Main(string[] args)
        {
            if (false)
            {
                Stream fs = File.Open(Program.path, FileMode.Open);
                BFT bft = new BFT();
                bft.DeSerialize(fs);
                fs.Close();

                //bft.m_nSomething10 = 0; // width*2 of glyph
                //bft.m_nSomething11 = 0; // height of glyph
                bft.m_nSomething20 = 0; // nothing
                //bft.m_nSomething21 = 0; // something vertical shift (baseline?)
                bft.m_nSomething30 = 0; // nothing
                bft.m_nSomething31 = 0; // nothing
                bft.m_nSomething40 = 0; // nothing
                bft.m_nSomething41 = 0; // nothing
                bft.m_nSomething50 = 0; // nothing
                bft.m_nSomething51 = 4; // crash!!! type? flags?

                for (int i = 0; i < bft.m_tSymbols.Count; i++)
                {
                    if (bft.m_tSymbols[i].m_nId == 1080)
                        bft.m_tSymbols[i].m_nXAdvanced = 1;
                }

                Stream fs2 = File.Open(Program.path + ".new", FileMode.Create);
                bft.Serialize(fs2);
                fs2.Close();
            }
            else
            {
                //string fnt_in = @"C:\Users\vladi\Documents\comic_sdw.fnt";
                //string bft_out = @"C:\Users\vladi\Documents\main_font_sdw_vita_en.bft";
                string fnt_in = args[0];
                string bft_out = args[1];

                BMFontInfo info = new BMFontInfo();
                info.Read(fnt_in);
                BFT bft = new BFT();
                bft.m_nSign = 0x4642;
                bft.m_nSomething10 = (byte)(info.m_nMaxWidth * 2);
                bft.m_nSomething11 = info.m_nMaxHeight;
                bft.m_nSomething21 = info.m_nBase; // ?
                bft.m_nSomething51 = 4; // ?
                UInt32 nCurrentOffset = 0;
                for (int i = 0; i < info.m_tChars.Count; i++)
                {
                    bft.m_tSymbols.Add(new BFT.SymbolInfo()
                    {
                        m_nId = info.m_tChars[i].m_nId,
                        m_nWidth = info.m_tChars[i].m_nWidth,
                        m_nHeight = info.m_tChars[i].m_nHeight,
                        m_nXOffset = info.m_tChars[i].m_nXOffset,
                        m_nYOffset = info.m_tChars[i].m_nYOffset,
                        m_nXAdvanced = (sbyte)(info.m_tChars[i].m_nXAdvance - info.m_tChars[i].m_nWidth),
                        m_nLength = (UInt16)info.m_tData[i].Length,
                        m_nOffset = nCurrentOffset
                    });
                    nCurrentOffset += (UInt32)info.m_tData[i].Length + (UInt32)(info.m_tData[i].Length % 16 != 0 ? 16 - info.m_tData[i].Length % 16 : 0);
                }
                bft.m_pImage = new byte[nCurrentOffset];
                for (int i = 0; i < bft.m_tSymbols.Count; i++)
                    for (int j = 0; j < bft.m_tSymbols[i].m_nLength; j++)
                        bft.m_pImage[bft.m_tSymbols[i].m_nOffset + j] = info.m_tData[i][j];
                Stream fs = File.Open(bft_out, FileMode.Create);
                bft.Serialize(fs);
                fs.Close();
            }
        }
    }
}
