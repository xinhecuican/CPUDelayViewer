#include "statrangedialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

StatRangeDialog::StatRangeDialog(QWidget* parent) : QDialog(parent) {
    this->loader = nullptr;
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* titleLabel = new QLabel(tr("从"), this);
    startLineEdit = new QLineEdit(this);
    QLabel* endLabel = new QLabel(tr("到"), this);
    endLineEdit = new QLineEdit(this);
    startLineEdit->setText("0");
    QHBoxLayout* lineLayout = new QHBoxLayout();
    lineLayout->addWidget(titleLabel);
    lineLayout->addWidget(startLineEdit);
    lineLayout->addWidget(endLabel);
    lineLayout->addWidget(endLineEdit);
    layout->addLayout(lineLayout);

    QHBoxLayout* okLayout = new QHBoxLayout();
    layout->addLayout(okLayout);
    QPushButton* okButton = new QPushButton(tr("确定"), this);
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    okLayout->addStretch();
    okLayout->addWidget(okButton, 0, Qt::AlignRight);
    connect(okButton, &QPushButton::clicked, this, [=, this](bool checked){
        accept();
        emit dataValid(loader, startLineEdit->text().toULongLong(), endLineEdit->text().toULongLong());
    });
}

void StatRangeDialog::showDialog(DataLoader* loader) {
    this->loader = loader;
    startLineEdit->setText("0");
    endLineEdit->setText(QString::number(loader->getInstCount()));
    open();
}
