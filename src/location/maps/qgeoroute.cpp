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

#include "qgeoroute.h"
#include "qgeoroute_p.h"

#include "qgeorectangle.h"
#include "qgeoroutesegment.h"

#include <QDateTime>
#include <QVariantMap>

QT_BEGIN_NAMESPACE

QT_DEFINE_QESDP_SPECIALIZATION_DTOR(QGeoRoutePrivate)

/*!
    \class QGeoRoute
    \inmodule QtLocation
    \ingroup QtLocation-routing
    \since 5.6

    \brief The QGeoRoute class represents a route between two points.

    A QGeoRoute object contains high level information about a route, such
    as the length the route, the estimated travel time for the route,
    and enough information to render a basic image of the route on a map.

    The QGeoRoute object also contains a list of QGeoRouteSegment objecs which
    describe subsections of the route in greater detail.

    Routing information is normally requested using
    QGeoRoutingManager::calculateRoute(), which returns a QGeoRouteReply
    instance. If the operation is completed successfully the routing
    information can be accessed with QGeoRouteReply::routes()

    \sa QGeoRoutingManager
*/

/*!
    Constructs a route object.
*/
QGeoRoute::QGeoRoute()
    : d_ptr(new QGeoRoutePrivate())
{}

/*!
    Returns the private implementation.
*/
QExplicitlySharedDataPointer<QGeoRoutePrivate> &QGeoRoute::d()
{
    return d_ptr;
}

const QExplicitlySharedDataPointer<QGeoRoutePrivate> &QGeoRoute::const_d() const
{
    return d_ptr;
}

/*!
    Constructs a route object from the contents of \a other.
*/
QGeoRoute::QGeoRoute(const QGeoRoute &other) noexcept = default;

/*!
    Destroys this route object.
*/
QGeoRoute::~QGeoRoute() = default;

/*!
    Assigns the contents of \a other to this route and returns a reference to
    this route.
*/
QGeoRoute &QGeoRoute::operator=(const QGeoRoute & other) noexcept
{
    if (this == &other)
        return *this;

    d_ptr = other.d_ptr;
    return *this;
}

/*!
    \fn bool QGeoRoute::operator==(const QGeoRoute &lhs, const QGeoRoute &rhs) noexcept

    Returns whether the routes \a lhs and \a rhs are equal.
*/

/*!
    \fn bool QGeoRoute::operator!=(const QGeoRoute &lhs, const QGeoRoute &rhs) noexcept

    Returns whether the routes \a lhs and \a rhs are not equal.
*/

bool QGeoRoute::isEqual(const QGeoRoute &other) const noexcept
{
    return ( (d_ptr.constData() == other.d_ptr.constData())
            || (*d_ptr) == (*other.d_ptr));
}


/*!
    Sets the identifier of this route to \a id.

    Service providers which support the updating of routes commonly assign
    identifiers to routes.  If this route came from such a service provider changing
    the identifier will probably cause route updates to stop working.
*/
void QGeoRoute::setRouteId(const QString &id)
{
    d_ptr->setId(id);
}

/*!
    Returns the identifier of this route.

    Service providers which support the updating of routes commonly assign
    identifiers to routes.  If this route did not come from such a service provider
    the function will return an empty string.
*/
QString QGeoRoute::routeId() const
{
    return d_ptr->id();
}

/*!
    Sets the route request which describes the criteria used in the
    calculcation of this route to \a request.
*/
void QGeoRoute::setRequest(const QGeoRouteRequest &request)
{
    d_ptr->setRequest(request);
}

/*!
    Returns the route request which describes the criteria used in
    the calculation of this route.
*/
QGeoRouteRequest QGeoRoute::request() const
{
    return d_ptr->request();
}

/*!
    Sets the bounding box which encompasses the entire route to \a bounds.
*/
void QGeoRoute::setBounds(const QGeoRectangle &bounds)
{
    d_ptr->setBounds(bounds);
}

/*!
    Returns a bounding box which encompasses the entire route.
*/
QGeoRectangle QGeoRoute::bounds() const
{
    return d_ptr->bounds();
}

