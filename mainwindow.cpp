#include "mainwindow.h"
#include "codeeditor.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "aboutdialog.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QFontDialog>
#include <QColorDialog>
#include <QTimer>
#include <QDateTime>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QPalette>
#include <QCloseEvent>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_editor(new CodeEditor(this))
    , m_findDlg(nullptr)
    , m_replaceDlg(nullptr)
    , m_autoSaveTimer(new QTimer(this))
{
    setCentralWidget(m_editor);

    createActions();
    createMenus();
    createToolBar();
    createStatusBar();

    // 修改状态联动窗口标题
    connect(m_editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::updateWindowTitle);
    // 撤销 / 重做 / 复制可用状态联动
    connect(m_editor->document(), &QTextDocument::undoAvailable,
            m_actUndo, &QAction::setEnabled);
    connect(m_editor->document(), &QTextDocument::redoAvailable,
            m_actRedo, &QAction::setEnabled);
    connect(m_editor, &CodeEditor::copyAvailable,
            m_actCut, &QAction::setEnabled);
    connect(m_editor, &CodeEditor::copyAvailable,
            m_actCopy, &QAction::setEnabled);
    connect(m_editor, &CodeEditor::cursorPositionChanged,
            this, &MainWindow::onCursorPositionChanged);

    m_autoSaveTimer->setInterval(60000); // 60 秒自动保存一次
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);

    setCurrentFile(QString());
    resize(900, 600);
    onCursorPositionChanged();
}

// ---------------- 动作创建 ----------------

