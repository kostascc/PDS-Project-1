A = Problem.A;

[M,N] = size(A);

for i=1:M
    A(i,i) = 0;
end

C = (A.*(A*A))*ones( N,1 )/2;

triangles_sum = 0;

for i=1:M
    triangles_sum = triangles_sum + C(i);
end

triangles_sum = triangles_sum/3