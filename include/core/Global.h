#ifndef QWORD_CORE_GLOBAL_H
#define QWORD_CORE_GLOBAL_H

#include <QtCore/QtGlobal>

#ifdef QWORD_EDITOR_LIBRARY
#  define QWORD_EXPORT Q_DECL_EXPORT
#else
#  define QWORD_EXPORT Q_DECL_IMPORT
#endif

#endif // QWORD_CORE_GLOBAL_H