void MainWindow::createActions()
{
    // 文件
    m_actNew = new QAction("新建", this);
    m_actNew->setShortcut(QKeySequence::New);
    connect(m_actNew, &QAction::triggered, this, &MainWindow::onNewFile);

    m_actOpen = new QAction("打开...", this);
    m_actOpen->setShortcut(QKeySequence::Open);
    connect(m_actOpen, &QAction::triggered, this, &MainWindow::onOpenFile);

    m_actSave = new QAction("保存", this);
    m_actSave->setShortcut(QKeySequence::Save);
    connect(m_actSave, &QAction::triggered, this, &MainWindow::onSaveFile);

    m_actSaveAs = new QAction("另存为...", this);
    m_actSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actSaveAs, &QAction::triggered, this, &MainWindow::onSaveFileAs);

    m_actQuit = new QAction("退出", this);
    m_actQuit->setShortcut(QKeySequence::Quit);
    connect(m_actQuit, &QAction::triggered, this, &QMainWindow::close);

    // 编辑
    m_actUndo = new QAction("撤销", this);
    m_actUndo->setShortcut(QKeySequence::Undo);
    m_actUndo->setEnabled(false);
    connect(m_actUndo, &QAction::triggered, this, &MainWindow::onUndo);

    m_actRedo = new QAction("重做", this);
    m_actRedo->setShortcut(QKeySequence::Redo);
    m_actRedo->setEnabled(false);
    connect(m_actRedo, &QAction::triggered, this, &MainWindow::onRedo);

    m_actCut = new QAction("剪切", this);
    m_actCut->setShortcut(QKeySequence::Cut);
    m_actCut->setEnabled(false);
    connect(m_actCut, &QAction::triggered, this, &MainWindow::onCut);

    m_actCopy = new QAction("复制", this);
    m_actCopy->setShortcut(QKeySequence::Copy);
    m_actCopy->setEnabled(false);
    connect(m_actCopy, &QAction::triggered, this, &MainWindow::onCopy);

    m_actPaste = new QAction("粘贴", this);
    m_actPaste->setShortcut(QKeySequence::Paste);
    connect(m_actPaste, &QAction::triggered, this, &MainWindow::onPaste);

    m_actSelectAll = new QAction("全选", this);
    m_actSelectAll->setShortcut(QKeySequence::SelectAll);
    connect(m_actSelectAll, &QAction::triggered, this, &MainWindow::onSelectAll);

    m_actFind = new QAction("查找...", this);
    m_actFind->setShortcut(QKeySequence::Find);
    connect(m_actFind, &QAction::triggered, this, &MainWindow::openFindDialog);

    m_actReplace = new QAction("替换...", this);
    m_actReplace->setShortcut(QKeySequence::Replace);
    connect(m_actReplace, &QAction::triggered, this, &MainWindow::openReplaceDialog);

    // 格式
    m_actFont = new QAction("字体...", this);
    connect(m_actFont, &QAction::triggered, this, &MainWindow::chooseFont);

    m_actFontColor = new QAction("字体颜色...", this);
    connect(m_actFontColor, &QAction::triggered, this, &MainWindow::chooseFontColor);

    m_actBgColor = new QAction("背景色...", this);
    connect(m_actBgColor, &QAction::triggered, this, &MainWindow::chooseBgColor);

    m_actWrap = new QAction("自动换行", this);
    m_actWrap->setCheckable(true);
    m_actWrap->setChecked(true);
    connect(m_actWrap, &QAction::toggled, this, &MainWindow::toggleWrap);

    // 视图
    m_actViewToolBar = new QAction("工具栏", this);
    m_actViewToolBar->setCheckable(true);
    m_actViewToolBar->setChecked(true);
    connect(m_actViewToolBar, &QAction::toggled, this, &MainWindow::toggleToolbar);

    m_actViewStatusBar = new QAction("状态栏", this);
    m_actViewStatusBar->setCheckable(true);
    m_actViewStatusBar->setChecked(true);
    connect(m_actViewStatusBar, &QAction::toggled, this, &MainWindow::toggleStatusBar);

    m_actViewLineNumber = new QAction("行号", this);
    m_actViewLineNumber->setCheckable(true);
    m_actViewLineNumber->setChecked(true);
    connect(m_actViewLineNumber, &QAction::toggled, this, &MainWindow::toggleLineNumber);

    m_actAutoSave = new QAction("定时自动保存", this);
    m_actAutoSave->setCheckable(true);
    connect(m_actAutoSave, &QAction::toggled, this, &MainWindow::toggleAutoSave);

    // 帮助
    m_actAbout = new QAction("关于...", this);
    connect(m_actAbout, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus()
{
    m_menuFile = menuBar()->addMenu("文件(&F)");
    m_menuFile->addAction(m_actNew);
    m_menuFile->addAction(m_actOpen);
    m_menuFile->addAction(m_actSave);
    m_menuFile->addAction(m_actSaveAs);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actQuit);

    m_menuEdit = menuBar()->addMenu("编辑(&E)");
    m_menuEdit->addAction(m_actUndo);
    m_menuEdit->addAction(m_actRedo);
    m_menuEdit->addSeparator();
    m_menuEdit->addAction(m_actCut);
    m_menuEdit->addAction(m_actCopy);
    m_menuEdit->addAction(m_actPaste);
    m_menuEdit->addAction(m_actSelectAll);
    m_menuEdit->addSeparator();
    m_menuEdit->addAction(m_actFind);
    m_menuEdit->addAction(m_actReplace);

    m_menuFormat = menuBar()->addMenu("格式(&O)");
    m_menuFormat->addAction(m_actFont);
    m_menuFormat->addAction(m_actFontColor);
    m_menuFormat->addAction(m_actBgColor);
    m_menuFormat->addSeparator();
    m_menuFormat->addAction(m_actWrap);

    m_menuView = menuBar()->addMenu("视图(&V)");
    m_menuView->addAction(m_actViewToolBar);
    m_menuView->addAction(m_actViewStatusBar);
    m_menuView->addAction(m_actViewLineNumber);
    m_menuView->addSeparator();
    m_menuView->addAction(m_actAutoSave);

    m_menuHelp = menuBar()->addMenu("帮助(&H)");
    m_menuHelp->addAction(m_actAbout);
}

void MainWindow::createToolBar()
{
    m_toolBar = addToolBar("主工具栏");
    m_toolBar->setObjectName("mainToolBar");

    // 文件组
    m_toolBar->addAction(m_actNew);
    m_toolBar->addAction(m_actOpen);
    m_toolBar->addAction(m_actSave);
    m_toolBar->addSeparator();

    // 编辑组
    m_toolBar->addAction(m_actUndo);
    m_toolBar->addAction(m_actRedo);
    m_toolBar->addAction(m_actCut);
    m_toolBar->addAction(m_actCopy);
    m_toolBar->addAction(m_actPaste);
    m_toolBar->addSeparator();

    // 查找替换组
    m_toolBar->addAction(m_actFind);
    m_toolBar->addAction(m_actReplace);
}

void MainWindow::createStatusBar()
{
    m_statusBar = statusBar();
    m_statusLabel = new QLabel(this);
    m_statusBar->addPermanentWidget(m_statusLabel);
}

// ---------------- 文件操作 ----------------

