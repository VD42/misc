using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;

namespace WH40JFS
{
    public class JFS : BinaryHelper
    {
        public class File : BinaryHelper
        {
            public UInt32 m_nSomething1;
            public Int32 m_nOffset;
            public Int32 m_nCompressedSize;
            public Int32 m_nSize;
            public Int32 m_nSomething2;
            public Int32 m_nCompress;

            public void DeSerialize(Stream s)
            {
                m_nSomething1 = ReadUInt32(s);
                m_nOffset = ReadInt32(s);
                m_nCompressedSize = ReadInt32(s);
                m_nSize = ReadInt32(s);
                m_nSomething2 = ReadInt32(s);
                m_nCompress = ReadInt32(s);
            }

            public void Serialize(Stream s)
            {
                WriteUInt32(s, m_nSomething1);
                WriteInt32(s, m_nOffset);
                WriteInt32(s, m_nCompressedSize);
                WriteInt32(s, m_nSize);
                WriteInt32(s, m_nSomething2);
                WriteInt32(s, m_nCompress);
            }
        }

        public class FileInfo
        {
            public string m_sFilename;
            public Collection<string> m_tSomething;
            public DateTime m_dtDate;
        }

        public UInt32 m_nSign;
        public Int32 m_nSomething;
        public Int32 m_nOffset;
        public Int32 m_nFilesCount;
        public List<File> m_tFiles;
        public List<FileInfo> m_tFileInfos;

        public JFS()
        {
            m_tFiles = new List<File>();
            m_tFileInfos = new List<FileInfo>();
        }

        public void DeSerialize(Stream s)
        {
            m_nSign = ReadUInt32(s);
            if (m_nSign != 0x4a465301)
                throw new Exception("Bad file!");
            m_nSomething = ReadInt32(s);
            m_nOffset = ReadInt32(s);
            m_nFilesCount = ReadInt32(s);
            for (int i = 0; i < m_nFilesCount; i++)
            {
                m_tFiles.Add(new File());
                m_tFiles[m_tFiles.Count - 1].DeSerialize(s);
            }
        }

        public void Serialize(Stream s)
        {
            WriteUInt32(s, m_nSign);
            WriteInt32(s, m_nSomething);
            WriteInt32(s, m_nOffset);
            WriteInt32(s, m_nFilesCount);
            for (int i = 0; i < m_nFilesCount; i++)
                m_tFiles[i].Serialize(s);
        }

        public void ReadNames(Stream s)
        {
            BinaryFormatter bf = new BinaryFormatter();
            for (int i = 0; i < m_tFiles.Count; i++)
            {
                m_tFileInfos.Add(new FileInfo()
                {
                    m_sFilename = (string)bf.Deserialize(s),
                    m_tSomething = (Collection<string>)bf.Deserialize(s),
                    m_dtDate = (DateTime)bf.Deserialize(s)
                });
            }
        }

        public void WriteNames(Stream s)
        {
            BinaryFormatter bf = new BinaryFormatter();
            for (int i = 0; i < m_tFiles.Count; i++)
            {
                bf.Serialize(s, m_tFileInfos[i].m_sFilename);
                bf.Serialize(s, m_tFileInfos[i].m_tSomething);
                bf.Serialize(s, m_tFileInfos[i].m_dtDate);
            }
        }
    }

    class Program
    {
        public static void Unpack(string filename, string out_path)
        {
            string list_filename = Path.Combine(Path.GetDirectoryName(filename), Path.GetFileNameWithoutExtension(filename) + ".jfslist");

            Stream fs1 = File.Open(filename, FileMode.Open);
            Stream fs2 = File.Open(list_filename, FileMode.Open);
            JFS jfs = new JFS();
            jfs.DeSerialize(fs1);
            jfs.ReadNames(fs2);
            fs2.Close();
            for (int i = 0; i < jfs.m_tFiles.Count; i++)
            {
                string file_path = Path.Combine(out_path, jfs.m_tFileInfos[i].m_sFilename);
                string file_dir = Path.GetDirectoryName(file_path);
                Directory.CreateDirectory(file_dir);
                fs1.Seek(jfs.m_tFiles[i].m_nOffset, SeekOrigin.Begin);
                if (jfs.m_tFiles[i].m_nCompress == 1)
                {
                    UInt16 nSign = jfs.ReadUInt16(fs1);
                    if (nSign != 0x9c78)
                        throw new Exception("Bad compress!");
                    System.IO.Compression.DeflateStream ds = new System.IO.Compression.DeflateStream(fs1, System.IO.Compression.CompressionMode.Decompress, true);
                    byte[] data = new byte[jfs.m_tFiles[i].m_nSize];
                    ds.Read(data, 0, jfs.m_tFiles[i].m_nSize);
                    ds.Close();
                    Stream fs3 = File.Open(file_path, FileMode.Create);
                    fs3.Write(data, 0, data.Length);
                    fs3.Close();
                }
                else
                {
                    if (jfs.m_tFiles[i].m_nSize != jfs.m_tFiles[i].m_nCompressedSize)
                        throw new Exception("Compressed?!!");
                    byte[] data = new byte[jfs.m_tFiles[i].m_nSize];
                    fs1.Read(data, 0, jfs.m_tFiles[i].m_nSize);
                    Stream fs3 = File.Open(file_path, FileMode.Create);
                    fs3.Write(data, 0, data.Length);
                    fs3.Close();
                }
            }
            fs1.Close();
        }

