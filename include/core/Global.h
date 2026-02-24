/**
 * @file Global.h
 * @brief 全局配置文件
 * 
 * 包含项目的全局定义和宏，主要用于库导出/导入设置。
 */

#ifndef QWORD_CORE_GLOBAL_H
#define QWORD_CORE_GLOBAL_H

#include <QtCore/QtGlobal>

/**
 * @brief 导出/导入宏
 * 
 * 对于静态库，不需要导出/导入宏，因此定义为空。
 * 如果将来改为动态库，可以根据需要修改此宏定义。
 */
#define QWORD_EXPORT

#endif // QWORD_CORE_GLOBAL_H
