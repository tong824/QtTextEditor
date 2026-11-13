#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CodeEditor;
class FindDialog;
class ReplaceDialog;

class QAction;
class QMenu;
class QToolBar;
class QStatusBar;
class QTimer;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件操作
    void onNewFile();
    void onOpenFile();
    bool onSaveFile();
    bool onSaveFileAs();

    // 编辑操作
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();

    // 查找与替换
    void openFindDialog();
    void openReplaceDialog();
    void onFindRequested(const QString &text, bool backward);
    void onReplaceNext(const QString &before);
    void onReplaceCurrent(const QString &before, const QString &after);
    void onReplaceAll(const QString &before, const QString &after);

    // 格式与视图
    void chooseFont();
    void chooseFontColor();
    void chooseBgColor();
    void toggleWrap(bool checked);
    void toggleToolbar(bool checked);
    void toggleStatusBar(bool checked);
    void toggleLineNumber(bool checked);
    void toggleAutoSave(bool checked);

    void about();
    void onCursorPositionChanged();
    void updateWindowTitle();

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();

    bool saveModified();                 // 返回 true 表示可以继续
    void loadFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void autoSave();

    CodeEditor *m_editor;
    FindDialog *m_findDlg;
    ReplaceDialog *m_replaceDlg;

    QMenu *m_menuFile;
    QMenu *m_menuEdit;
    QMenu *m_menuFormat;
    QMenu *m_menuView;
    QMenu *m_menuHelp;

    QToolBar *m_toolBar;
    QStatusBar *m_statusBar;
    QLabel *m_statusLabel;

    QAction *m_actNew;
    QAction *m_actOpen;
    QAction *m_actSave;
    QAction *m_actSaveAs;
    QAction *m_actQuit;

    QAction *m_actUndo;
    QAction *m_actRedo;
    QAction *m_actCut;
    QAction *m_actCopy;
    QAction *m_actPaste;
    QAction *m_actSelectAll;
    QAction *m_actFind;
    QAction *m_actReplace;

    QAction *m_actFont;
    QAction *m_actFontColor;
    QAction *m_actBgColor;
    QAction *m_actWrap;

    QAction *m_actViewToolBar;
    QAction *m_actViewStatusBar;
    QAction *m_actViewLineNumber;
    QAction *m_actAutoSave;

    QAction *m_actAbout;

    QString m_currentFile;
    QTimer *m_autoSaveTimer;
};

#endif // MAINWINDOW_H