bool MainWindow::saveModified()
{
    if (!m_editor->document()->isModified())
        return true;

    QMessageBox msg(this);
    msg.setWindowTitle("未保存");
    msg.setText("文档内容已修改，是否保存更改？");
    QPushButton *saveBtn = msg.addButton("保存", QMessageBox::AcceptRole);
    QPushButton *discardBtn = msg.addButton("不保存", QMessageBox::DestructiveRole);
    QPushButton *cancelBtn = msg.addButton("取消", QMessageBox::RejectRole);
    msg.setDefaultButton(saveBtn);
    msg.exec();

    if (msg.clickedButton() == saveBtn)
        return onSaveFile();
    if (msg.clickedButton() == cancelBtn)
        return false;
    Q_UNUSED(discardBtn);
    return true;
}

void MainWindow::onNewFile()
{
    if (!saveModified())
        return;
    m_editor->clear();
    setCurrentFile(QString());
}

void MainWindow::onOpenFile()
{
    if (!saveModified())
        return;

    const QString fileName = QFileDialog::getOpenFileName(
        this, "打开文件", QString(),
        "文本文件 (*.txt *.md *.cpp *.h *.py *.log *.json);;所有文件 (*.*)");
    if (fileName.isEmpty())
        return;

    loadFile(fileName);
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::warning(this, "打开失败", "文件不存在：\n" + fileName);
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "打开失败",
                             "无法打开文件：\n" + file.errorString());
        return;
    }

    QTextStream in(&file);
    const QString text = in.readAll();
    file.close();

    m_editor->setPlainText(text);
    setCurrentFile(fileName);
    m_statusBar->showMessage("已打开 " + fileName, 3000);
}

bool MainWindow::onSaveFile()
{
    if (m_currentFile.isEmpty())
        return onSaveFileAs();
    return writeFile(m_currentFile);
}

bool MainWindow::onSaveFileAs()
{
    const QString suggested = m_currentFile.isEmpty() ? "未命名.txt" : m_currentFile;
    const QString fileName = QFileDialog::getSaveFileName(
        this, "另存为", suggested, "文本文件 (*.txt);;所有文件 (*.*)");
    if (fileName.isEmpty())
        return false;
    return writeFile(fileName);
}

bool MainWindow::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "保存失败",
                             "无法写入文件：\n" + file.errorString());
        return false;
    }

    QTextStream out(&file);
    out << m_editor->toPlainText();
    file.close();

    setCurrentFile(fileName);
    m_statusBar->showMessage("已保存 " + fileName, 3000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    m_editor->document()->setModified(false);
    updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
    const QString name = m_currentFile.isEmpty()
        ? QStringLiteral("未命名.txt") : QFileInfo(m_currentFile).fileName();
    setWindowTitle(QString("%1[*] - 简单文本编辑器").arg(name));
    setWindowModified(m_editor->document()->isModified());
}

// ---------------- 编辑操作 ----------------

void MainWindow::onUndo()  { m_editor->undo(); }
void MainWindow::onRedo()   { m_editor->redo(); }
void MainWindow::onCut()    { m_editor->cut(); }
void MainWindow::onCopy()  { m_editor->copy(); }
void MainWindow::onPaste() { m_editor->paste(); }
void MainWindow::onSelectAll() { m_editor->selectAll(); }

// ---------------- 查找与替换 ----------------

void MainWindow::openFindDialog()
{
    if (!m_findDlg) {
        m_findDlg = new FindDialog(this);
        connect(m_findDlg, &FindDialog::findRequested,
                this, &MainWindow::onFindRequested);
    }
    m_findDlg->show();
    m_findDlg->raise();
    m_findDlg->setFocus();
}

void MainWindow::openReplaceDialog()
{
    if (!m_replaceDlg) {
        m_replaceDlg = new ReplaceDialog(this);
        connect(m_replaceDlg, &ReplaceDialog::findNextRequested,
                this, [this](const QString &text) { onFindRequested(text, false); });
        connect(m_replaceDlg, &ReplaceDialog::replaceRequested,
                this, &MainWindow::onReplaceCurrent);
        connect(m_replaceDlg, &ReplaceDialog::replaceAllRequested,
                this, &MainWindow::onReplaceAll);
    }
    m_replaceDlg->show();
    m_replaceDlg->raise();
    m_replaceDlg->setFocus();
}

