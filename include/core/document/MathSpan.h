/**
 * @file MathSpan.h
 * @brief 公式内联元素数据基类
 * 
 * 所有公式元素都继承自此类，采用Composite设计模式处理嵌套结构。
 * MathSpan只负责数据存储，不负责渲染。
 */

#ifndef MATHSPAN_H
#define MATHSPAN_H

#include "core/document/InlineSpan.h"
#include "core/Global.h"
#include <QObject>
#include <QList>

namespace QtWordEditor {

/**
 * @brief 公式内联元素数据基类
 * 
 * 所有公式元素都继承自此类，采用Composite设计模式处理嵌套结构。
 * MathSpan只负责数据存储，不负责渲染。
 */
class MathSpan : public InlineSpan
{
    Q_OBJECT
public:
    /**
     * @brief 公式元素类型枚举
     */
    enum MathType {
        Number,         ///< 数字/变量
        Operator,       ///< 运算符
        RowContainer,   ///< 水平行容器
        Fraction,       ///< 分数
        Radical,        ///< 根号
        SubSup,         ///< 上下标
        Bracket         ///< 括号
    };

    /**
     * @brief 构造函数
     */
    explicit MathSpan(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MathSpan() override;

    // ========== InlineSpan 接口实现 ==========

    /**
     * @brief 获取内联内容类型
     * @return 固定返回 InlineSpan::Math
     */
    Type type() const override { return Math; }

    /**
     * @brief 获取占用的字符数
     * @return 公式作为内联元素占用1个字符位置
     */
    int length() const override { return 1; }

    // ========== MathSpan 特有接口 ==========

    /**
     * @brief 获取公式元素的具体类型
     * @return MathType 枚举值
     */
    virtual MathType mathType() const = 0;

    /**
     * @brief 判断是否是容器类型
     * @return true 表示是容器，可以包含子元素
     */
    virtual bool isContainer() const { return false; }

    // ========== 子项管理 ==========

    /**
     * @brief 在指定位置插入子元素
     * @param index 插入位置
     * @param item 要插入的子元素
     */
    virtual void insertChild(int index, MathSpan *item);

    /**
     * @brief 在末尾添加子元素
     * @param item 要添加的子元素
     */
    virtual void appendChild(MathSpan *item);

    /**
     * @brief 移除指定的子元素
     * @param item 要移除的子元素
     */
    virtual void removeChild(MathSpan *item);

    /**
     * @brief 获取所有子元素列表
     * @return 子元素列表（非容器类型返回空列表）
     */
    virtual QList<MathSpan*> children() const;

    /**
     * @brief 获取子元素数量
     * @return 子元素数量
     */
    virtual int childCount() const;

    /**
     * @brief 获取指定位置的子元素
     * @param index 位置索引
     * @return 子元素指针，如果越界返回 nullptr
     */
    virtual MathSpan *childAt(int index) const;

    /**
     * @brief 获取子元素在父元素中的索引
     * @param child 子元素指针
     * @return 索引位置，如果未找到返回 -1
     */
    virtual int indexOfChild(MathSpan *child) const;

    // ========== 父项管理 ==========

    /**
     * @brief 获取父元素
     * @return 父元素指针，如果没有父元素返回 nullptr
     */
    MathSpan *parentMathSpan() const;

    /**
     * @brief 设置父元素
     * @param parent 父元素指针
     */
    void setParentMathSpan(MathSpan *parent);

signals:
    /**
     * @brief 公式内容发生变化信号
     * 
     * 当公式内容发生变化时发出此信号，通知视图层更新。
     */
    void contentChanged();

private:
    MathSpan *m_parent;  ///< 父元素指针
};

} // namespace QtWordEditor

#endif // MATHSPAN_H
