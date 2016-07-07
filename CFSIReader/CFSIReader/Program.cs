using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CFSIReader
{
    public class CFSI : BinaryHelper
    {
        public class FileInfo
        {
            public string m_sFileName;
            public UInt32 m_nOffset;
            public UInt32 m_nLength;
        }

        public List<FileInfo> m_tFiles;

        public CFSI()
        {
            m_tFiles = new List<FileInfo>();
        }

        public void DeSerialize(Stream s)
        {
            Console.WriteLine("Reading files table...");
            byte nCount = 0;
            while (nCount != 1)
                nCount = ReadByte(s); // ?
            while (nCount > 0)
            {
                byte nDirNameLength = ReadByte(s);
                byte[] pDirName = new byte[nDirNameLength];
                s.Read(pDirName, 0, pDirName.Length);
                string sDirName = ASCIIEncoding.ASCII.GetString(pDirName);
                nCount = ReadByte(s);
                for (int i = 0; i < nCount; i++)
                {
                    byte nFileNameLength = ReadByte(s);
                    byte[] pFileName = new byte[nFileNameLength];
                    s.Read(pFileName, 0, pFileName.Length);
                    string sFileName = ASCIIEncoding.ASCII.GetString(pFileName);
                    UInt32 nOffset = ReadUInt32(s);
                    UInt32 nLength = ReadUInt32(s);
                    m_tFiles.Add(new FileInfo() {
                        m_sFileName = sDirName + sFileName,
                        m_nOffset = nOffset << 4,
                        m_nLength = nLength
                    });
                }
            }
            while (s.Position % 16 != 0)
            {
                nCount = ReadByte(s);
                if (nCount != 0)
                    throw new Exception("Bad read!");
            }
        }

        public void Unpack(Stream s, string sDir)
        {
            UInt32 nStart = (UInt32)s.Position;

            for (int i = 0; i < m_tFiles.Count; i++)
            {
                string sFullPath = Path.Combine(sDir, m_tFiles[i].m_sFileName);
                Console.WriteLine("[" + (i + 1) + " of " + m_tFiles.Count + "] " + m_tFiles[i].m_sFileName);
                Directory.CreateDirectory(Path.GetDirectoryName(sFullPath));
                Stream fs = File.Open(sFullPath, FileMode.Create);
                byte[] pData = new byte[m_tFiles[i].m_nLength];
                while (s.Position != m_tFiles[i].m_nOffset + nStart)
                {
                    byte nTemp = ReadByte(s);
                    if (nTemp != 0)
                        throw new Exception("Bad read!");
                }
                s.Read(pData, 0, pData.Length);
                fs.Write(pData, 0, pData.Length);
                fs.Close();
            }
        }

        public void Pack(Stream s, string sDir, Dictionary<string, List<string>> files)
        {
            if (false)
                WriteUInt16(s, 0xe8fc); // priority?
            WriteByte(s, 1);
            UInt32 nCurrentOffset = 0;
            foreach (var item in files)
            {
                string path = item.Key.Substring(sDir.Length);
                if (path[0] == '\\')
                    path = path.Substring(1);
                if (path != "")
                    path += "\\";
                byte[] pDirName = ASCIIEncoding.ASCII.GetBytes(path.Replace('\\', '/'));
                WriteByte(s, (byte)pDirName.Length);
                s.Write(pDirName, 0, pDirName.Length);
                WriteByte(s, (byte)item.Value.Count);
                for (int i = 0; i < item.Value.Count; i++)
                {
                    byte[] pFileName = ASCIIEncoding.ASCII.GetBytes(item.Value[i].Replace('\\', '/'));
                    WriteByte(s, (byte)pFileName.Length);
                    s.Write(pFileName, 0, pFileName.Length);
                    Stream fs = File.Open(Path.Combine(item.Key, item.Value[i]), FileMode.Open);
                    UInt32 nLength = (UInt32)fs.Length;
                    fs.Close();
                    WriteUInt32(s, nCurrentOffset >> 4);
                    WriteUInt32(s, nLength);
                    m_tFiles.Add(new FileInfo()
                    {
                        m_sFileName = path + item.Value[i],
                        m_nOffset = nCurrentOffset,
                        m_nLength = nLength
                    });
                    nCurrentOffset += nLength + (nLength % 16 != 0 ? 16 - nLength % 16 : 0);
                }
            }
            while (s.Position % 16 != 0)
                WriteByte(s, 0);
            UInt32 nStart = (UInt32)s.Position;
            for (int i = 0; i < m_tFiles.Count; i++)
            {
                while (s.Position < nStart + m_tFiles[i].m_nOffset)
                    WriteByte(s, 0);
                Stream fs = File.Open(Path.Combine(sDir, m_tFiles[i].m_sFileName), FileMode.Open);
                byte[] pData = new byte[m_tFiles[i].m_nLength];
                fs.Read(pData, 0, pData.Length);
                s.Write(pData, 0, pData.Length);
                fs.Close();
            }
            while (s.Position % 16 != 0)
                WriteByte(s, 0);
        }
    }

    class Program
    {
        public static void Unpack(string filename, string out_path)
        {
            // unpack "E:\SteamLibrary\steamapps\common\Zero Escape\00000000.cfsi" "E:\SteamLibrary\steamapps\common\Zero Escape\00000000"
            // unpack "E:\SteamLibrary\steamapps\common\Zero Escape\00000001.cfsi" "E:\SteamLibrary\steamapps\common\Zero Escape\00000002"
            Stream fs = File.Open(filename, FileMode.Open);
            CFSI cfsi = new CFSI();
            cfsi.DeSerialize(fs);
            cfsi.Unpack(fs, out_path);
            fs.Close();
        }

        public static void GetFiles(string dir, ref Dictionary<string, List<string>> files)
        {
            string[] tFiles = Directory.GetFiles(dir);
            if (tFiles.Length > 0)
            {
                files.Add(dir, new List<string>());
                for (int i = 0; i < tFiles.Length; i++)
                    files[dir].Add(Path.GetFileName(tFiles[i]));
            }
            string[] tDirs = Directory.GetDirectories(dir);
            for (int i = 0; i < tDirs.Length; i++)
                GetFiles(tDirs[i], ref files);
        }

        public static void Pack(string filename, string files_path)
        {
            // pack "E:\SteamLibrary\steamapps\common\Zero Escape\00000001.cfsi" "E:\SteamLibrary\steamapps\common\Zero Escape\00000001"
            // pack "E:\SteamLibrary\steamapps\common\Zero Escape\00000001.cfsi" "E:\SteamLibrary\steamapps\common\Zero Escape\00000001"
            files_path = Path.GetFullPath(files_path);
            Dictionary<string, List<string>> m_mapFiles = new Dictionary<string, List<string>>();
            GetFiles(files_path, ref m_mapFiles);
            Stream fs = File.Open(filename, FileMode.Create);
            CFSI cfsi = new CFSI();
            cfsi.Pack(fs, files_path, m_mapFiles);
            fs.Close();
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
