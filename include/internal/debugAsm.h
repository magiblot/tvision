#ifndef DEBUGASM_H
#define DEBUGASM_H

#include <iostream.h>

static const char* trace[100];
static char traceAsm[100];

static int traceIndex;

#define startArrayTrace()    traceIndex = 0;
#define endArrayTrace()      trace[traceIndex] = 0;
#define pushArrayTrace(a)    trace[traceIndex++] = a;

#define startTrace()    cerr << endl;
#define endTrace()      cerr << endl;
#define pushTrace(a)    cerr << a << ' ';

static void printTrace()
{
    int i = 0;
    while (trace[i] != 0)
    {
        cerr << trace[i] << ' ';
        if (++i % 7 == 0)
            cerr << endl;
    }
    if (i % 7 || i == 0)
        cerr << endl;
}

static void printTraceAsm()
{
    int i = 0;
    while (traceAsm[i] != -1)
    {
        cerr << (int) traceAsm[i] << ' ';
        if (++i % 10 == 0)
            cerr << endl;
    }
    if (i % 10 || i == 0)
        cerr << endl;
}

#endif
