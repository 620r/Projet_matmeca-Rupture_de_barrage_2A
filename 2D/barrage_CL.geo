SetFactory("Built-in");

Point(1) = {0, 0, 0, 0.5};
Point(2) = {15, 0, 0, 0.5};
Point(3) = {15, 10, 0, 0.5};
Point(4) = {0, 10, 0, 0.5};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};

Line Loop(1) = {1, 2, 3, 4};
Plane Surface(1) = {1};

Physical Line(1) = {1, 3};
Physical Line(2) = {4};
Physical Line(3) = {2};
Physical Surface(10) = {1};
