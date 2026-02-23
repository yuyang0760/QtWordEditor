#ifndef COORDINATEPATH_H
#define COORDINATEPATH_H

#include <vector>
#include "editcontrol/cursor/PathSegment.h"

namespace QtWordEditor {

/**
 * @brief 坐标路径
 * 
 * 描述从根元素到当前位置的完整路径
 */
class CoordinatePath {
public:
    /**
     * @brief 默认构造函数
     */
    CoordinatePath();
    
    /**
     * @brief 从向量构造
     * @param segments 路径段向量
     */
    explicit CoordinatePath(std::vector<PathSegment> segments);
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的另一个路径
     */
    CoordinatePath(const CoordinatePath& other);
    
    /**
     * @brief 析构函数
     */
    ~CoordinatePath();
    
    /**
     * @brief 拷贝赋值运算符
     * @param other 要拷贝的另一个路径
     * @return 引用到当前对象
     */
    CoordinatePath& operator=(const CoordinatePath& other);
    
    // ========== 路径操作 ==========
    
    /**
     * @brief 在路径末尾添加一个段
     * @param segment 要添加的路径段
     */
    void push(PathSegment segment);
    
    /**
     * @brief 移除路径末尾的段
     */
    void pop();
    
    /**
     * @brief 获取路径末尾的段（可修改）
     * @return 引用到末尾的路径段
     */
    PathSegment& top();
    
    /**
     * @brief 获取路径末尾的段（只读）
     * @return 常量引用到末尾的路径段
     */
    const PathSegment& top() const;
    
    /**
     * @brief 获取指定索引的段
     * @param index 索引位置
     * @return 引用到指定位置的路径段
     */
    PathSegment& at(size_t index);
    
    /**
     * @brief 获取指定索引的段（只读）
     * @param index 索引位置
     * @return 常量引用到指定位置的路径段
     */
    const PathSegment& at(size_t index) const;
    
    /**
     * @brief 清空路径
     */
    void clear();
    
    // ========== 信息查询 ==========
    
    /**
     * @brief 获取路径深度
     * @return 路径段的数量
     */
    size_t depth() const;
    
    /**
     * @brief 检查路径是否为空
     * @return 如果为空返回 true
     */
    bool isEmpty() const;
    
    /**
     * @brief 检查路径是否有效
     * @return 如果有效返回 true
     */
    bool isValid() const;
    
    // ========== 迭代器支持 ==========
    
    /**
     * @brief 获取开始迭代器（可修改）
     * @return 迭代器
     */
    std::vector<PathSegment>::iterator begin();
    
    /**
     * @brief 获取结束迭代器（可修改）
     * @return 迭代器
     */
    std::vector<PathSegment>::iterator end();
    
    /**
     * @brief 获取开始迭代器（只读）
     * @return 常量迭代器
     */
    std::vector<PathSegment>::const_iterator begin() const;
    
    /**
     * @brief 获取结束迭代器（只读）
     * @return 常量迭代器
     */
    std::vector<PathSegment>::const_iterator end() const;
    
    // ========== 比较操作 ==========
    
    /**
     * @brief 相等性比较
     * @param other 要比较的另一个路径
     * @return 如果相等返回 true
     */
    bool operator==(const CoordinatePath& other) const;
    
    /**
     * @brief 不等性比较
     * @param other 要比较的另一个路径
     * @return 如果不等返回 true
     */
    bool operator!=(const CoordinatePath& other) const;
    
private:
    std::vector<PathSegment> m_segments;  ///< 路径段列表
};

} // namespace QtWordEditor

#endif // COORDINATEPATH_H
