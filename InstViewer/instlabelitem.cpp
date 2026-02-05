#include "instlabelitem.h"
#include <QTextDocument>
#include <QFont>

InstLabelItem::InstLabelItem(QGraphicsItem* parent) : QGraphicsTextItem(parent),
    m_borderColor(Qt::black),
    m_fillColor(Qt::white),
    m_borderWidth(1) {
    setAcceptHoverEvents(false);
    QTextDocument* doc = document();
    doc->setDefaultTextOption(QTextOption(Qt::AlignCenter));
    connect(document(), &QTextDocument::contentsChanged,
            this, &InstLabelItem::checkAdjust);
}

void InstLabelItem::setBorderColor(const QColor& color)
{
    m_borderColor = color;
    update();
}

void InstLabelItem::setFillColor(const QColor& color)
{
    m_fillColor = color;
    update();
}

void InstLabelItem::setBorderWidth(int width)
{
    m_borderWidth = width;
    update();
}

void InstLabelItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (m_borderWidth > 0) {
        QRectF rect = boundingRect();
        
        painter->setRenderHint(QPainter::Antialiasing);
        
        painter->setBrush(m_fillColor);
        painter->setPen(QPen(m_borderColor, m_borderWidth));
        painter->drawRoundedRect(rect.adjusted(m_borderWidth, m_borderWidth, -m_borderWidth, -m_borderWidth), 5, 5);
    }
    
    QGraphicsTextItem::paint(painter, option, widget);
}

void InstLabelItem::setWidthHeight(qreal width, qreal height) {
    max_height = height;
    max_width = width;
    setTextWidth(max_width);
}

void InstLabelItem::checkAdjust() {
    QRectF bound = boundingRect();
    while (bound.height() > max_height) {
        prepareGeometryChange();
        QFont f = font();
        qreal pointsize = f.pointSizeF();
        f.setPointSizeF(pointsize - 1);
        update();
        bound = boundingRect();
    }
}
