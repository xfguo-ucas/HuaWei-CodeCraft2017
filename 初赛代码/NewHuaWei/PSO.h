//
//  PSO.h
//  
//
//  Created by Mordekaiser on 17/4/5.
//
//

#ifndef ____PSO__
#define ____PSO__

#include <stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include "solveAlgorithm.h"

    

int getFittness(int x[],Graph& graph);
void pop_init(Graph& graph);
int * max(int * fit,int size);
void PSO_func(Graph& graph);
void optimize(Graph& graph);

#endif /* defined(____PSO__) */
