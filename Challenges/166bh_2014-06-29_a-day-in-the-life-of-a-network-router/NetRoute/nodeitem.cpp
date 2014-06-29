#include <QtGui/QPainter>
#include <QtWidgets/QApplication>

#include "EdgeItem.h"
#include "NodeItem.h"

const float NodeItem::Radius = 20.0f;
const float NodeItem::Border = 2.0f;

NodeItem::NodeItem(QGraphicsItem* parent /* = NULL */)
    : QGraphicsObject(parent)
{
    resetBackground();
    resetBorderPen();
    resetSelectionPen();
    resetTextPen();
    resetFont();

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPos(0, 0);
}

NodeItem::~NodeItem()
{
}

void NodeItem::resetBackground ()
{
    QLinearGradient g (-Radius, -Radius, Radius / 2, Radius / 2);
    g.setColorAt (0, QColor (0, 255, 0));
    g.setColorAt (1, QColor (255, 255, 255));
    m_brush = g;
}

void NodeItem::setBackground (QBrush brush)
{
    m_brush = brush;
    invalidate ();
}

void NodeItem::setBorderPen (QPen pen)
{
    m_penBorder = pen;
    invalidate ();
}

void NodeItem::resetBorderPen ()
{
    m_penBorder = QPen (QBrush (Qt::darkGray), Border);
}

void NodeItem::setSelectionPen (QPen pen)
{
    m_penSelect = pen;
    invalidate ();
}

void NodeItem::resetSelectionPen ()
{
    m_penSelect = QPen (QBrush (Qt::gray), 1, Qt::DashLine);
}

void NodeItem::setTextPen (QPen pen)
{
    m_penText = pen;
    invalidate ();
}

void NodeItem::resetTextPen ()
{
    m_penText   = QPen (Qt::black);
}

void NodeItem::setFont (QFont font)
{
    m_font = font;
    invalidate ();
}

void NodeItem::resetFont ()
{
    m_font      = qApp->font ();
    m_font.setBold (true);
    m_font.setPixelSize(14);
}

void NodeItem::setText (const QString& text)
{
    m_text = text;
    invalidate ();
}

QRectF NodeItem::boundingRect () const
{
    QRectF rect (-Radius, -Radius, 2*Radius, 2*Radius);
    return rect.adjusted (-10, -10, 10, 10);
}

void NodeItem::addEdge (EdgeItem* edge)
{
    m_edges.append (edge);
}

void NodeItem::removeEdge (EdgeItem* edge)
{
    m_edges.removeAll (edge);
}

void NodeItem::invalidate ()
{
    QRectF rc = boundingRect ();
    rc.moveTo (pos ());
    update (rc);
}

void NodeItem::paint (QPainter* painter, const QStyleOptionGraphicsItem* option,
    QWidget* widget /* = NULL */)
{
    QRectF rect (-Radius, -Radius, 2*Radius, 2*Radius);

    // Draw the background and border
    painter->setBrush (m_brush);
    painter->setPen (m_penBorder);
    painter->drawEllipse (rect);

    // Draw the text, if there is any
    if (!m_text.isEmpty ()) {
        painter->setPen (m_penText);
        painter->setFont (m_font);
        painter->drawText (rect, Qt::AlignCenter | Qt::AlignVCenter |
            Qt::TextSingleLine, m_text);
    }

    // Draw the selection rectangle
    if (isSelected ()) {
        rect.adjust (-10, -10, 10, 10);
        painter->setPen (m_penSelect);
        painter->setBrush (Qt::NoBrush);
        painter->drawRoundedRect (rect, 10, 10);
    }
}

void NodeItem::mousePressEvent (QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent (event);
}

void NodeItem::mouseReleaseEvent (QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent (event);
}

void NodeItem::mouseDoubleClickEvent (QGraphicsSceneMouseEvent* event)
{
    emit nodeItemDoubleClicked (this);
    QGraphicsObject::mouseDoubleClickEvent (event);
}

QVariant NodeItem::itemChange (QGraphicsItem::GraphicsItemChange change,
    const QVariant& value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        QListIterator<EdgeItem*> i (m_edges);
        while (i.hasNext ()) {
            EdgeItem* edge = i.next ();
            edge->adjust ();
        }
    }

    return QGraphicsItem::itemChange (change, value);
}
