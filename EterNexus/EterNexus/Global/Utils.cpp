#include "Utils.h"
#include "Version.h"

void *memcpyi(void *destination, long source, size_t num)
{
    return memcpy(destination, &source, num);
}

char __outhex[10];
char *hprintf(int num)
{
    snprintf(__outhex, 10, "%08X", num);
    return __outhex;
}

bool IsValidExtension(char *string)
{
    const char *szNotAllowed = "/\\:*?\"<>|";
    bool valid = true;

    for(size_t i = 0; i < strlen(szNotAllowed); i++)
    {
        if(strchr(string, szNotAllowed[i]))
            valid = false;
    }

    return valid;
}

QString GetIntelVirtualPath(QString &szFileVirtualPath)
{
    szFileVirtualPath.replace("\\", "/");

    if(szFileVirtualPath.left(QString("ymir work").length()) == "ymir work")
        szFileVirtualPath.insert(0, "d:/");

#ifdef ETER_NEXUS_UNIVERSAL_ELEMENTS
    if(szFileVirtualPath.left(QString("universalelements work").length()) == "universalelements work")
        szFileVirtualPath.insert(0, "d:/");
#endif

    return szFileVirtualPath;
}

QString GetDiffFromPaths(QString szFullPath, QString szDiffPath)
{
    if(szDiffPath.length() >= szFullPath.length()) return szFullPath;
    //if(szDiffPath.lastIndexOf("\\") >= szFullPath.length()) return szFullPath;

    return szFullPath.right(szFullPath.length()-szDiffPath.length()-1);
    //return szFullPath.right(szFullPath.length()-szDiffPath.lastIndexOf("\\")-1);
}

QString GetFileNameFromPath(QString szPath)
{
    if(!szPath.contains("\\")) return szPath;
    if(szPath.lastIndexOf("\\") == szPath.length()-1) szPath = szPath.right(szPath.length()-1);

    return szPath.right(szPath.length()-szPath.lastIndexOf("\\")-1);
}

char szFilteredPath[MAX_PATH];

const char *GetFilteredPath(const char *szFilePath)
{
    memcpy(szFilteredPath, szFilePath, strlen(szFilePath)+1);

    for(size_t i = 0; i < strlen(szFilteredPath); i++)
        if(szFilteredPath[i] == '/') szFilteredPath[i] = '\\';

    if(!strnicmp(szFilteredPath, "d:\\", 3)) return &szFilteredPath[3];

    return szFilteredPath;
}

void CheckAndCreateDir(QString fileName)
{
    fileName.replace("/", "\\");

    QString tPath, dir = fileName.left(fileName.lastIndexOf("\\"));

    if(!dir.contains("\\"))
        return;

    QStringList dirList = dir.split("\\");

    for(int i = 0; i < dirList.count(); i++)
    {
        if (!dirList.at(i).isNull() && dirList.at(i) != "d:")
        {
            tPath.append(dirList.at(i)).append("\\");
            if (!QDir(QString(tPath)).exists())
                QDir().mkdir(QString(tPath));
        }
    }
}

void GetFilesInDirectory(QString m_dir, QStringList &m_list)
{
    QDir m_dirInfo = QDir(m_dir);
    QStringList m_files = m_dirInfo.entryList(QDir::Files);

    for(int i = 0; i < m_files.length(); i++)
        m_list.append(m_dir + "\\" +m_files.at(i));
}

void GetAllSubDirectory(QString m_dir, QStringList &m_list)
{
    QDir m_dirInfo = QDir(m_dir);
    QStringList m_dirs = m_dirInfo.entryList(QDir::Dirs);

    for(int i = 2; i < m_dirs.length(); i++)
    {
        GetFilesInDirectory(m_dir + "\\" + m_dirs.at(i), m_list);
        GetAllSubDirectory(m_dir + "\\" + m_dirs.at(i), m_list);
    }
}

QStringList GetFilesListFromDirectory(QString m_dir)
{
    QStringList m_fileList;

    GetFilesInDirectory(m_dir, m_fileList);
    GetAllSubDirectory(m_dir, m_fileList);

    return m_fileList;
}
