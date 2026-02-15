#ifndef GRAPHDIALOG_H_
#define GRAPHDIALOG_H_
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <vector>
#include "../DataLoader/dataloader.h"

class GraphDialog : public QDialog {
    Q_OBJECT

public:
    struct ConditionInfo {
        QString field;
        QString op;
        QString value;
    };
    struct GraphDialogInfo {
        QVector<ConditionInfo> conditions;
        quint64 start;
        quint64 end;
    };

public:
    GraphDialog(QWidget* parent = nullptr);
    ~GraphDialog();

    void showDialog(QWidget* trigger, DataLoader* loader);
    GraphDialogInfo getDialogInfo();
    void setDialogInfo(const GraphDialogInfo& info);
    QString getQuery(quint64& start, quint64& end);

signals:
    void conditionValid(QWidget* trigger, DataLoader* loader, QString query, quint64 start, quint64 end);

private:
    struct Condition {
        QComboBox* fieldComboBox;
        QComboBox* operatorComboBox;
        QComboBox* valueComboBox;
        QComboBox* logicComboBox;
        QPushButton* removeButton;
        QHBoxLayout* layout;

        ~Condition() {
            delete fieldComboBox;
            delete operatorComboBox;
            delete valueComboBox;
            delete logicComboBox;
            delete removeButton;
            delete layout;
        }

        QString getQuery(bool isLast, DataLoader* loader) {
            QString field = fieldComboBox->currentText();
            QString op = operatorComboBox->currentText();
            QString value;
            if (field == "type" || field == "result") {
                value = QString::number(valueComboBox->currentIndex());
            } else {
                value = valueComboBox->currentText();
            }
            QString query = QString(" %1 %2 %3 ").arg(field, op, value);
            if (!isLast) {
                query += QString(" %1").arg(logicComboBox->currentText());
            }
            return query;
        }
    };
    
    void addCondition();
    void removeCondition(Condition* condition);
    void accept();

private:
    QVBoxLayout* layout;
    QLineEdit* startLineEdit;
    QLineEdit* endLineEdit;
    DataLoader* loader;
    quint64 instCount;
    QWidget* trigger;
    
    std::vector<Condition*> conditions;
    QPushButton* addConditionButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QVector<QString> fields;
    
    void setupUI();
    void setupConnections();
};

#endif