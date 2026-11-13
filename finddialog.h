#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);

signals:
    void findRequested(const QString &text, bool backward);

private slots:
    void onFindClicked();

private:
    QLineEdit *m_editFind;
    QCheckBox *m_checkBackward;
};

#endif // FINDDIALOG_H
