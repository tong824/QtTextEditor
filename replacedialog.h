#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

class QLineEdit;

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);

signals:
    void findNextRequested(const QString &text);
    void replaceRequested(const QString &before, const QString &after);
    void replaceAllRequested(const QString &before, const QString &after);

private slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();

private:
    QLineEdit *m_editFind;
    QLineEdit *m_editReplace;
};

#endif // REPLACEDIALOG_H
