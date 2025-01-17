/****************************************************************************
**
** Copyright (C) 2020 Paolo Angelelli <paolo.angelelli@gmail.com>
** Copyright (C) 2020 The Qt Company Ltd.
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

#ifndef QDECLARATIVEPOLYLINEMAPITEM_P_P_H
#define QDECLARATIVEPOLYLINEMAPITEM_P_P_H

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

#include <QSharedPointer>
#include <QtCore/QScopedValueRollback>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGMaterialShader>
#include <QtPositioning/QGeoPath>
#include <QtPositioning/QGeoPolygon>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCircle>

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qdeclarativegeomapitemutils_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qgeomapitemgeometry_p.h>

#include <QtPositioning/private/qdoublevector2d_p.h>

#include <array>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QGeoMapPolylineGeometry : public QGeoMapItemGeometry
{
public:
    QGeoMapPolylineGeometry();

    void updateSourcePoints(const QGeoMap &map,
                            const QList<QDoubleVector2D> &path,
                            const QGeoCoordinate geoLeftBound);

    void updateScreenPoints(const QGeoMap &map,
                            qreal strokeWidth,
                            bool adjustTranslation = true);

    void clearSource();

    bool contains(const QPointF &point) const override;

    QList<QList<QDoubleVector2D> > clipPath(const QGeoMap &map,
                    const QList<QDoubleVector2D> &path,
                    QDoubleVector2D &leftBoundWrapped);

    void pathToScreen(const QGeoMap &map,
                      const QList<QList<QDoubleVector2D> > &clippedPaths,
                      const QDoubleVector2D &leftBoundWrapped);

public:
    QList<qreal> srcPoints_;
    QList<QPainterPath::ElementType> srcPointTypes_;

#ifdef QT_LOCATION_DEBUG
    QList<QDoubleVector2D> m_wrappedPath;
    QList<QList<QDoubleVector2D>> m_clippedPaths;
#endif

    friend class QDeclarativeCircleMapItem;
    friend class QDeclarativePolygonMapItem;
    friend class QDeclarativeRectangleMapItem;
};

class Q_LOCATION_PRIVATE_EXPORT VisibleNode
{
public:
    VisibleNode();
    virtual ~VisibleNode();

    bool subtreeBlocked() const;
    void setSubtreeBlocked(bool blocked);
    bool visible() const;
    void setVisible(bool visible);

    bool m_blocked : 1;
    bool m_visible : 1;
};

class Q_LOCATION_PRIVATE_EXPORT MapItemGeometryNode : public QSGGeometryNode, public VisibleNode
{
public:
    ~MapItemGeometryNode() override;
    bool isSubtreeBlocked() const override;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineMaterial : public QSGFlatColorMaterial
{
public:
    MapPolylineMaterial()
        : QSGFlatColorMaterial()
    {
        // Passing RequiresFullMatrix is essential in order to prevent the
        // batch renderer from baking in simple, translate-only transforms into
        // the vertex data. The shader will rely on the fact that
        // vertexCoord.xy is the Shape-space coordinate and so no modifications
        // are welcome.
        setFlag(Blending | RequiresFullMatrix);
    }

    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode renderMode) const override;

    void setGeoProjection(const QMatrix4x4 &p)
    {
        m_geoProjection = p;
    }

    QMatrix4x4 geoProjection() const
    {
        return m_geoProjection;
    }

    void setCenter(const QDoubleVector3D &c)
    {
        m_center = c;
    }

    QDoubleVector3D center() const
    {
        return m_center;
    }

    int wrapOffset() const
    {
        return m_wrapOffset;
    }

    void setWrapOffset(int wrapOffset)
    {
        m_wrapOffset = wrapOffset;
    }

    void setLineWidth(float lw)
    {
        m_lineWidth = lw;
    }

    float lineWidth() const
    {
        return m_lineWidth;
    }

    QSGMaterialType *type() const override;
    int compare(const QSGMaterial *other) const override;

protected:
    QMatrix4x4 m_geoProjection;
    QDoubleVector3D m_center;
    int m_wrapOffset = 0;
    float m_lineWidth = 1.0;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineNode : public MapItemGeometryNode
{
public:
    MapPolylineNode();
    ~MapPolylineNode() override;

    void update(const QColor &fillColor, const QGeoMapItemGeometry *shape);

protected:
    QSGFlatColorMaterial fill_material_;
    QSGGeometry geometry_;
};

class Q_LOCATION_PRIVATE_EXPORT QGeoMapItemLODGeometry
{
public:
    mutable std::array<QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>, 7>
            m_verticesLOD; // fix it to 7,
                           // do not allow simplifications beyond ZL 20. This could actually be
                           // limited even further
    mutable QList<QDeclarativeGeoMapItemUtils::vec2> *m_screenVertices;
    mutable QSharedPointer<unsigned int> m_working;

    QGeoMapItemLODGeometry()
    {
        resetLOD();
    }

    void resetLOD()
    {
        // New pointer, some old LOD task might still be running and operating on the old pointers.
        m_verticesLOD[0] = QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>(
                new QList<QDeclarativeGeoMapItemUtils::vec2>);
        for (unsigned int i = 1; i < m_verticesLOD.size(); ++i)
            m_verticesLOD[i] = nullptr; // allocate on first use
        m_screenVertices = m_verticesLOD.front().data(); // resetting pointer to data to be LOD 0
    }

    static unsigned int zoomToLOD(unsigned int zoom);

    static unsigned int zoomForLOD(unsigned int zoom);

    bool isLODActive(unsigned int lod) const;

    void selectLOD(unsigned int zoom, double leftBound, bool /*closed*/);

    static QList<QDeclarativeGeoMapItemUtils::vec2>
    getSimplified(QList<QDeclarativeGeoMapItemUtils::vec2> &wrappedPath, double leftBoundWrapped,
                  unsigned int zoom);

    static void enqueueSimplificationTask(
            const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>
                    &input, // reference as it gets copied in the nested call
            const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>> &output,
            double leftBound, unsigned int zoom, QSharedPointer<unsigned int> &working);

    void selectLODOnDataChanged(unsigned int zoom, double leftBound) const;

    bool selectLODOnLODMismatch(unsigned int zoom, double leftBound, bool closed) const
    {
        if (*m_working > 0) {
            return false;
        }
        const_cast<QGeoMapItemLODGeometry *>(this)->selectLOD(zoom,
                 leftBound,
                 closed);
        return true;
    }
};

