/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDocGallery module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgallerytrackerlistcolumn_p.h"

#include "qgallerytrackerschema_p.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE_DOCGALLERY

QVariant QGalleryTrackerStringColumn::toVariant(const QString &string) const
{
    return QVariant(string);
}

QVariant QGalleryTrackerStringListColumn::toVariant(const QString &string) const
{
    return string.split(m_separatorChar, QString::SkipEmptyParts);
}

QVariant QGalleryTrackerUrlColumn::toVariant(const QString &string) const
{
    return QUrl::fromEncoded(string.toUtf8(), QUrl::StrictMode);
}

QString QGalleryTrackerStringListColumn::toString(const QVariant &variant) const
{
    return variant.type() == QVariant::StringList
        ? variant.toStringList().join(m_separatorString)
        : variant.toString();
}

QVariant QGalleryTrackerIntegerColumn::toVariant(const QString &string) const
{
    bool ok;

    int number = string.toInt(&ok);

    return ok ? QVariant(number) : QVariant();
}

QVariant QGalleryTrackerDoubleColumn::toVariant(const QString &string) const
{
    bool ok;

    double number = string.toDouble(&ok);

    return ok ? QVariant(number) : QVariant();
}

QVariant QGalleryTrackerDateTimeColumn::toVariant(const QString &string) const
{
    QDateTime dateTime = QDateTime::fromString(string, Qt::ISODate);

    return dateTime.isValid() ? QVariant(dateTime) : QVariant();
}

QString QGalleryTrackerDateTimeColumn::toString(const QVariant &variant) const
{
    return variant.toDateTime().toString(Qt::ISODate);
}

QVariant QGalleryTrackerStaticColumn::value(QVector<QVariant>::const_iterator) const
{
    return m_value;
}

QVariant QGalleryTrackerPrefixColumn::value(QVector<QVariant>::const_iterator row) const
{
    return QString(m_prefix + (row + m_column)->toString());
}

QVariant QGalleryTrackerCompositeIdColumn::value(QVector<QVariant>::const_iterator row) const
{
    QString fragment = (row + m_columns.at(0))->toString();
    fragment.replace(QLatin1String("/"), QLatin1String("//"));

    return QString(m_prefix + fragment + QLatin1Char('/') + (row + m_columns.at(1))->toString());
}

QVariant QGalleryTrackerFileUrlColumn::value(QVector<QVariant>::const_iterator row) const
{
    return *(row + m_column);
}

QGalleryTrackerCompositeColumn *QGalleryTrackerFileUrlColumn::create(const QVector<int> &)
{
    return new QGalleryTrackerFileUrlColumn(QGALLERYTRACKERFILEURLCOLUMN_DEFAULT_COL);
}

QVariant QGalleryTrackerFilePathColumn::value(QVector<QVariant>::const_iterator row) const
{
    return (row + QGALLERYTRACKERFILEURLCOLUMN_DEFAULT_COL)->toUrl().path();
}

QGalleryTrackerCompositeColumn *QGalleryTrackerFilePathColumn::create(const QVector<int> &)
{
    return new QGalleryTrackerFilePathColumn;
}

QVariant QGalleryTrackerPathColumn::value(QVector<QVariant>::const_iterator row) const
{
    QString filePath = (row + QGALLERYTRACKERFILEURLCOLUMN_DEFAULT_COL)->toUrl().path();
    return filePath.section(QLatin1Char('/'), 0, -2);
}

QGalleryTrackerCompositeColumn *QGalleryTrackerPathColumn::create(const QVector<int> &)
{
    return new QGalleryTrackerPathColumn;
}

QVariant QGalleryTrackerFileExtensionColumn::value(QVector<QVariant>::const_iterator row) const
{
    QString fileName = (row + m_column)->toUrl().path();
    const int index = fileName.lastIndexOf(QLatin1Char('.'));
    return index > fileName.lastIndexOf(QLatin1Char('/'))
            ? QVariant(fileName.mid(index + 1))
            : QVariant();
}

QGalleryTrackerCompositeColumn *QGalleryTrackerFileExtensionColumn::create(const QVector<int> &)
{
    return new QGalleryTrackerFileExtensionColumn(QGALLERYTRACKERFILEURLCOLUMN_DEFAULT_COL);
}

QVariant QGalleryTrackerOrientationColumn::value(QVector<QVariant>::const_iterator row) const
{
    QString orientation = (row + m_column)->toString();
    if (orientation == QLatin1String("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#orientation-top"))
        return 0;
    else if (orientation == QLatin1String("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#orientation-left"))
        return 90;
    else if (orientation == QLatin1String("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#orientation-bottom"))
        return 180;
    else if (orientation == QLatin1String("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#orientation-right"))
        return 270;
    else
        return 0;
}

QGalleryTrackerCompositeColumn *QGalleryTrackerOrientationColumn::create(const QVector<int> &columns)
{
    return new QGalleryTrackerOrientationColumn(columns.first());
}

QT_END_NAMESPACE_DOCGALLERY