void MainWindow::onFindRequested(const QString &text, bool backward)
{
    if (text.isEmpty())
        return;

    QTextDocument::FindFlags flags;
    if (backward)
        flags |= QTextDocument::FindBackward;

    QTextCursor cursor = m_editor->textCursor();
    QTextCursor found = m_editor->document()->find(text, cursor, flags);

    // 未命中时回绕：向后查找从文档开头重来，向前查找从文档末尾重来
    if (found.isNull()) {
        QTextCursor wrap(m_editor->document());
        wrap.movePosition(backward ? QTextCursor::End : QTextCursor::Start);
        found = m_editor->document()->find(text, wrap, flags);
    }

    if (found.isNull()) {
        m_statusBar->showMessage(QString("未找到 \"%1\"").arg(text), 3000);
        QMessageBox::information(this, "查找", QString("未找到 \"%1\"").arg(text));
        return;
    }

    m_editor->setTextCursor(found);
    m_editor->setFocus();
}

void MainWindow::onReplaceNext(const QString &before)
{
    onFindRequested(before, false);
}

void MainWindow::onReplaceCurrent(const QString &before, const QString &after)
{
    QTextCursor cursor = m_editor->textCursor();
    if (cursor.hasSelection() && cursor.selectedText() == before)
        cursor.insertText(after);

    const QTextCursor found = m_editor->document()->find(before, cursor);
    if (found.isNull()) {
        m_statusBar->showMessage(QString("未找到 \"%1\"").arg(before), 3000);
        return;
    }
    m_editor->setTextCursor(found);
}

void MainWindow::onReplaceAll(const QString &before, const QString &after)
{
    if (before.isEmpty())
        return;

    m_editor->moveCursor(QTextCursor::Start);
    QTextCursor cursor = m_editor->textCursor();
    int count = 0;

    while (true) {
        const QTextCursor found = m_editor->document()->find(before, cursor);
        if (found.isNull())
            break;
        QTextCursor replace = found;
        replace.insertText(after);
        cursor = replace;
        ++count;
    }

    m_statusBar->showMessage(QString("共替换 %1 处").arg(count), 3000);
    onCursorPositionChanged();
}

// ---------------- 格式与视图 ----------------

void MainWindow::chooseFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, m_editor->font(), this);
    if (ok)
        m_editor->setFont(font);
}

void MainWindow::chooseFontColor()
{
    const QColor initial = m_editor->currentCharFormat().foreground().color();
    const QColor color = QColorDialog::getColor(initial, this);
    if (!color.isValid())
        return;

    QTextCharFormat fmt = m_editor->currentCharFormat();
    fmt.setForeground(color);
    m_editor->setCurrentCharFormat(fmt);

    // 若有选中内容，同步把颜色应用到选中部分
    QTextCursor cur = m_editor->textCursor();
    if (cur.hasSelection())
        cur.mergeCharFormat(fmt);
}

void MainWindow::chooseBgColor()
{
    const QColor color = QColorDialog::getColor(Qt::white, this, "编辑器背景色");
    if (color.isValid()) {
        QPalette pal = m_editor->palette();
        pal.setColor(QPalette::Base, color);
        m_editor->setPalette(pal);
    }
}

void MainWindow::toggleWrap(bool checked)
{
    m_editor->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth
                                      : QPlainTextEdit::NoWrap);
}

void MainWindow::toggleToolbar(bool checked)
{
    m_toolBar->setVisible(checked);
}

void MainWindow::toggleStatusBar(bool checked)
{
    m_statusBar->setVisible(checked);
}

void MainWindow::toggleLineNumber(bool checked)
{
    m_editor->setLineNumberAreaVisible(checked);
}

void MainWindow::toggleAutoSave(bool checked)
{
    if (checked) {
        m_autoSaveTimer->start();
        m_statusBar->showMessage("已开启定时自动保存（60 秒）", 3000);
    } else {
        m_autoSaveTimer->stop();
        m_statusBar->showMessage("已关闭定时自动保存", 3000);
    }
}

void MainWindow::autoSave()
{
    if (!m_editor->document()->isModified())
        return;
    if (m_currentFile.isEmpty()) {
        m_statusBar->showMessage("未命名文件，请先保存后再使用自动保存", 3000);
        return;
    }
    if (writeFile(m_currentFile))
        m_statusBar->showMessage(
            "已自动保存 " + QDateTime::currentDateTime().toString("HH:mm:ss"), 3000);
}

// ---------------- 其他 ----------------

void MainWindow::about()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::onCursorPositionChanged()
{
    const QTextCursor c = m_editor->textCursor();
    const int line = c.blockNumber() + 1;
    const int col = c.columnNumber() + 1;
    const int chars = m_editor->toPlainText().length();
    m_statusLabel->setText(QString("行: %1  列: %2  字符: %3")
                           .arg(line).arg(col).arg(chars));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (saveModified())
        event->accept();
    else
        event->ignore();
}