        public class Adler32Computer
        {
            private int a = 1;
            private int b = 0;

            public int Checksum
            {
                get
                {
                    return ((b * 65536) + a);
                }
            }

            private static readonly int Modulus = 65521;

            public void Update(byte[] data, int offset, int length)
            {
                for (int counter = 0; counter < length; ++counter)
                {
                    a = (a + (data[offset + counter])) % Modulus;
                    b = (b + a) % Modulus;
                }
            }
        }

        public static void Pack(string filename, string out_path)
        {
            string list_filename = Path.Combine(Path.GetDirectoryName(filename), Path.GetFileNameWithoutExtension(filename) + ".jfslist");

            Stream fs1 = File.Open(filename, FileMode.Open);
            Stream fs2 = File.Open(list_filename, FileMode.Open);
            JFS jfs = new JFS();
            jfs.DeSerialize(fs1);
            jfs.ReadNames(fs2);
            fs2.Close();
            List<byte[]> tDatas = new List<byte[]>();
            int nCurrentOffset = jfs.m_nOffset + 12;
            for (int i = 0; i < jfs.m_tFiles.Count; i++)
            {
                string file_path = Path.Combine(out_path, jfs.m_tFileInfos[i].m_sFilename);
                if (!File.Exists(file_path))
                {
                    fs1.Seek(jfs.m_tFiles[i].m_nOffset, SeekOrigin.Begin);
                    tDatas.Add(new byte[jfs.m_tFiles[i].m_nCompressedSize]);
                    fs1.Read(tDatas[tDatas.Count - 1], 0, jfs.m_tFiles[i].m_nCompressedSize);
                    jfs.m_tFiles[i].m_nOffset = nCurrentOffset;
                    nCurrentOffset += jfs.m_tFiles[i].m_nCompressedSize + (jfs.m_tFiles[i].m_nCompressedSize % 4 > 0 ? 4 - (jfs.m_tFiles[i].m_nCompressedSize % 4) : 0);
                    continue;
                }
                Stream fs3 = File.Open(file_path, FileMode.Open);
                jfs.m_tFiles[i].m_nSize = (Int32)fs3.Length;
                if (jfs.m_tFiles[i].m_nCompress == 1)
                {
                    MemoryStream ms = new MemoryStream();
                    jfs.WriteUInt16(ms, 0x9c78);
                    System.IO.Compression.DeflateStream ds = new System.IO.Compression.DeflateStream(ms, System.IO.Compression.CompressionLevel.Optimal, true);
                    fs3.CopyTo(ds);
                    ds.Close();

                    Adler32Computer ad32 = new Adler32Computer();
                    byte[] BufForAdlerCalc = new byte[fs3.Length];
                    fs3.Seek(0, SeekOrigin.Begin);
                    fs3.Read(BufForAdlerCalc, 0, BufForAdlerCalc.Length);
                    ad32.Update(BufForAdlerCalc, 0, BufForAdlerCalc.Length);
                    byte[] AdlerRes = BitConverter.GetBytes(ad32.Checksum);
                    BinaryHelper bh = new BinaryHelper();
                    bh.WriteByte(ms, AdlerRes[3]);
                    bh.WriteByte(ms, AdlerRes[2]);
                    bh.WriteByte(ms, AdlerRes[1]);
                    bh.WriteByte(ms, AdlerRes[0]);

                    ms.Seek(0, SeekOrigin.Begin);
                    tDatas.Add(new byte[ms.Length]);
                    ms.Read(tDatas[tDatas.Count - 1], 0, tDatas[tDatas.Count - 1].Length);
                    ms.Close();
                    jfs.m_tFiles[i].m_nCompressedSize = tDatas[tDatas.Count - 1].Length;
                }
                else
                {
                    jfs.m_tFiles[i].m_nCompressedSize = jfs.m_tFiles[i].m_nSize;
                    tDatas.Add(new byte[jfs.m_tFiles[i].m_nSize]);
                    fs3.Read(tDatas[tDatas.Count - 1], 0, jfs.m_tFiles[i].m_nSize);
                }
                jfs.m_tFiles[i].m_nOffset = nCurrentOffset;
                nCurrentOffset += jfs.m_tFiles[i].m_nCompressedSize + (jfs.m_tFiles[i].m_nCompressedSize % 4 > 0 ? 4 - (jfs.m_tFiles[i].m_nCompressedSize % 4) : 0);
            }
            fs1.Close();
            fs1 = File.Open(filename, FileMode.Create);
            jfs.Serialize(fs1);
            for (int i = 0; i < jfs.m_tFiles.Count; i++)
            {
                if (fs1.Position != jfs.m_tFiles[i].m_nOffset)
                    throw new Exception("Bad offset calc!");
                fs1.Write(tDatas[i], 0, tDatas[i].Length);
                int nAllign = tDatas[i].Length % 4 > 0 ? 4 - (tDatas[i].Length % 4) : 0;
                for (int j = 0; j < nAllign; j++)
                    jfs.WriteByte(fs1, 0);
            }
            fs1.Close();
        }

        static void Main(string[] args)
        {
            if (args[0] == "unpack")
                Unpack(args[1], args[2]);
            else if (args[0] == "pack")
                Pack(args[1], args[2]);
        }
    }
}
