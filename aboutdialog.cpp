#include "aboutdialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("关于");
    setFixedSize(320, 200);

    auto *label = new QLabel(
        "<h3>简单文本编辑器</h3>"
        "<p>Qt 应用程序开发 · 实验二</p>"
        "<p>版本：1.0</p>"
        "<p>作者：陈雨彤（2024414300203 软卓2班）</p>"
        "<p>支持文件读写、撤销重做、查找替换、<br>字体颜色与行号显示等功能。</p>",
        this);
    label->setAlignment(Qt::AlignCenter);

    auto *btnOk = new QPushButton("确定", this);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(btnOk, 0, Qt::AlignCenter);
}
