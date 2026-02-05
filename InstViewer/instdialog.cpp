#include "instdialog.h"
#include "../DataLoader/dataloader.h"
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include "../config.h"

InstDialog::InstDialog(QWidget* parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    layout = new QVBoxLayout(this);
    QHBoxLayout* selectorLayout = new QHBoxLayout();
    layout->addLayout(selectorLayout);
    QLabel* modelLabel = new QLabel(tr("DataLoader 名称:"));
    QComboBox* modelComboBox = new QComboBox();
    for (auto& loader : DataLoader::loader_meta.keys()) {
        modelComboBox->addItem(loader);
    }
    selectorLayout->addWidget(modelLabel);
    selectorLayout->addWidget(modelComboBox);
    QHBoxLayout* fileNameLayout = new QHBoxLayout();
    layout->addLayout(fileNameLayout);
    QLabel* fileNameLabel = new QLabel(tr("数据文件路径:"), this);
    QString savePath = Config::get("instDialog/filepath").toString();
    QLineEdit* fileNameLineEdit = new QLineEdit(this);
    fileNameLineEdit->setText(savePath);
    QPushButton* openFileButton = new QPushButton(tr("打开文件"), this);
    connect(openFileButton, &QPushButton::clicked, this, [=, this](bool checked){
        QString filePath = QFileDialog::getOpenFileName(this, tr("打开数据文件"), fileNameLineEdit->text(), tr("数据文件 (*.db)"));
        if (filePath.isEmpty()) {
            return;
        }
        Config::set("instDialog/filepath", filePath);
        fileNameLineEdit->setText(filePath);
    });
    fileNameLayout->addWidget(fileNameLabel);
    fileNameLayout->addWidget(fileNameLineEdit, 1);
    fileNameLayout->addWidget(openFileButton);
    QHBoxLayout* okLayout = new QHBoxLayout();
    layout->addLayout(okLayout);
    QPushButton* okButton = new QPushButton(tr("确定"), this);
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    okLayout->addStretch();
    okLayout->addWidget(okButton, 0, Qt::AlignRight);
    connect(okButton, &QPushButton::clicked, this, [=, this](bool checked){
        model = modelComboBox->currentText();
        fileName = fileNameLineEdit->text();
        accept();
        emit dataValid(model, fileName);
    });
}
