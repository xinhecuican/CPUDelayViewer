#include "insttoolbar.h"
#include <QLabel>
#include "../config.h"
#include <QRegularExpressionValidator>

InstToolBar::InstToolBar(QWidget* parent) : QToolBar(parent) {
    setFloatable(false);
    setMovable(false);
    QLabel* jumpLabel = new QLabel(tr("跳转: "), this);
    addWidget(jumpLabel);
    jumpEdit = new QLineEdit(this);
    QFont defaultFont(Config::getStr("InstScnene/Font", "Courier New"), Config::getInt("InstScnene/FontSize", 12));
    QFontMetrics metrics(defaultFont);
    int instLabelWidth = metrics.horizontalAdvance("0000000000000000");
    jumpEdit->setMaximumWidth(instLabelWidth);
    jumpEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    // 限制只能输入数字
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(QRegularExpression("\\d+"), this);
    jumpEdit->setValidator(validator);
    connect(jumpEdit, &QLineEdit::editingFinished, this, [=, this](){
        bool ok;
        quint64 instId = jumpEdit->text().toULongLong(&ok);
        if (ok) {
            emit jumpToInst(instId);
        } else {
            instId = jumpEdit->text().toULongLong(&ok, 16);
            if (ok) {
                emit jumpToInst(instId);
            }
        }
    });
    
    addWidget(jumpEdit);
}

void InstToolBar::onTopRowChanged(quint64 inst_id) {
    jumpEdit->setPlaceholderText(QString::number(inst_id, 16));
}
