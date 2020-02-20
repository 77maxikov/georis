#ifndef _LINESEARCH_H
#define _LINESEARCH_H
#include "OptimTask.h"
void lineSearchGolden(const v_type &dir,const MinTaskN &task,v_type &x, unsigned & fevals);

#endif // LINE_SEARCH_H_INCLUDED