/*!
    Sets the first route segment in the route to \a routeSegment.
*/
void QGeoRoute::setFirstRouteSegment(const QGeoRouteSegment &routeSegment)
{
    d_ptr->setFirstSegment(routeSegment);
}

/*!
    Returns the first route segment in the route.

    Will return an invalid route segment if there are no route segments
    associated with the route.

    The remaining route segments can be accessed sequentially with
    QGeoRouteSegment::nextRouteSegment.
*/
QGeoRouteSegment QGeoRoute::firstRouteSegment() const
{
    return d_ptr->firstSegment();
}

/*!
    Sets the estimated amount of time it will take to traverse this route,
    in seconds, to \a secs.
*/
void QGeoRoute::setTravelTime(int secs)
{
    d_ptr->setTravelTime(secs);
}

/*!
    Returns the estimated amount of time it will take to traverse this route,
    in seconds.
*/
int QGeoRoute::travelTime() const
{
    return d_ptr->travelTime();
}

/*!
    Sets the distance covered by this route, in meters, to \a distance.
*/
void QGeoRoute::setDistance(qreal distance)
{
    d_ptr->setDistance(distance);
}

/*!
    Returns the distance covered by this route, in meters.
*/
qreal QGeoRoute::distance() const
{
    return d_ptr->distance();
}

/*!
    Sets the travel mode for this route to \a mode.

    This should be one of the travel modes returned by request().travelModes().
*/
void QGeoRoute::setTravelMode(QGeoRouteRequest::TravelMode mode)
{
    d_ptr->setTravelMode(mode);
}

/*!
    Returns the travel mode for the this route.

    This should be one of the travel modes returned by request().travelModes().
*/
QGeoRouteRequest::TravelMode QGeoRoute::travelMode() const
{
    return d_ptr->travelMode();
}

/*!
    Sets the geometric shape of the route to \a path.

    The coordinates in \a path should be listed in the order in which they
    would be traversed by someone traveling along this segment of the route.
*/
void QGeoRoute::setPath(const QList<QGeoCoordinate> &path)
{
    d_ptr->setPath(path);
}

/*!
    Returns the geometric shape of the route.

    The coordinates should be listed in the order in which they
    would be traversed by someone traveling along this segment of the route.
*/
QList<QGeoCoordinate> QGeoRoute::path() const
{
    return d_ptr->path();
}

/*!
    Sets the route \a legs for a multi-waypoint route.

    \sa QGeoRouteLeg
    \since 5.12
*/
void QGeoRoute::setRouteLegs(const QList<QGeoRoute> &legs)
{
    d_ptr->setRouteLegs(legs);
}

/*!
    Returns the legs for the route.

    \sa QGeoRouteLeg
    \since 5.12
*/
QList<QGeoRoute> QGeoRoute::routeLegs() const
{
    return d_ptr->routeLegs();
}

/*!
    Sets the extended attributes \a extendedAttributes associated with this route.

    \since 5.13
*/
void QGeoRoute::setExtendedAttributes(const QVariantMap &extendedAttributes)
{
    d_ptr->setExtendedAttributes(extendedAttributes);
}

/*!
    Returns the extended attributes associated with this route.

    \since 5.13
*/
QVariantMap QGeoRoute::extendedAttributes() const
{
    return d_ptr->extendedAttributes();
}

/*!
    Sets the route leg index to \a idx.
*/
void QGeoRoute::setLegIndex(int idx)
{
    d()->setLegIndex(idx);
}

/*!
    Returns the index of this route leg inside the containing QGeoRoute::routeLegs list.
    Can be used to find the next legs.
*/
int QGeoRoute::legIndex() const
{
    return const_d()->legIndex();
}

/*!
    Sets the \a route that contains this route leg.
*/
void QGeoRoute::setOverallRoute(const QGeoRoute &route)
{
    d()->setContainingRoute(route);
}

/*!
    Returns the route that contains this route leg.
*/
QGeoRoute QGeoRoute::overallRoute() const
{
    return const_d()->containingRoute();
}

/*******************************************************************************
*******************************************************************************/

bool QGeoRoutePrivate::operator ==(const QGeoRoutePrivate &other) const
{
    return equals(other);
}

