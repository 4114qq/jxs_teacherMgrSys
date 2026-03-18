#ifndef ADDCONFIGDIALOG_H
#define ADDCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class AddConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddConfigDialog(const QStringList &groups, QWidget *parent = nullptr);
    ~AddConfigDialog();

    QString key() const;
    QString value() const;
    QString description() const;
    QString group() const;

private slots:
    void onAddGroupClicked();
    void onOKClicked();

private:
    void setupUI(const QStringList &groups);

    QLineEdit *m_keyEdit;
    QLineEdit *m_valueEdit;
    QLineEdit *m_descEdit;
    QComboBox *m_groupCombo;
    QPushButton *m_addGroupBtn;
    QPushButton *m_okBtn;
    QPushButton *m_cancelBtn;
};

#endif // ADDCONFIGDIALOG_H
