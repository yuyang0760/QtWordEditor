/**
 * @file RowContainerMathSpan.h
 * @brief 水平行容器数据类
 * 
 * 用于在水平方向上排列多个公式元素，是最常用的容器类型。
 */

#ifndef ROWCONTAINERMATHSPAN_H
#define ROWCONTAINERMATHSPAN_H

#include "core/document/MathSpan.h"
#include <QList>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 水平行容器数据类
 * 
 * 用于在水平方向上排列多个公式元素，是最常用的容器类型。
 */
class RowContainerMathSpan : public MathSpan
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     */
    explicit RowContainerMathSpan(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~RowContainerMathSpan() override;

    // ========== MathSpan 接口实现 ==========

    /**
     * @brief 获取公式元素类型
     * @return 固定返回 MathSpan::RowContainer
     */
    MathType mathType() const override { return RowContainer; }

    /**
     * @brief 判断是否是容器类型
     * @return 固定返回 true
     */
    bool isContainer() const override { return true; }

    /**
     * @brief 克隆当前对象
     * @return 新的 RowContainerMathSpan 指针
     */
    InlineSpan *clone() const override;

    // ========== 子项管理（重写） ==========

    /**
     * @brief 在指定位置插入子元素
     * @param index 插入位置
     * @param item 要插入的子元素
     */
    void insertChild(int index, MathSpan *item) override;

    /**
     * @brief 在末尾添加子元素
     * @param item 要添加的子元素
     */
    void appendChild(MathSpan *item) override;

    /**
     * @brief 移除指定的子元素
     * @param item 要移除的子元素
     */
    void removeChild(MathSpan *item) override;

    /**
     * @brief 获取所有子元素列表
     * @return 子元素列表
     */
    QList<MathSpan*> children() const override;

    /**
     * @brief 获取子元素数量
     * @return 子元素数量
     */
    int childCount() const override;

    /**
     * @brief 获取指定位置的子元素
     * @param index 位置索引
     * @return 子元素指针
     */
    MathSpan *childAt(int index) const override;

    /**
     * @brief 获取子元素在父元素中的索引
     * @param child 子元素指针
     * @return 索引位置
     */
    int indexOfChild(MathSpan *child) const override;

    // ========== RowContainerMathSpan 特有接口 ==========

    /**
     * @brief 获取子元素之间的间距
     * @return 间距值（像素）
     */
    qreal spacing() const;

    /**
     * @brief 设置子元素之间的间距
     * @param spacing 间距值（像素）
     */
    void setSpacing(qreal spacing);

private:
    QList<MathSpan*> m_children;  ///< 子元素列表
    qreal m_spacing;               ///< 子元素之间的间距
};

} // namespace QtWordEditor

#endif // ROWCONTAINERMATHSPAN_H
