#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "core/Global.h"
#include "ui/dialogs/PageSetupDialog.h"
#include "editcontrol/cursor/Cursor.h"

namespace QtWordEditor {

class Document;
class DocumentScene;
class DocumentView;
class Selection;
class EditEventHandler;
class FormatController;
class StyleManager;
class RibbonBar;
class DebugConsole;

/**
 * @brief 主窗口类，应用程序的主要界面
 *
 * 该类是整个应用程序的主窗口，负责：
 * 1. 管理文档的创建、打开、保存等基本操作
 * 2. 协调各个组件之间的交互
 * 3. 提供用户界面元素（菜单、工具栏、状态栏等）
 * 4. 处理用户的输入事件和命令执行
 * 5. 管理调试控制台和其他辅助功能
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为nullptr
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow() override;

    /**
     * @brief 获取当前文档
     * @return 当前文档指针
     */
    Document *document() const;
    
    /**
     * @brief 设置当前文档
     * @param document 新的文档指针
     */
    void setDocument(Document *document);

protected:
    /**
     * @brief 处理窗口关闭事件
     * @param event 关闭事件对象
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    // ========== 文件操作槽函数 ==========
    
    /** @brief 创建新文档 */
    void newDocument();
    
    /** @brief 打开现有文档 */
    void openDocument();
    
    /** @brief 保存当前文档 */
    bool saveDocument();
    
    /** @brief 另存为文档 */
    bool saveAsDocument();
    
    /** @brief 显示关于对话框 */
    void about();

    // ========== 编辑操作槽函数 ==========
    
    /** @brief 撤销操作 */
    void undo();
    
    /** @brief 重做操作 */
    void redo();
    
    /** @brief 剪切操作 */
    void cut();
    
    /** @brief 复制操作 */
    void copy();
    
    /** @brief 粘贴操作 */
    void paste();

    // ========== 视图和设置槽函数 ==========
    
    /** @brief 放大视图 */
    void zoomIn();
    
    /** @brief 缩小视图 */
    void zoomOut();
    
    /** @brief 适应页面大小 */
    void zoomToFit();
    
    /** @brief 页面设置 */
    void pageSetup();
    
    /** @brief 更新窗口标题 */
    void updateWindowTitle();
    
    /** @brief 更新用户界面 */
    void updateUI();
    
    /** @brief 更新状态栏信息 */
    void updateStatusBar(const QPointF &scenePos, const QPoint &viewPos);
    
    /** @brief 切换到中文界面 */
    void switchToChinese();
    
    /** @brief 切换到英文界面 */
    void switchToEnglish();
    
    // ========== 光标相关槽函数 ==========
    
    /** @brief 更新光标位置显示
     * @param pos 新的光标位置
     */
    void updateCursorPosition(const CursorPosition &pos);
    
    /**
     * @brief 更新样式状态显示
     */
    void updateStyleState();
    
    /**
     * @brief 计算光标的视觉位置
     * @param pos 光标位置结构体
     * @return 对应的场景坐标位置
     */
    QPointF calculateCursorVisualPosition(const CursorPosition &pos);

private:
    /** @brief 设置用户界面 */
    void setupUi();
    
    /** @brief 设置自定义状态栏 */
    void setupCustomStatusBar();
    
    /** @brief 设置调试控制台 */
    void setupDebugConsole();
    
    /** @brief 创建动作（菜单项和工具栏按钮）*/
    void createActions();
    
    /** @brief 检查是否需要保存 */
    bool maybeSave();
    
    /** @brief 重新翻译界面文本 */
    void retranslateUi();

    Document *m_document;                   ///< 当前文档
    DocumentScene *m_scene;                 ///< 文档场景
    DocumentView *m_view;                   ///< 文档视图
    Cursor *m_cursor;                       ///< 光标控制器
    Selection *m_selection;                 ///< 选择控制器
    EditEventHandler *m_editEventHandler;   ///< 编辑事件处理器
    FormatController *m_formatController;   ///< 格式控制器
    StyleManager *m_styleManager;           ///< 样式管理器
    RibbonBar *m_ribbonBar;                 ///< 功能区工具栏

    QString m_currentFile;                  ///< 当前文件路径
    bool m_isModified;                      ///< 文档是否被修改
    qreal m_currentZoom;                    ///< 当前缩放比例
    QPointF m_lastScenePos;                 ///< 上次场景位置
    QPoint m_lastViewPos;                   ///< 上次视图位置
    PageSetup m_pageSetup;                  ///< 页面设置
    
    CursorPosition m_currentCursorPos;      ///< 当前光标位置
    
    QWidget *m_statusBarWidget;              ///< 状态栏自定义容器
    QLabel *m_statusLine1Label;              ///< 状态栏第一行
    QLabel *m_statusLine2Label;              ///< 状态栏第二行
    
    QDockWidget *m_debugConsoleDock;        ///< 调试控制台停靠窗口
    QPlainTextEdit *m_debugConsoleTextEdit; ///< 调试控制台文本编辑器
};

} // namespace QtWordEditor

#endif // MAINWINDOW_H