class Q_LOCATION_PRIVATE_EXPORT QGeoMapPolylineGeometryOpenGL : public QGeoMapItemGeometry, public QGeoMapItemLODGeometry
{
public:
    typedef struct {
        QList<QDoubleVector2D> wrappedBboxes;
    } WrappedPolyline;

    QGeoMapPolylineGeometryOpenGL()
    {
        m_working = QSharedPointer<unsigned int>(new unsigned int(0));
    }

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoPolygon &poly);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoPath &poly);

    void updateSourcePoints(const QGeoProjectionWebMercator &p,
                            const QList<QDoubleVector2D> &wrappedPath,
                            const QGeoRectangle &boundingRectangle);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoRectangle &rect);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoCircle &circle);

    void updateScreenPoints(const QGeoMap &map,
                            qreal strokeWidth,
                            bool adjustTranslation = true);

    void updateQuickGeometry(const QGeoProjectionWebMercator &p, qreal strokeWidth = 0.0);

    bool allocateAndFillEntries(QSGGeometry *geom,
                                bool closed = false,
                                unsigned int zoom = 0) const;
    void allocateAndFillLineStrip(QSGGeometry *geom,
                                  int lod = 0) const;

    bool contains(const QPointF &point) const override
    {
        Q_UNUSED(point);
        return false;
    }

    static double distanceTo(const QDoubleVector2D &a, const QDoubleVector2D &b, const QDoubleVector2D &p)
    {
        double u = ((p.x() - a.x()) * (b.x() - a.x()) + (p.y() - a.y()) * (b.y() - a.y()) ) / (b - a).lengthSquared();
        QDoubleVector2D intersection(a.x() + u * (b.x() - a.x()) , a.y() + u * (b.y() - a.y()) );

        QDoubleVector2D candidate = ( (p-a).length() < (p-b).length() ) ? a : b;

        if (u > 0 && u < 1
            && (p-intersection).length() < (p-candidate).length()  ) // And it falls in the segment
                candidate = intersection;

        return qAbs((candidate - p).length());
    }
    // Note: this is also slightly incorrect on joins and in the beginning/end of the line
    bool contains(const QPointF &point, qreal lineWidth, const QGeoProjectionWebMercator &p) const
    {
        const double lineHalfWidth = lineWidth * 0.5;
        const QDoubleVector2D pt(point);
        QDoubleVector2D a;
        if (m_screenVertices->size())
            a = p.wrappedMapProjectionToItemPosition(p.wrapMapProjection(m_screenVertices->first().toDoubleVector2D()));
        QDoubleVector2D b;
        for (qsizetype i = 1; i < m_screenVertices->size(); ++i) {
            const auto &screenVertice = m_screenVertices->at(i);
            if (!a.isFinite()) {
                a = p.wrappedMapProjectionToItemPosition(p.wrapMapProjection(screenVertice.toDoubleVector2D()));
                continue;
            }

            b = p.wrappedMapProjectionToItemPosition(p.wrapMapProjection(screenVertice.toDoubleVector2D()));
            if (!b.isFinite()) {
                a = b;
                continue;
            }

            if (b == a)
                continue;

            // Heavily simplifying it here: if a point is not projectable, skip the segment.
            // For a correct solution, the segment should be clipped instead.
            if (distanceTo(a, b, pt) <= lineHalfWidth)
                return true;

            a = b;
        }
        return false;
    }

