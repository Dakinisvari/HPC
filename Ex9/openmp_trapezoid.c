#include<stdio.h>
#include<omp.h>
double f(double x)
{
   return x*x;
}
int main()
{
   int n, i, thread_count;
   double a, b, h;
   double sum, temp_sum = 0.0;
   printf("Enter a:");
   scanf("%lf",&a);
   printf("Enter b:");
   scanf("%lf",&b);
   printf("Enter no.of trapezoids:");
   scanf("%d",&n);
   printf("Enter no.of threads:");
   scanf("%d",&thread_count);
   h = (b - a)/n;
   printf("h = (b-a)/n  = %.2lf\n", h);
   sum = (f(a) + f(b)) / 2.0;
   printf("(f(a)+f(b))/2  = %.2lf\n", sum);
#pragma omp parallel for num_threads(thread_count) reduction(+:temp_sum)
   for(i = 1; i < n; i++){
      double x = a + i * h;
      double fx = f(x);
#pragma omp critical
      {
         printf("\nf(x)=%.2lf\n", fx);
      }


      temp_sum += fx;
      printf("Local sum=%.2lf",temp_sum);
   }
   printf("\nSum of intermediate values = %.2lf\n", temp_sum);
   sum += temp_sum;
   printf("Global sum = %.2lf\n", sum);
   double integral = sum * h;
   printf("Integral = sum * h = %.2lf * %.2lf = %.2lf\n", sum, h, integral);
   printf("Estimated Integral = %.2lf\n", integral);

   return 0;
}
