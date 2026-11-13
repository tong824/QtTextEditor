#include "finddialog.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("查找");
    setModal(false);

    m_editFind = new QLineEdit(this);
    m_editFind->setPlaceholderText("请输入要查找的内容");

    m_checkBackward = new QCheckBox("向上查找", this);

    auto *btnFind = new QPushButton("查找下一个", this);
    auto *btnClose = new QPushButton("关闭", this);

    auto *topLayout = new QHBoxLayout;
    topLayout->addWidget(new QLabel("查找内容：", this));
    topLayout->addWidget(m_editFind);

    auto *middleLayout = new QHBoxLayout;
    middleLayout->addWidget(m_checkBackward);
    middleLayout->addStretch();
    middleLayout->addWidget(btnFind);
    middleLayout->addWidget(btnClose);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(middleLayout);

    connect(btnFind, &QPushButton::clicked, this, &FindDialog::onFindClicked);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::close);
    connect(m_editFind, &QLineEdit::returnPressed, this, &FindDialog::onFindClicked);

    m_editFind->setFocus();
}

void FindDialog::onFindClicked()
{
    const QString text = m_editFind->text();
    if (text.isEmpty())
        return;
    emit findRequested(text, m_checkBackward->isChecked());
}
