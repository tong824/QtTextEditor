#include "replacedialog.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("替换");
    setModal(false);

    m_editFind = new QLineEdit(this);
    m_editFind->setPlaceholderText("要查找的内容");
    m_editReplace = new QLineEdit(this);
    m_editReplace->setPlaceholderText("替换为");

    auto *btnFindNext = new QPushButton("查找下一个", this);
    auto *btnReplace = new QPushButton("替换", this);
    auto *btnReplaceAll = new QPushButton("全部替换", this);
    auto *btnClose = new QPushButton("关闭", this);

    auto *grid = new QGridLayout;
    grid->addWidget(new QLabel("查找内容：", this), 0, 0);
    grid->addWidget(m_editFind, 0, 1);
    grid->addWidget(new QLabel("替换为：", this), 1, 0);
    grid->addWidget(m_editReplace, 1, 1);

    auto *btnLayout = new QVBoxLayout;
    btnLayout->addWidget(btnFindNext);
    btnLayout->addWidget(btnReplace);
    btnLayout->addWidget(btnReplaceAll);
    btnLayout->addWidget(btnClose);

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(grid);
    mainLayout->addLayout(btnLayout);

    connect(btnFindNext, &QPushButton::clicked, this, &ReplaceDialog::onFindNext);
    connect(btnReplace, &QPushButton::clicked, this, &ReplaceDialog::onReplace);
    connect(btnReplaceAll, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAll);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::close);

    m_editFind->setFocus();
}

void ReplaceDialog::onFindNext()
{
    if (!m_editFind->text().isEmpty())
        emit findNextRequested(m_editFind->text());
}

void ReplaceDialog::onReplace()
{
    if (!m_editFind->text().isEmpty())
        emit replaceRequested(m_editFind->text(), m_editReplace->text());
}

void ReplaceDialog::onReplaceAll()
{
    if (!m_editFind->text().isEmpty())
        emit replaceAllRequested(m_editFind->text(), m_editReplace->text());
}
