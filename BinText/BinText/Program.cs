using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BinText
{
    class Program
    {
        public class TextInfo
        {
            public UInt32 m_nID;
            public string m_sString;
        }

        public static void Unpack(string fin, string fout)
        {
            Stream fs = File.Open(fin, FileMode.Open);
            List<TextInfo> texts = new List<TextInfo>();
            BinaryHelper bh = new BinaryHelper();
            while (fs.Position < fs.Length)
            {
                UInt32 nID = bh.ReadUInt32(fs);
                Int32 nLength = bh.ReadInt32(fs);
                byte[] pStr = new byte[nLength];
                fs.Read(pStr, 0, pStr.Length);
                texts.Add(new TextInfo()
                {
                    m_nID = nID,
                    m_sString = UTF8Encoding.UTF8.GetString(pStr)
                });
            }
            StreamWriter sw = new StreamWriter(fout);
            for (int i = 0; i < texts.Count; i++)
            {
                sw.Write(texts[i].m_nID);
                if (i < texts.Count - 1)
                    sw.Write(",");
            }
            sw.WriteLine();
            for (int i = 0; i < texts.Count; i++)
            {
                if (texts[i].m_sString.IndexOf("\\n") > -1)
                    throw new Exception("Found \\n!");
                sw.WriteLine(texts[i].m_sString.Replace("\n", "\\n"));
            }
            sw.Close();
        }

        public static void Pack(string fin, string fout)
        {
            StreamReader sr = new StreamReader(fout);
            string[] ids = sr.ReadLine().Split(',');
            Stream fs = File.Open(fin, FileMode.Create);
            BinaryHelper bh = new BinaryHelper();
            for (int i = 0; i < ids.Length; i++)
            {
                UInt32 nID = UInt32.Parse(ids[i]);
                bh.WriteUInt32(fs, nID);
                string sString = sr.ReadLine().Replace("\\n", "\n");
                byte[] pStr = UTF8Encoding.UTF8.GetBytes(sString);
                bh.WriteInt32(fs, pStr.Length);
                fs.Write(pStr, 0, pStr.Length);
            }
            fs.Close();
            while (!sr.EndOfStream)
            {
                string test = sr.ReadLine();
                if (test != "")
                    throw new Exception("Bad txt!");
            }
            sr.Close();
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
