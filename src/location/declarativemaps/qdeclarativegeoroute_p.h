/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOROUTE_H
#define QDECLARATIVEGEOROUTE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtLocation/private/qlocationglobal_p.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtQml/QQmlPropertyMap>
#include <QtLocation/QGeoRoute>
#include <QtLocation/QGeoRouteSegment>

Q_MOC_INCLUDE(<QtLocation/private/qdeclarativegeoroutemodel_p.h>)

QT_BEGIN_NAMESPACE
class QDeclarativeGeoRouteQuery;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoRoute : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Route)
    Q_PROPERTY(QGeoRectangle bounds READ bounds CONSTANT)
    Q_PROPERTY(int travelTime READ travelTime CONSTANT)
    Q_PROPERTY(qreal distance READ distance CONSTANT)
    Q_PROPERTY(QList<QGeoCoordinate> path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QList<QGeoRouteSegment> segments READ segments CONSTANT)
    Q_PROPERTY(QDeclarativeGeoRouteQuery *routeQuery READ routeQuery REVISION(5, 11))
    Q_PROPERTY(QList<QDeclarativeGeoRoute *> legs READ legs CONSTANT REVISION(5, 12))
    Q_PROPERTY(QVariantMap extendedAttributes READ extendedAttributes CONSTANT REVISION(5, 13))
    Q_PROPERTY(int legIndex READ legIndex CONSTANT)
    Q_PROPERTY(QDeclarativeGeoRoute *overallRoute READ overallRoute CONSTANT)


public:
    explicit QDeclarativeGeoRoute(QObject *parent = nullptr);
    QDeclarativeGeoRoute(const QGeoRoute &route, QObject *parent = nullptr);
    ~QDeclarativeGeoRoute();

    QGeoRectangle bounds() const;
    int travelTime() const;
    qreal distance() const;

    QList<QGeoCoordinate> path() const;
    void setPath(const QList<QGeoCoordinate> &value);

    QList<QGeoRouteSegment> segments();
    int legIndex() const;
    QDeclarativeGeoRoute *overallRoute() const;

    int segmentsCount() const;
    const QGeoRoute &route() const;
    QDeclarativeGeoRouteQuery *routeQuery() const;
    QList<QDeclarativeGeoRoute *> legs() const;
    QVariantMap extendedAttributes() const;

    Q_INVOKABLE bool equals(QDeclarativeGeoRoute *other) const;

Q_SIGNALS:
    void pathChanged();

private:
    QList<QGeoCoordinate> routePath();

    QGeoRoute route_;
    mutable QDeclarativeGeoRouteQuery *routeQuery_ = nullptr;

    friend class QDeclarativeRouteMapItem;
};

QT_END_NAMESPACE

#endif
