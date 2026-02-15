#include "graphdialog.h"

GraphDialog::GraphDialog(QWidget* parent) : QDialog(parent) {
    layout = new QVBoxLayout(this);
    setupUI();
    setupConnections();
}

GraphDialog::~GraphDialog() {
    auto it = conditions.begin();
    while (it != conditions.end()) {
        delete (*it);
        it = conditions.erase(it);
    }
}

void GraphDialog::setupUI() {
    // Primary key range section
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
    
    // Search conditions section
    QHBoxLayout* conditionsLayout = new QHBoxLayout();
    QLabel* conditionsLabel = new QLabel(tr("搜索条件:"), this);
    conditionsLayout->addWidget(conditionsLabel);
    conditionsLayout->addStretch();
    
    // Add button
    addConditionButton = new QPushButton(tr("添加条件"), this);
    conditionsLayout->addWidget(addConditionButton);
    layout->addLayout(conditionsLayout);

    layout->addStretch();
    
    // Button box
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    okButton = new QPushButton(tr("确定"), this);
    cancelButton = new QPushButton(tr("取消"), this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    // Set window properties
    setWindowTitle(tr("搜索条件设定"));
}

void GraphDialog::setupConnections() {
    connect(addConditionButton, &QPushButton::clicked, this, &GraphDialog::addCondition);
    connect(okButton, &QPushButton::clicked, this, &GraphDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void GraphDialog::showDialog(QWidget* trigger, DataLoader* loader) {
    if (loader) {
        this->trigger = trigger;
        this->loader = loader;
        instCount = loader->getInstCount();
        endLineEdit->setPlaceholderText(QString::number(instCount));
        fields = loader->getFieldsNames();
    }
    open();
}

void GraphDialog::addCondition() {
    Condition* condition = new Condition();
    
    // Create UI elements
    condition->fieldComboBox = new QComboBox(this);
    for (const auto& field : fields) {
        condition->fieldComboBox->addItem(field);
    }
    
    condition->operatorComboBox = new QComboBox(this);
    condition->operatorComboBox->addItem(tr(">"));
    condition->operatorComboBox->addItem(tr("<"));
    condition->operatorComboBox->addItem(tr("="));
    
    condition->valueComboBox = new QComboBox(this);
    condition->valueComboBox->setMinimumWidth(200);
    
    // Populate valueComboBox based on selected field
    auto populateValueComboBox = [=, this]() {
        condition->valueComboBox->clear();
        QString field = condition->fieldComboBox->currentText();
        if (field == "type" && loader) {
            QVector<QString> types = loader->getInstTypes();
            for (const auto& type : types) {
                condition->valueComboBox->addItem(type);
            }
        } else if (field == "result" && loader) {
            QVector<QString> results = loader->getResultNames();
            for (const auto& result : results) {
                condition->valueComboBox->addItem(result);
            }
        }
    };
    
    // Initial population
    populateValueComboBox();
    
    // Connect signal to update when field changes
    connect(condition->fieldComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]() {
        populateValueComboBox();
    });
    
    condition->logicComboBox = new QComboBox(this);
    condition->logicComboBox->addItem(tr("OR"));
    condition->logicComboBox->addItem(tr("AND"));
    
    condition->removeButton = new QPushButton(tr("删除"), this);
    
    // Create layout
    condition->layout = new QHBoxLayout();
    condition->layout->addWidget(condition->fieldComboBox);
    condition->layout->addWidget(condition->operatorComboBox);
    condition->layout->addWidget(condition->valueComboBox);
    condition->layout->addWidget(condition->logicComboBox);
    condition->layout->addWidget(condition->removeButton);
    
    // Add to layout and conditions list
    layout->insertLayout(layout->count() - 2, condition->layout); // Insert before add button and button box
    conditions.push_back(condition);
    
    // Connect remove button
    connect(condition->removeButton, &QPushButton::clicked, [=, this]() {
        removeCondition(condition);
    });
}

void GraphDialog::removeCondition(Condition* condition) {
    if (condition) {
        auto it = std::find(conditions.begin(), conditions.end(), condition);
        if (it != conditions.end()) {    
            layout->removeItem(condition->layout);
            delete condition;
            conditions.erase(it);
        }
    }
}

void GraphDialog::accept() {
    QDialog::accept();
    quint64 start, end;
    QString query = getQuery(start, end);
    emit conditionValid(trigger, loader, query, start, end);
}

GraphDialog::GraphDialogInfo GraphDialog::getDialogInfo() {
    GraphDialogInfo info;
    for (const auto& condition : conditions) {
        ConditionInfo condInfo;
        condInfo.field = condition->fieldComboBox->currentText();
        condInfo.op = condition->operatorComboBox->currentText();
        condInfo.value = condition->valueComboBox->currentText();
        info.conditions.append(condInfo);
    }
    info.start = startLineEdit->text().toLongLong();
    info.end = endLineEdit->text().isEmpty() ? endLineEdit->placeholderText().toLongLong() : endLineEdit->text().toLongLong();
    return info;
}

void GraphDialog::setDialogInfo(const GraphDialogInfo& info) {
    if (conditions.size() < info.conditions.size()) {
        for (int i = conditions.size(); i < info.conditions.size(); ++i) {
            addCondition();
        }
    } else {
        for (int i = info.conditions.size(); i < conditions.size(); ++i) {
            delete conditions[i];
        }
        conditions.resize(info.conditions.size());
    }
    for (int i = 0; i < info.conditions.size(); ++i) {
        const auto& condInfo = info.conditions[i];
        Condition* condition = conditions[i];
        condition->fieldComboBox->setCurrentText(condInfo.field);
        condition->operatorComboBox->setCurrentText(condInfo.op);
        condition->valueComboBox->setCurrentText(condInfo.value);
    }
    quint64 start_i = info.start < 0 ? 0 : info.start;
    quint64 end_i = info.end > instCount ? instCount : info.end;
    if (end_i - start_i < 1000) {
        start_i = end_i - 1000;
    }

    startLineEdit->setText(QString::number(start_i));
    endLineEdit->setText(QString::number(end_i));
}

QString GraphDialog::getQuery(quint64& start, quint64& end) {

    QString query = QString("SELECT COUNT(*) FROM insts WHERE ");
    for (int i = 0; i < conditions.size(); ++i) {
        query += conditions[i]->getQuery(i == conditions.size() - 1, loader);
    }
    start = startLineEdit->text().toLongLong();
    end = endLineEdit->text().isEmpty() ? endLineEdit->placeholderText().toLongLong() : endLineEdit->text().toLongLong();
    return query;
}