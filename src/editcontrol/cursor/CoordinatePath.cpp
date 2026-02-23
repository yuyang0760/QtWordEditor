/**
 * @file CoordinatePath.cpp
 * @brief 坐标路径类的实现
 * 
 * 实现了坐标路径的基本操作
 */

#include "editcontrol/cursor/CoordinatePath.h"

namespace QtWordEditor {

/**
 * @brief 默认构造函数
 */
CoordinatePath::CoordinatePath() = default;

/**
 * @brief 从向量构造
 * @param segments 路径段向量
 */
CoordinatePath::CoordinatePath(std::vector<PathSegment> segments)
    : m_segments(std::move(segments)) {
}

/**
 * @brief 拷贝构造函数
 * @param other 要拷贝的另一个路径
 */
CoordinatePath::CoordinatePath(const CoordinatePath& other)
    : m_segments(other.m_segments) {
}

/**
 * @brief 析构函数
 */
CoordinatePath::~CoordinatePath() = default;

/**
 * @brief 拷贝赋值运算符
 * @param other 要拷贝的另一个路径
 * @return 引用到当前对象
 */
CoordinatePath& CoordinatePath::operator=(const CoordinatePath& other) {
    if (this != &other) {
        m_segments = other.m_segments;
    }
    return *this;
}

// ========== 路径操作 ==========

/**
 * @brief 在路径末尾添加一个段
 * @param segment 要添加的路径段
 */
void CoordinatePath::push(PathSegment segment) {
    m_segments.push_back(std::move(segment));
}

/**
 * @brief 移除路径末尾的段
 */
void CoordinatePath::pop() {
    if (!m_segments.empty()) {
        m_segments.pop_back();
    }
}

/**
 * @brief 获取路径末尾的段（可修改）
 * @return 引用到末尾的路径段
 */
PathSegment& CoordinatePath::top() {
    return m_segments.back();
}

/**
 * @brief 获取路径末尾的段（只读）
 * @return 常量引用到末尾的路径段
 */
const PathSegment& CoordinatePath::top() const {
    return m_segments.back();
}

/**
 * @brief 获取指定索引的段
 * @param index 索引位置
 * @return 引用到指定位置的路径段
 */
PathSegment& CoordinatePath::at(size_t index) {
    return m_segments.at(index);
}

/**
 * @brief 获取指定索引的段（只读）
 * @param index 索引位置
 * @return 常量引用到指定位置的路径段
 */
const PathSegment& CoordinatePath::at(size_t index) const {
    return m_segments.at(index);
}

/**
 * @brief 清空路径
 */
void CoordinatePath::clear() {
    m_segments.clear();
}

// ========== 信息查询 ==========

/**
 * @brief 获取路径深度
 * @return 路径段的数量
 */
size_t CoordinatePath::depth() const {
    return m_segments.size();
}

/**
 * @brief 检查路径是否为空
 * @return 如果为空返回 true
 */
bool CoordinatePath::isEmpty() const {
    return m_segments.empty();
}

/**
 * @brief 检查路径是否有效
 * @return 如果有效返回 true
 */
bool CoordinatePath::isValid() const {
    if (m_segments.empty()) {
        return true; // 空路径也是有效的
    }
    for (const auto& seg : m_segments) {
        if (!seg.isValid()) {
            return false;
        }
    }
    return true;
}

// ========== 迭代器支持 ==========

/**
 * @brief 获取开始迭代器（可修改）
 * @return 迭代器
 */
std::vector<PathSegment>::iterator CoordinatePath::begin() {
    return m_segments.begin();
}

/**
 * @brief 获取结束迭代器（可修改）
 * @return 迭代器
 */
std::vector<PathSegment>::iterator CoordinatePath::end() {
    return m_segments.end();
}

/**
 * @brief 获取开始迭代器（只读）
 * @return 常量迭代器
 */
std::vector<PathSegment>::const_iterator CoordinatePath::begin() const {
    return m_segments.begin();
}

/**
 * @brief 获取结束迭代器（只读）
 * @return 常量迭代器
 */
std::vector<PathSegment>::const_iterator CoordinatePath::end() const {
    return m_segments.end();
}

// ========== 比较操作 ==========

/**
 * @brief 相等性比较
 * @param other 要比较的另一个路径
 * @return 如果相等返回 true
 */
bool CoordinatePath::operator==(const CoordinatePath& other) const {
    return m_segments == other.m_segments;
}

/**
 * @brief 不等性比较
 * @param other 要比较的另一个路径
 * @return 如果不等返回 true
 */
bool CoordinatePath::operator!=(const CoordinatePath& other) const {
    return m_segments != other.m_segments;
}

} // namespace QtWordEditor
