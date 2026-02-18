#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QGraphicsView>
#include "core/Global.h"

namespace QtWordEditor {

class DocumentScene;
class Cursor;

/**
 * @brief 文档视图类，显示文档场景并处理缩放、滚动和事件转发
 *
 * 该类继承自QGraphicsView，负责：
 * 1. 显示文档场景内容
 * 2. 处理用户交互事件（键盘、鼠标等）
 * 3. 管理视图缩放和平移
 * 4. 转发事件给相应的处理器
 * 5. 维护光标位置和视图状态
 */
class DocumentView : public QGraphicsView
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为nullptr
     */
    explicit DocumentView(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~DocumentView() override;

    /**
     * @brief 设置要显示的文档场景
     * @param scene 文档场景对象指针
     */
    void setScene(DocumentScene *scene);

    /**
     * @brief 获取当前缩放比例
     * @return 当前的缩放因子
     */
    qreal zoom() const;
    
    /**
     * @brief 设置缩放比例
     * @param zoom 新的缩放因子
     */
    void setZoom(qreal zoom);
    
    /** @brief 放大视图 */
    void zoomIn();
    
    /** @brief 缩小视图 */
    void zoomOut();
    
    /** @brief 适应页面大小 */
    void zoomToFit();

signals:
    /** @brief 键盘按键按下时发出的信号 */
    void keyPressed(QKeyEvent *event);
    
    /** @brief 鼠标按下时发出的信号 */
    void mousePressed(const QPointF &scenePos);
    
    /** @brief 鼠标移动时发出的信号（场景坐标） */
    void mouseMoved(const QPointF &scenePos);
    
    /** @brief 鼠标释放时发出的信号（场景坐标） */
    void mouseReleased(const QPointF &scenePos);
    
    /** @brief 输入法事件接收时发出的信号 */
    void inputMethodReceived(QInputMethodEvent *event);
    
    /** @brief 鼠标位置变化时发出的信号 */
    void mousePositionChanged(const QPointF &scenePos, const QPoint &viewPos);
    
    /** @brief 缩放比例变化时发出的信号 */
    void zoomChanged(qreal zoom);

public:
    /** @brief 更新鼠标位置显示 */
    void updateMousePosition();
    
    /**
     * @brief 设置光标控制器
     * @param cursor 光标对象指针
     */
    void setCursor(Cursor *cursor);
    
    /**
     * @brief 设置光标的视觉位置
     * @param pos 光标在场景中的位置
     */
    void setCursorVisualPosition(const QPointF &pos);

protected:
    /**
     * @brief 处理键盘按下事件
     * @param event 键盘事件对象
     */
    void keyPressEvent(QKeyEvent *event) override;
    
    /**
     * @brief 处理键盘释放事件
     * @param event 键盘事件对象
     */
    void keyReleaseEvent(QKeyEvent *event) override;
    
    /**
     * @brief 处理鼠标按下事件
     * @param event 鼠标事件对象
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief 处理鼠标移动事件
     * @param event 鼠标事件对象
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    
    /**
     * @brief 处理鼠标释放事件
     * @param event 鼠标事件对象
     */
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    /**
     * @brief 处理鼠标滚轮事件
     * @param event 滚轮事件对象
     */
    void wheelEvent(QWheelEvent *event) override;
    
    /**
     * @brief 处理输入法事件
     * @param event 输入法事件对象
     */
    void inputMethodEvent(QInputMethodEvent *event) override;
    
    /**
     * @brief 处理内容滚动
     * @param dx 水平滚动距离
     * @param dy 垂直滚动距离
     */
    void scrollContentsBy(int dx, int dy) override;
    
    /**
     * @brief 处理窗口大小调整事件
     * @param event 调整大小事件对象
     */
    void resizeEvent(QResizeEvent *event) override;
    
    /**
     * @brief 查询输入法相关信息
     * @param query 查询类型
     * @return 查询结果
     */
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

private:
    qreal m_zoom;              ///< 当前缩放比例
    QPoint m_lastMousePos;     ///< 上次鼠标位置
    Cursor *m_cursor;          ///< 光标控制器
    QPointF m_cursorVisualPos; ///< 光标视觉位置
};

} // namespace QtWordEditor

#endif // DOCUMENTVIEW_H