bool QGeoRoutePrivate::equals(const QGeoRoutePrivate &other) const
{
    // here both routes are of type QGeoRoutePrivate
    QGeoRouteSegment s1 = firstSegment();
    QGeoRouteSegment s2 = other.firstSegment();

    while (true) {
        if (s1.isValid() != s2.isValid())
            return false;
        if (!s1.isValid())
            break;
        if (s1 != s2)
            return false;
        s1 = s1.nextRouteSegment();
        s2 = s2.nextRouteSegment();
    }

    return id() == other.id()
        && request() == other.request()
        && bounds() == other.bounds()
        && travelTime() == other.travelTime()
        && distance() == other.distance()
        && travelMode() == other.travelMode()
        && path() == other.path()
        && routeLegs() == other.routeLegs()
        && extendedAttributes() == other.extendedAttributes();
}

void QGeoRoutePrivate::setId(const QString &id)
{
    m_id = id;
}

QString QGeoRoutePrivate::id() const
{
    return m_id;
}

void QGeoRoutePrivate::setRequest(const QGeoRouteRequest &request)
{
    m_request = request;
}

QGeoRouteRequest QGeoRoutePrivate::request() const
{
    return m_request;
}

void QGeoRoutePrivate::setBounds(const QGeoRectangle &bounds)
{
    m_bounds = bounds;
}

QGeoRectangle QGeoRoutePrivate::bounds() const
{
    return m_bounds;
}

void QGeoRoutePrivate::setTravelTime(int travelTime)
{
    m_travelTime = travelTime;
}

int QGeoRoutePrivate::travelTime() const
{
    return m_travelTime;
}

void QGeoRoutePrivate::setDistance(qreal distance)
{
    m_distance = distance;
}

qreal QGeoRoutePrivate::distance() const
{
    return m_distance;
}

void QGeoRoutePrivate::setTravelMode(QGeoRouteRequest::TravelMode mode)
{
    m_travelMode = mode;
}

QGeoRouteRequest::TravelMode QGeoRoutePrivate::travelMode() const
{
    return m_travelMode;
}

void QGeoRoutePrivate::setPath(const QList<QGeoCoordinate> &path)
{
    m_path = path;
}

QList<QGeoCoordinate> QGeoRoutePrivate::path() const
{
    return m_path;
}

void QGeoRoutePrivate::setFirstSegment(const QGeoRouteSegment &firstSegment)
{
    m_firstSegment = firstSegment;
    m_numSegments = -1;
}

QGeoRouteSegment QGeoRoutePrivate::firstSegment() const
{
    return m_firstSegment;
}

int QGeoRoutePrivate::segmentsCount() const
{
    if (m_numSegments >= 0)
        return m_numSegments;

    int count = 0;
    forEachSegment([&count](const QGeoRouteSegment &){
        ++count;
    });
    m_numSegments = count;
    return count;
}

QList<QGeoRouteSegment> QGeoRoutePrivate::segments() const
{
    QList<QGeoRouteSegment> segments;
    forEachSegment([&segments](const QGeoRouteSegment &segment){
        segments.append(segment);
    });
    return segments;
}

void QGeoRoutePrivate::setRouteLegs(const QList<QGeoRoute> &legs)
{
    m_legs = legs;
}

QList<QGeoRoute> QGeoRoutePrivate::routeLegs() const
{
    return m_legs;
}

void QGeoRoutePrivate::setExtendedAttributes(const QVariantMap &extendedAttributes)
{
    m_extendedAttributes = extendedAttributes;
}

QVariantMap QGeoRoutePrivate::extendedAttributes() const
{
    return m_extendedAttributes;
}

void QGeoRoutePrivate::setLegIndex(int idx)
{
    if (idx >= 0)
        m_legIndex = idx;
}

int QGeoRoutePrivate::legIndex() const
{
    return m_legIndex;
}

void QGeoRoutePrivate::setContainingRoute(const QGeoRoute &route)
{
    m_containingRoute.reset(new QGeoRoute(route));
}

QGeoRoute QGeoRoutePrivate::containingRoute() const
{
    if (m_containingRoute)
        return *m_containingRoute;
    return QGeoRoute();
}

QT_END_NAMESPACE

#include "moc_qgeoroute.cpp"
