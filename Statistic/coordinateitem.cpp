#include "coordinateitem.h"
#include <QFontMetrics>
#include <QFont>
#include <QPainter>

CoordinateItem::CoordinateItem(QGraphicsItem* parent) : QGraphicsItem(parent) {
}

void CoordinateItem::setCoordinate(qreal width, qreal height, qreal xstart, qreal xend, qreal ystart, qreal yend) {
    this->width = width;
    this->height = height;
    this->xstart = xstart;
    this->xend = xend;
    this->ystart = ystart;
    this->yend = yend;
    xstep = width / xticker;
    ystep = height / yticker;
    
    // 计算 annoText 及其宽度
    QFont font;
    QFontMetrics fontMetrics(font);
    fontHeight = fontMetrics.height();
    
    // 计算 x 轴 annoText
    xAnnoTexts.clear();
    xAnnoWidths.clear();
    maxXAnnoWidth = 0;
    int xannoDuration = xticker / xanno;
    qreal xannoStep = (xend - xstart) / xanno;
    int annoTimes = 0;
    for (int i = 0; i <= xanno; i++) {
        QString text = QString::number(quint64(xstart + i * xannoStep));
        int textWidth = fontMetrics.horizontalAdvance(text);
        xAnnoTexts.append(text);
        xAnnoWidths.append(textWidth);
        if (textWidth > maxXAnnoWidth) {
            maxXAnnoWidth = textWidth;
        }
    }
    
    // 计算 y 轴 annoText
    yAnnoTexts.clear();
    yAnnoWidths.clear();
    maxYAnnoWidth = 0;
    int yannoDuration = yticker / yanno;
    qreal yannoStep = (yend - ystart) / yanno;
    for (int i = 0; i <= yanno; i++) {
        QString text = QString::number(ystart + i * yannoStep, 'g', 2);
        int textWidth = fontMetrics.horizontalAdvance(text);
        yAnnoTexts.append(text);
        yAnnoWidths.append(textWidth);
        if (textWidth > maxYAnnoWidth) {
            maxYAnnoWidth = textWidth;
        }
    }
    
    // 更新 boundingRect
    bound = QRectF(-30, -(fontHeight + 30), width + 100, height + 100);
    update();
}

void CoordinateItem::setTicker(int xticker, int yticker) {
    this->xticker = xticker;
    this->yticker = yticker;
    update();
}

void CoordinateItem::setAnnotation(int xanno, int yanno) {
    this->xanno = xanno;
    this->yanno = yanno;
    update();
}

void CoordinateItem::setLabels(const QString& xlabel, const QString& ylabel) {
    this->xlabel = xlabel;
    this->ylabel = ylabel;
    QFont font;
    QFontMetrics fontMetrics(font);
    xlabelWidth = fontMetrics.horizontalAdvance(xlabel);
    ylabelWidth = fontMetrics.horizontalAdvance(ylabel);
    update();
}

void CoordinateItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::black);
    
    // 绘制坐标轴，延伸 50 并绘制箭头
    const int arrowSize = 10;
    const int extend = 50;
    const int yTextOffset = 10;
    
    // 绘制 x 轴
    painter->drawLine(0, 0, width + extend, 0);
    // 绘制 x 轴箭头
    painter->drawLine(width + extend, 0, width + extend - arrowSize, -arrowSize / 2);
    painter->drawLine(width + extend, 0, width + extend - arrowSize, arrowSize / 2);
    
    // 绘制 y 轴
    painter->drawLine(0, 0, 0, height + extend);
    // 绘制 y 轴箭头
    painter->drawLine(0, height + extend, -arrowSize / 2, height + extend - arrowSize);
    painter->drawLine(0, height + extend, arrowSize / 2, height + extend - arrowSize);
    
    // 绘制标签
    painter->drawText(width + extend - xlabelWidth, fontHeight + 20, xlabel); // x 轴 label 移到下方
    painter->drawText(10, height + extend - 20, ylabel);
    
    // 绘制 x 轴刻度和文字
    int times = 0;
    int xannoDuration = xticker / xanno;
    int annoIndex = 0;
    for (int i = 0; i <= width; i += xstep) {
        if (times % xannoDuration == 0 && annoIndex < xAnnoTexts.size()) {
            painter->drawLine(i, 0, i, 7);
            // x 轴文字居中显示
            int textWidth = xAnnoWidths[annoIndex];
            painter->drawText(i - textWidth / 2, fontHeight + 5, xAnnoTexts[annoIndex]); // 移到刻度下方
            annoIndex++;
        } else {
            painter->drawLine(i, 0, i, 5);
        }
        times++;
    }
    
    // 绘制 y 轴刻度和文字
    times = 0;
    annoIndex = 0;
    int yannoDuration = yticker / yanno;
    for (int i = 0; i <= height; i += ystep) {
        if (times % yannoDuration == 0 && annoIndex < yAnnoTexts.size()) {
            painter->drawLine(0, i, 7, i);
            // y 轴文字移到右边，距离坐标轴 10
            painter->drawText(yTextOffset, i + fontHeight / 2, yAnnoTexts[annoIndex]);
            annoIndex++;
        } else {
            painter->drawLine(0, i, 5, i);
        };
        times++;
    }
}

QRectF CoordinateItem::boundingRect() const {
    return bound;
}