public:
    QDoubleVector2D m_bboxLeftBoundWrapped;
    QList<WrappedPolyline> m_wrappedPolygons;
    int m_wrapOffset;

    friend class QDeclarativeCircleMapItem;
    friend class QDeclarativePolygonMapItem;
    friend class QDeclarativeRectangleMapItem;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineShaderLineStrip : public QSGMaterialShader
{
public:
    MapPolylineShaderLineStrip();

    bool updateUniformData(RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineShaderExtruded : public QSGMaterialShader
{
public:
    MapPolylineShaderExtruded();

    bool updateUniformData(RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineNodeOpenGLLineStrip : public MapItemGeometryNode
{
public:
    MapPolylineNodeOpenGLLineStrip();
    ~MapPolylineNodeOpenGLLineStrip() override;

    void update(const QColor &fillColor,
                const qreal lineWidth,
                const QGeoMapPolylineGeometryOpenGL *shape,
                const QMatrix4x4 &geoProjection,
                const QDoubleVector3D &center,
                const Qt::PenCapStyle capStyle = Qt::SquareCap);

protected:
    MapPolylineMaterial fill_material_;
    QSGGeometry geometry_;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineMaterialExtruded : public MapPolylineMaterial
{
public:
    MapPolylineMaterialExtruded() : MapPolylineMaterial()
    {

    }
    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode renderMode) const override;

    void setMiter(int m)
    {
        m_miter = m;
    }

    int miter() const
    {
        return m_miter;
    }

    QSGMaterialType *type() const override;
    int compare(const QSGMaterial *other) const override;

    int m_miter = 0;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineNodeOpenGLExtruded : public MapItemGeometryNode
{
public:

    typedef struct MapPolylineEntry {
         QDeclarativeGeoMapItemUtils::vec2 pos;
         QDeclarativeGeoMapItemUtils::vec2 prev;
         QDeclarativeGeoMapItemUtils::vec2 next;
         float direction;
         float triangletype; // es2 does not support int attribs
         float vertextype;

         static const QSGGeometry::AttributeSet &attributes()
         {
             static const QSGGeometry::Attribute dataTri[] = {
                 QSGGeometry::Attribute::createWithAttributeType(0, 2, QSGGeometry::FloatType, QSGGeometry::PositionAttribute) // pos
                 ,QSGGeometry::Attribute::createWithAttributeType(1, 2, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute) // next
                 ,QSGGeometry::Attribute::createWithAttributeType(2, 2, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute) // previous
                 ,QSGGeometry::Attribute::createWithAttributeType(3, 1, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute)  // direction
                 ,QSGGeometry::Attribute::createWithAttributeType(4, 1, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute)  // triangletype
                 ,QSGGeometry::Attribute::createWithAttributeType(5, 1, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute)  // vertextype
             };
             static const QSGGeometry::AttributeSet attrsTri = { 6, sizeof(MapPolylineNodeOpenGLExtruded::MapPolylineEntry), dataTri };
             return attrsTri;
         }
    } MapPolylineEntry;

    MapPolylineNodeOpenGLExtruded();
    ~MapPolylineNodeOpenGLExtruded() override;

    void update(const QColor &fillColor,
                float lineWidth,
                const QGeoMapPolylineGeometryOpenGL *shape,
                const QMatrix4x4 &geoProjection,
                const QDoubleVector3D &center,
                const Qt::PenCapStyle capStyle = Qt::FlatCap,
                bool closed = false,
                unsigned int zoom = 30);

    static const QSGGeometry::AttributeSet &attributesMapPolylineTriangulated();

protected:
    MapPolylineMaterialExtruded fill_material_;
    QSGGeometry m_geometryTriangulating;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivate
{
public:
    QDeclarativePolylineMapItemPrivate(QDeclarativePolylineMapItem &poly) : m_poly(poly)
    {

    }
    QDeclarativePolylineMapItemPrivate(QDeclarativePolylineMapItemPrivate &other) : m_poly(other.m_poly)
    {
    }

    virtual ~QDeclarativePolylineMapItemPrivate();
    virtual void markSourceDirtyAndUpdate() = 0;
    virtual void onMapSet() = 0;
    virtual void onLinePropertiesChanged() = 0;
    virtual void onGeoGeometryChanged() = 0;
    virtual void onGeoGeometryUpdated() = 0;
    virtual void onItemGeometryChanged() = 0;
    virtual void updatePolish() = 0;
    virtual void afterViewportChanged() = 0;
    virtual QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) = 0;
    virtual bool contains(const QPointF &point) const = 0;

    QDeclarativePolylineMapItem &m_poly;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivateCPU: public QDeclarativePolylineMapItemPrivate
{
public:
    QDeclarativePolylineMapItemPrivateCPU(QDeclarativePolylineMapItem &poly) : QDeclarativePolylineMapItemPrivate(poly)
    {
    }

    QDeclarativePolylineMapItemPrivateCPU(QDeclarativePolylineMapItemPrivate &other)
    : QDeclarativePolylineMapItemPrivate(other)
    {
    }

    ~QDeclarativePolylineMapItemPrivateCPU() override;
    void onLinePropertiesChanged() override
    {
        // mark dirty just in case we're a width change
        markSourceDirtyAndUpdate();
    }
    void markSourceDirtyAndUpdate() override
    {
        m_geometry.markSourceDirty();
        m_poly.polishAndUpdate();
    }
    void regenerateCache()
    {
        if (!m_poly.map() || m_poly.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
            return;
        const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection());
        m_geopathProjected.clear();
        m_geopathProjected.reserve(m_poly.m_geopath.size());
        for (const QGeoCoordinate &c : m_poly.m_geopath.path())
            m_geopathProjected << p.geoToMapProjection(c);
    }
    void updateCache()
    {
        if (!m_poly.map() || m_poly.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
            return;
        const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection());
        m_geopathProjected << p.geoToMapProjection(m_poly.m_geopath.path().last());
    }
    void preserveGeometry()
    {
        m_geometry.setPreserveGeometry(true, m_poly.m_geopath.boundingGeoRectangle().topLeft());
    }
    void afterViewportChanged() override
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onMapSet() override
    {
        regenerateCache();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryChanged() override
    {
        regenerateCache();
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryUpdated() override
    {
        updateCache();
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onItemGeometryChanged() override
    {
        onGeoGeometryChanged();
    }
    void updatePolish() override
    {
        if (m_poly.m_geopath.path().length() < 2) { // Possibly cleared
            m_geometry.clear();
            m_poly.setWidth(0);
            m_poly.setHeight(0);
            return;
        }
        QScopedValueRollback<bool> rollback(m_poly.m_updatingGeometry);
        m_poly.m_updatingGeometry = true;

        const QGeoMap *map = m_poly.map();
        const qreal borderWidth = m_poly.m_line.width();

        m_geometry.updateSourcePoints(*map, m_geopathProjected, m_poly.m_geopath.boundingGeoRectangle().topLeft());
        m_geometry.updateScreenPoints(*map, borderWidth);

        m_poly.setWidth(m_geometry.sourceBoundingBox().width() + borderWidth);
        m_poly.setHeight(m_geometry.sourceBoundingBox().height() + borderWidth);

        m_poly.setPositionOnMap(m_geometry.origin(), -1 * m_geometry.sourceBoundingBox().topLeft()
                                + QPointF(borderWidth, borderWidth) * 0.5 ); // it has to be shifted so that the center of the line is on the correct geocoord
    }
    QSGNode *updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData * /*data*/) override
    {
        if (!m_node || !oldNode) {
            m_node = new MapPolylineNode();
            if (oldNode) {
                delete oldNode;
                oldNode = nullptr;
            }
        } else {
            m_node = static_cast<MapPolylineNode *>(oldNode);
        }

        //TODO: update only material
        if (m_geometry.isScreenDirty() || m_poly.m_dirtyMaterial || !oldNode) {
            m_node->update(m_poly.m_line.color(), &m_geometry);
            m_geometry.setPreserveGeometry(false);
            m_geometry.markClean();
            m_poly.m_dirtyMaterial = false;
        }
        return m_node;
    }
    bool contains(const QPointF &point) const override
    {
        return m_geometry.contains(point);
    }

    QList<QDoubleVector2D> m_geopathProjected;
    QGeoMapPolylineGeometry m_geometry;
    MapPolylineNode *m_node = nullptr;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivateOpenGLLineStrip: public QDeclarativePolylineMapItemPrivate
{
public:

    QDeclarativePolylineMapItemPrivateOpenGLLineStrip(QDeclarativePolylineMapItem &poly) : QDeclarativePolylineMapItemPrivate(poly)
    {
    }

    QDeclarativePolylineMapItemPrivateOpenGLLineStrip(QDeclarativePolylineMapItemPrivate &other)
    : QDeclarativePolylineMapItemPrivate(other)
    {
    }

    ~QDeclarativePolylineMapItemPrivateOpenGLLineStrip() override;
    void onLinePropertiesChanged() override
    {
        afterViewportChanged();
    }
    void markSourceDirtyAndUpdate() override
    {
        m_geometry.markSourceDirty();
        m_poly.polishAndUpdate();
    }
    void preserveGeometry()
    {
        m_geometry.setPreserveGeometry(true, m_poly.m_geopath.boundingGeoRectangle().topLeft());
    }
    void onMapSet() override
    {
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryChanged() override
    {
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryUpdated() override
    {
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onItemGeometryChanged() override
    {
        onGeoGeometryChanged();
    }
    void afterViewportChanged() override
    {
        preserveGeometry();
        m_poly.polishAndUpdate();
    }
    bool contains(const QPointF &point) const override
    {
        return m_geometry.contains(m_poly.mapToItem(m_poly.quickMap(), point),
                                   m_poly.line()->width(),
                                   static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection()));
    }
    void updatePolish() override
    {
        if (m_poly.m_geopath.path().length() == 0) { // Possibly cleared
            m_geometry.clear();
            m_geometry.clear();
            m_poly.setWidth(0);
            m_poly.setHeight(0);
            return;
        }

        QScopedValueRollback<bool> rollback(m_poly.m_updatingGeometry);
        m_poly.m_updatingGeometry = true;
        const qreal lineWidth = m_poly.m_line.width();
        m_geometry.updateSourcePoints(*m_poly.map(), m_poly.m_geopath);
        m_geometry.markScreenDirty();
        m_geometry.updateScreenPoints(*m_poly.map(), lineWidth);

        m_poly.setWidth(m_geometry.sourceBoundingBox().width());
        m_poly.setHeight(m_geometry.sourceBoundingBox().height());
        m_poly.setPosition(1.0 * m_geometry.firstPointOffset() - QPointF(lineWidth * 0.5,lineWidth * 0.5));
    }
    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override
    {
        Q_UNUSED(data);

        if (!m_node || !oldNode) {
            m_node = new MapPolylineNodeOpenGLLineStrip();
            if (oldNode)
                delete oldNode;
        } else {
            m_node = static_cast<MapPolylineNodeOpenGLLineStrip *>(oldNode);
        }

        if (m_geometry.isScreenDirty() || m_poly.m_dirtyMaterial) {
            const QGeoMap *map = m_poly.map();
            const QMatrix4x4 &combinedMatrix = map->geoProjection().qsgTransform();
            const QDoubleVector3D &cameraCenter = map->geoProjection().centerMercator();
            m_node->update(m_poly.m_line.color(), // This updates only the material if the geometry is unchanged
                           m_poly.m_line.width(),
                         &m_geometry,
                         combinedMatrix,
                         cameraCenter);
            m_geometry.setPreserveGeometry(false);
            m_geometry.markClean();
            m_poly.m_dirtyMaterial = false;
        }
        return m_node;
    }

    QGeoMapPolylineGeometryOpenGL m_geometry;
    MapPolylineNodeOpenGLLineStrip *m_node = nullptr;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivateOpenGLExtruded: public QDeclarativePolylineMapItemPrivateOpenGLLineStrip
{
public:

    QDeclarativePolylineMapItemPrivateOpenGLExtruded(QDeclarativePolylineMapItem &poly)
    : QDeclarativePolylineMapItemPrivateOpenGLLineStrip(poly)
    {
    }

    QDeclarativePolylineMapItemPrivateOpenGLExtruded(QDeclarativePolylineMapItemPrivate &other)
    : QDeclarativePolylineMapItemPrivateOpenGLLineStrip(other)
    {
    }

    ~QDeclarativePolylineMapItemPrivateOpenGLExtruded() override;

    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override
    {
        Q_UNUSED(data);
        const QGeoMap *map = m_poly.map();
        const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map->geoProjection());
        const QMatrix4x4 &combinedMatrix = p.qsgTransform();
        const QDoubleVector3D &cameraCenter = p.centerMercator();
        const QColor &color = m_poly.m_line.color();
        const float lineWidth = m_poly.m_line.width();

        MapPolylineNodeOpenGLExtruded *nodeTri = nullptr;
        if (!m_nodeTri || !oldNode) {
            if (oldNode)
                delete oldNode;
            nodeTri = new MapPolylineNodeOpenGLExtruded();
        } else {
            nodeTri = static_cast<MapPolylineNodeOpenGLExtruded *>(oldNode);
        }

        //TODO: update only material
        if (m_geometry.isScreenDirty() || m_poly.m_dirtyMaterial) {
            nodeTri->update(color,
                         lineWidth  ,
                         &m_geometry,
                         combinedMatrix,
                         cameraCenter,
                         Qt::FlatCap,
                         false,
                         m_poly.zoomForLOD(int(map->cameraData().zoomLevel())));
            m_geometry.setPreserveGeometry(false);
            m_geometry.markClean();
            m_poly.m_dirtyMaterial = false;
        }
        m_nodeTri = nodeTri;
        return nodeTri;
    }

    MapPolylineNodeOpenGLExtruded *m_nodeTri = nullptr;
};
QT_END_NAMESPACE

#endif // QDECLARATIVEPOLYLINEMAPITEM_P_P_H
