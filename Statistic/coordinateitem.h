#ifndef COORDINATEITEM_H_
#define COORDINATEITEM_H_
#include <QGraphicsItem>

class CoordinateItem : public QGraphicsItem {
public:
    CoordinateItem(QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void setCoordinate(qreal width, qreal height, qreal xstart, qreal xend, qreal ystart, qreal yend);
    void setTicker(int xticker, int yticker);
    void setAnnotation(int xanno, int yanno);
    void setLabels(const QString& xlabel, const QString& ylabel);

    qreal width;
    qreal height;
    qreal xstart;
    qreal xend;
    qreal ystart;
    qreal yend;
    int xticker;
    int yticker;
    int xanno;
    int yanno;
    qreal xstep;
    qreal ystep;
    QString xlabel;
    QString ylabel;
    int xlabelWidth;
    int ylabelWidth;
    QVector<QString> xAnnoTexts;
    QVector<int> xAnnoWidths;
    QVector<QString> yAnnoTexts;
    QVector<int> yAnnoWidths;
    int maxXAnnoWidth;
    int maxYAnnoWidth;
    int fontHeight;
    QRectF bound;
};

#endif