#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDir>

void *memcpyi(void *, long, size_t);
char *hprintf(int);

bool IsValidExtension(char *string);

QString GetIntelVirtualPath(QString &szFileVirtualPath);
QString GetDiffFromPaths(QString szFullPath, QString szDiffPath);
QString GetFileNameFromPath(QString szPath);
const char *GetFilteredPath(const char *szFilePath);
void CheckAndCreateDir(QString fileName);
QStringList GetFilesListFromDirectory(QString m_dir);

#endif // UTILS_H
