/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "diffeditorcontroller.h"

#include <coreplugin/icore.h>

static const char settingsGroupC[] = "DiffEditor";
static const char contextLineNumbersKeyC[] = "ContextLineNumbers";
static const char ignoreWhitespaceKeyC[] = "IgnoreWhitespace";

namespace DiffEditor {

DiffEditorController::DiffEditorController(QObject *parent)
    : QObject(parent),
      m_descriptionEnabled(false),
      m_contextLinesNumber(3),
      m_ignoreWhitespace(true)
{
    QSettings *s = Core::ICore::settings();
    s->beginGroup(QLatin1String(settingsGroupC));
    m_contextLinesNumber = s->value(QLatin1String(contextLineNumbersKeyC),
                                    m_contextLinesNumber).toInt();
    m_ignoreWhitespace = s->value(QLatin1String(ignoreWhitespaceKeyC),
                                  m_ignoreWhitespace).toBool();
    s->endGroup();

    clear();
}

DiffEditorController::~DiffEditorController()
{

}

QString DiffEditorController::clearMessage() const
{
    return m_clearMessage;
}

QList<FileData> DiffEditorController::diffFiles() const
{
    return m_diffFiles;
}

QString DiffEditorController::workingDirectory() const
{
    return m_workingDirectory;
}

QString DiffEditorController::description() const
{
    return m_description;
}

bool DiffEditorController::isDescriptionEnabled() const
{
    return m_descriptionEnabled;
}

int DiffEditorController::contextLinesNumber() const
{
    return m_contextLinesNumber;
}

bool DiffEditorController::isIgnoreWhitespace() const
{
    return m_ignoreWhitespace;
}

QString DiffEditorController::makePatch(int diffFileIndex,
                                        int chunkIndex,
                                        bool revert) const
{
    if (diffFileIndex < 0 || chunkIndex < 0)
        return QString();

    if (diffFileIndex >= m_diffFiles.count())
        return QString();

    const FileData fileData = m_diffFiles.at(diffFileIndex);
    if (chunkIndex >= fileData.chunks.count())
        return QString();

    const ChunkData chunkData = fileData.chunks.at(chunkIndex);
    const bool lastChunk = (chunkIndex == fileData.chunks.count() - 1);

    const QString fileName = revert
            ? fileData.rightFileInfo.fileName
            : fileData.leftFileInfo.fileName;

    return DiffUtils::makePatch(chunkData,
                                fileName,
                                fileName,
                                lastChunk && fileData.lastChunkAtTheEndOfFile);
}

void DiffEditorController::clear()
{
    clear(tr("No difference"));
}

void DiffEditorController::clear(const QString &message)
{
    setDescription(QString());
    setDiffFiles(QList<FileData>());
    m_clearMessage = message;
    emit cleared(message);
}

void DiffEditorController::setDiffFiles(const QList<FileData> &diffFileList,
                  const QString &workingDirectory)
{
    m_diffFiles = diffFileList;
    m_workingDirectory = workingDirectory;
    emit diffFilesChanged(diffFileList, workingDirectory);
}

void DiffEditorController::setDescription(const QString &description)
{
    if (m_description == description)
        return;

    m_description = description;
    emit descriptionChanged(description);
}

void DiffEditorController::setDescriptionEnabled(bool on)
{
    if (m_descriptionEnabled == on)
        return;

    m_descriptionEnabled = on;
    emit descriptionEnablementChanged(on);
}

void DiffEditorController::setContextLinesNumber(int lines)
{
    const int l = qMax(lines, 1);
    if (m_contextLinesNumber == l)
        return;

    m_contextLinesNumber = l;

    QSettings *s = Core::ICore::settings();
    s->beginGroup(QLatin1String(settingsGroupC));
    s->setValue(QLatin1String(contextLineNumbersKeyC), m_contextLinesNumber);
    s->endGroup();

    emit contextLinesNumberChanged(l);
}

void DiffEditorController::setIgnoreWhitespace(bool ignore)
{
    if (m_ignoreWhitespace == ignore)
        return;

    m_ignoreWhitespace = ignore;

    QSettings *s = Core::ICore::settings();
    s->beginGroup(QLatin1String(settingsGroupC));
    s->setValue(QLatin1String(ignoreWhitespaceKeyC), m_ignoreWhitespace);
    s->endGroup();

    emit ignoreWhitespaceChanged(ignore);
}

void DiffEditorController::requestReload()
{
    emit reloadRequested();
}

} // namespace DiffEditor
