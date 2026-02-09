#include "insttoolbar.h"
#include <QLabel>
#include "../config.h"
#include <QRegularExpressionValidator>
#include "instscene.h"

InstToolBar::InstToolBar(DataLoader* loader, InstScene* scene, QWidget* parent) : QToolBar(parent) {
    setFloatable(false);
    setMovable(false);
    this->loader = loader;
    this->scene = scene;
    connect(this, &InstToolBar::jumpToInst, scene, &InstScene::jumpToInst);
    connect(scene, &InstScene::topRowChanged, this, &InstToolBar::onTopRowChanged);
    
    QLabel* capacityLabel = new QLabel(tr("容量: %1   ").arg(loader->getInstCount()), this);
    addWidget(capacityLabel);
    QLabel* jumpLabel = new QLabel(tr("跳转: "), this);
    addWidget(jumpLabel);
    jumpEdit = new QLineEdit(this);
    QFont defaultFont(Config::getStr("InstScene/Font", "Courier New"), Config::getInt("InstScene/FontSize", 12));
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
            jumpEdit->setText("");
        } else {
            instId = jumpEdit->text().toULongLong(&ok, 16);
            if (ok) {
                emit jumpToInst(instId);
                jumpEdit->setText("");
            }
        }
    });
    
    addWidget(jumpEdit);
}

void InstToolBar::onTopRowChanged(quint64 inst_id) {
    jumpEdit->setPlaceholderText(QString::number(inst_id));
}
