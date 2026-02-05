#ifndef INSTLABELITEM_H_
#define INSTLABELITEM_H_
#include <QWidget>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QColor>

class InstLabelItem : public QGraphicsTextItem {
public:
    InstLabelItem(QGraphicsItem* parent = nullptr);
    
    void setBorderColor(const QColor& color);
    void setFillColor(const QColor& color);
    void setBorderWidth(int width);
    void setWidthHeight(qreal width, qreal height);
    
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    
private:
    void checkAdjust();

private:
    QColor m_borderColor;
    QColor m_fillColor;
    int m_borderWidth;
    qreal max_width;
    qreal max_height;

};
#endif
