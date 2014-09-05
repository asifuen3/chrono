#include "ChSolverSD.h"

using namespace chrono;

uint ChSolverSD::SolveSD(const uint max_iter,
                         const uint size,
                         const custom_vector<real> &b,
                         custom_vector<real> &x) {
   r.resize(size);
   temp.resize(size);
   ml.resize(size);
   mb.resize(size);
#pragma omp parallel for
   for (int i = 0; i < size; i++) {
      ml[i] = x[i];
      mb[i] = b[i];
   }

   r = data_container->host_data.Nshur * ml;

   r = mb - r;
   real resold = 1, resnew, normb = sqrt((mb, mb)), alpha;
   if (normb == 0.0) {
      normb = 1;
   }
   for (current_iteration = 0; current_iteration < max_iter; current_iteration++) {
      temp = data_container->host_data.Nshur * r;
      alpha = Dot(r, r) / Dot(r, temp);
      ml = ml + alpha * r;
      r = data_container->host_data.Nshur * ml;
      r = mb - r;
      resnew = sqrt((ml, ml));
      residual = abs(resnew - resold);

      AtIterationEnd(residual, 0, current_iteration);
      if (residual < tolerance) {
         break;
      }
      resold = resnew;
   }
   Project(ml.data());
   return current_iteration;
}
