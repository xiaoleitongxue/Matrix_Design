# Matrix Design
# Construction and Assignment

Matrix<T,N> is an N-dimensional matrix of some value type T. 
It can be used like this:
## Init by value
```
// zero dimensions: a scalar
Matrix<double ,0> m0 {1};

// one dimension: a vector (4 elements)
Matrix<double ,1> m1 {1,2,3,4};

// two dimensions (4*3 elements)
Matrix<double ,2> m2 { 
    {00,01,02,03}, // row 0
    {10,11,12,13}, // row 1
    {20,21,22,23} // row 2
};

// 3-by-2 matrix of 2-by-2 matrices
// a matrix is a plausible "number"
Matrix<Matrix<int,2>,2> mm { 
{ // row 0
    {{1, 2}, {3, 4}}, // col 0
    {{4, 5}, {6, 7}}, // col 1
},
{ // row 1
    {{8, 9}, {0, 1}}, // col 0
    {{2, 3}, {4, 5}}, // col 1
},
{ // row 2
    {{1, 2}, {3, 4}}, // col 0
    {{4, 5}, {6, 7}}, // col 1
}
};
```

## Init by dimensions

```
// three dimensions (4*7*9 elements), all 0-initialized
Matrix<double ,3> m3(4,7,9);

// 17 dimensions (no elements so far)
Matrix<complex<double>,17> m17;
```

## Subscripting and Slicing
A Matrix can be accessed through subscripting (to elements or rows), through rows and columns, or
through slices (parts of rows or columns).

|Sytanx|Meaning|
|--|--|
| m.row(i) | Row i of m; a Matrix_ref<T,N−1> |
| m.column(i) | Column i of m; a Matrix_ref<T,N−1> |
|m[i]| C-style subscripting: m.row(i)|
|m(i,j) | Fortran-style element access: m[i][j]; a T&;<br>the number of subscripts must be N|
| m(slice(i,n),slice(j)) | Submatrix access with slicing: a Matrix_ref<T,N>; <br> slice(i,n) is elements [i:i+n) of the subscript’s dimension; <br> slice(j) is elements [i:max) of the subscript’s dimension; <br> max is the dimension’s extent; the number of subscripts must be N |







