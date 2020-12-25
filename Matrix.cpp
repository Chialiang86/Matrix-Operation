#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>

#define THRESHOLD 0.0000000001
using namespace std;


enum RowOp { None = 0, RowSwap = 1, RowScale = 2, RowAddTo = 3};

template<class T>
class Matrix
{
public:
    Matrix();
    Matrix(int r, int c); // initialize r * c zero matrix
    Matrix(int n); // initialize n * n identity matrix
    ~Matrix();
    Matrix(const Matrix<T> &a);
    Matrix<T> operator+(const Matrix<T>& a) const;
    Matrix<T> operator-(const Matrix<T>&) const;
    Matrix<T>& operator=(const Matrix<T>& M);
    bool operator==(const Matrix<T>& M); // logical egual
    bool operator!=(const Matrix<T>& M); // logical unequal
    Matrix<T> operator*(const Matrix<T>&) const;
    Matrix<T> operator*(const T& scalar) const ;
    T& operator()(int r, int c) const; // get matrix[r][c] element
    Matrix<T> transpose();
    T determinant();
    Matrix<T> &RREF();
    Matrix<T> &inverse();

    // friend class
    template<class U>
    friend istream& operator>>(istream& i, Matrix<U>& M);
    template<class U>
    friend ostream& operator<<(ostream&, const Matrix<U>&);
    template<class U>
    friend Matrix<U> operator*(U, Matrix<U>&);
    
    // 自訂義function
    static Matrix<T> I(int n); // return n * n identity matrix
    static Matrix<T> O(int r, int c); // return r * c zero matrix
    static Matrix<int>& doubleToInt(const Matrix<double> &M);
    static Matrix<double>& intToDouble(const Matrix<int> &M);
    Matrix<T>& rowOp(RowOp type, int row1, int row2, double scalar);
    int r() const; // return rol
    int c() const; // return col
    void resize(int r, int c); // resize to r * c zero matrix
    void print(); //print matrix

    
    
private:
    
    Matrix<T> &rowSwap(int row1, int row2);
    Matrix<T> &rowAddTo(int row1, int row2, double scalar);
    Matrix<T> &rowScale(int row, double scalar);

    T **matrix;
    int row;
    int col;
};


template<class T>
Matrix<T>::Matrix()
{ 
    matrix = NULL;
    row = 0;
    col = 0;
}


template<class T>
Matrix<T>::~Matrix()
{ 
    
    if(matrix){
        for(int i = 0; i < row; i++)
            if(matrix[row])
                delete[] matrix[row];
        delete[] matrix;
        matrix = NULL;
    }
    
}


template<class T>
Matrix<T>::Matrix(int r, int c)
{ 
    row = r;
    col = c;
    matrix = new T*[row];
    for(int i = 0; i < row; i++)
        matrix[i] = new T[col];
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            matrix[i][j] = 0;
}


template<class T>
Matrix<T>::Matrix(int n)
{ 
    row = n;
    col = n;
    matrix = new T*[row];
    for(int i = 0; i < row; i++)
        matrix[i] = new T[col];
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            if(i == j)
                matrix[i][j] = 1;
            else
                matrix[i][j] = 0;
}


template<class T>
Matrix<T>::Matrix(const Matrix<T> &a)
{
    row = a.r();
    col = a.c();
    matrix = new T*[a.row];
    for(int i = 0; i < a.row; ++i)
        matrix[i] = new T[a.col];
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            matrix[i][j] = a(i, j);
}


template<class T>
Matrix<T> Matrix<T>::I(int n){
    return *(new Matrix<T>(n));
}

template<class T>
Matrix<T> Matrix<T>::O(int r, int c){
    return *(new Matrix<T>(r, c));
}

template<class T>
void Matrix<T>::resize(int r, int c){
    if(row == r && col == c) return;
    row = r;
    col = c;
    matrix = new T*[row];
    for(int i = 0; i < row; i++)
        matrix[i] = new T[col];
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            matrix[i][j] = 0;
}


template<class T>
int Matrix<T>::r() const{
    return row;
}


template<class T>
int Matrix<T>::c() const{
    return col;
}


template<class U>
istream& operator>>(istream& i, Matrix<U>& M){
    int r, c;
    cout << "please input nums of row and col:";
    i >> r >> c;
    M.resize(r, c);
    cout << "input " << r << " x " << c << " = " << r * c<< " datas." << endl;
    for(int ri = 0; ri < r; ri++)
        for(int ci = 0; ci < c; ci++)
            i >> M(ri, ci);
    return i;
}


template<class U>
ostream& operator<<(ostream& o, const Matrix<U>& M){
    for(int i = 0; i < M.r(); i++){
        for(int j = 0; j < M.c(); j++)
            o << setw(10) << M(i, j) << " ";
        cout << endl;
    }
    cout << endl;
    return o;
}


template<class T>
T& Matrix<T>::operator()(int r, int c) const {
    try{
        if(r < 0 || r >= row || c < 0 || c >= col)
            throw "index error in matrix operator()";
    } catch(const char * errmsg) {
        cout << errmsg << endl;
        exit(0);
    }
    return matrix[r][c];
}


template<class T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& M) const
{
    Matrix<T> res(row, col);
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            res(i, j) = matrix[i][j] + M(i, j);
    return res;
}


template<class T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& M) const
{
    Matrix<T> res(row, col);
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            res(i, j) = matrix[i][j] - M(i, j);
    return res;
}


template<class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& M){
    if(this == &M) return *this;
    if(row != M.r() || col != M.c()) resize(M.r(), M.c());
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            matrix[i][j] = M(i, j);
    return *this;        
}


Matrix<int>& doubleToInt(const Matrix<double> &M){
    Matrix<int>* res = new Matrix<int>(M.r(), M.c());
    for(int i = 0; i < (*res).r(); i++)
        for(int j = 0; j < (*res).c(); j++)
            (*res)(i, j) = (int)M(i, j);
    return * res;  
}

Matrix<double>& intToDouble(const Matrix<int> &M){
    Matrix<double>* res = new Matrix<double>(M.r(), M.c());
    for(int i = 0; i < (*res).r(); i++)
        for(int j = 0; j < (*res).c(); j++)
            (*res)(i, j) = (double)M(i, j);
    return * res;  
}


template<class T>
bool Matrix<T>::operator==(const Matrix<T>& M){
    if(row != M.r() || col != M.c()) return false;
    for(int r = 0; r < row; r++)
        for(int c = 0; c < col; c++)
            if(matrix[r][c] != M(r, c))
                return false;
    return true;
}

template<class T>
bool Matrix<T>::operator!=(const Matrix<T>& M){
    if(row != M.r() || col != M.c()) return true;
    for(int r = 0; r < row; r++)
        for(int c = 0; c < col; c++)
            if(matrix[r][c] != M(r, c))
                return true;
    return false;
}


template<class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& M) const{
    try{
        if(col != M.r())
            throw "dimension error in computing multiplication of two matrices";
    } catch(const char * errmsg){
        cout << errmsg << endl;
        exit(0);
    }

    Matrix<T> res(row, M.c());
    int sum;
    for(int r = 0; r < row; r++){
        for(int c = 0; c < M.c(); c++){
            sum = 0;
            for(int i = 0; i < col; i++)
                sum += matrix[r][i] * M(i, c);
            res(r, c) = sum;
        }
    }

    return res;
}


template<class T>
Matrix<T> Matrix<T>::operator*(const T& scalar) const{
    Matrix<T> res(row, col);
    for(int r = 0; r < row; r++)
        for(int c = 0; c < col; c++)
            res(r, c) = (T)(scalar * matrix[r][c]);
    return res;
}

template<class U>
Matrix<U> operator*(U scalar, Matrix<U>& M){
    Matrix<U> res(M.r(), M.c());
    for(int r = 0; r < res.r(); r++)
        for(int c = 0; c < res.c(); c++)
            res(r, c) = (U)(scalar * M(r, c));
    return res;
}

template<class T>
Matrix<T>& Matrix<T>::rowOp(RowOp type, int row1, int row2, double scalar){
    switch(type){
        case RowSwap:
            rowSwap(row1, row2);
            break;
        case RowScale:
            rowScale(row1, scalar);
            break;
        case RowAddTo:
            rowSwap(row1, row2);
            break;
        default:
            cout << "error row operation type!" << endl;
            break;
    }
    return *this;
}

template<class T>
Matrix<T>& Matrix<T>::rowSwap(int row1, int row2){
    try{
        if(row1 == row2) return *this;
        if(row1 < 0 || row1 >= row || row2 < 0 || row2 >= row)
            throw "rowSwap arg error";
    } catch(const char * errmsg){
        cout << errmsg << endl;
        exit(0);
    }

    T temp;
    for(int i = 0; i < col; i++){
        temp = matrix[row1][i];
        matrix[row1][i] = matrix[row2][i];
        matrix[row2][i] = temp;
    }
    return * this;
}



template<class T>
Matrix<T>& Matrix<T>::rowAddTo(int row1, int row2, double scalar){
    try{
        if(row1 == row2) return *this;
        if(row1 < 0 || row1 >= row || row2 < 0 || row2 >= row)
            throw "rowAddTo arg error";
    } catch(const char * errmsg){
        cout << errmsg << endl;
        exit(0);
    }

    Matrix<T> res;
    double temp;
    for(int i = 0; i < col; i++){
        temp = (T)(scalar * matrix[row1][i] + matrix[row2][i]);
        matrix[row2][i] = (temp <= (T)THRESHOLD && temp >= -(T)THRESHOLD) ? (T)(0.0) : temp;
    }
    return * this;
}


template<class T>
Matrix<T>& Matrix<T>::rowScale(int r, double scalar){
    try{
        if(r < 0 || r >= row)
            throw "rowScale arg error";
    } catch(const char * errmsg){
        cout << errmsg << endl;
        exit(0);
    }

    T temp;
    for(int i = 0; i < col; i++){
        temp = (T)(scalar * matrix[r][i]);
        temp = (temp <= (T)THRESHOLD && temp >= -(T)THRESHOLD) ? (T)(0.0) : temp;
        matrix[r][i] = temp;
    }
    return * this;
}

template<class T>
Matrix<T> Matrix<T>::transpose(){
    Matrix<T> t(col, row);
    for(int r = 0; r < row; r++)
        for(int c = 0; c < col; c++)
            t(c, r) = matrix[r][c];
    return t;
}


template<class T>
Matrix<T>& Matrix<T>::RREF(){
    Matrix<T>* ans = new Matrix<T>(row);
    Matrix<T> cache = *this;
    int pivotCol, pivotRow, r;
    double fac;
    bool pivotFind;
    pivotRow = 0;
    for(pivotCol = 0; pivotCol < col && pivotRow < row; pivotCol++){

        //find pivot pos and swap
        pivotFind = false;
        while(pivotCol < col){
            for(r = pivotRow; r < row; r++){
                if(matrix[r][pivotCol] != 0){
                    pivotFind = true;
                    break;
                }
            }
            if(pivotFind) break;
            pivotCol++;
        }

        if(r == row) break; // no pivot exist, end;

        rowSwap(r, pivotRow); // swap the pivot pos to upper part of matrix
        rowScale(pivotRow, 1.0 / matrix[pivotRow][pivotCol]); // set pivot num to 1, rescale pivot num 

        //row operation to make RREF
        for(int i = 0; i < row; i++){
            if(i != pivotRow && matrix[i][pivotCol] != 0){
                fac = - (double)(matrix[i][pivotCol]);
                rowAddTo(pivotRow, i, fac);
            }
        }
        pivotRow++;
    }

    *ans = *this;
    *this = cache;

    return * ans;
}

template<class T>
Matrix<T>& Matrix<T>::inverse(){
    try{
        if(row != col)
            throw "row and col should be same to conpute inverse";
    } catch(const char * errmsg){
        cout << errmsg << endl;
        exit(0); 
    }

    /* --- 概念 ---
     * 設 n*n 矩陣 A 
     * [ A | I ] -> RREF -> [ I | A inverse ]
     * 把 A 跟 I 同步列運算，把 A 的 RREF 拿來檢測 
     * 1.若 A 變成 I -> I 變成 A inverse
     * 2.否則 A inverse 不存在 -> 回傳 zero matrix
     */

    Matrix<T>* ans = new Matrix<T>(row);
    Matrix<T> cache = *this;

    int pivotCol, pivotRow, r;
    double fac;
    bool pivotFind;
    pivotRow = 0;
    for(pivotCol = 0; pivotCol < col && pivotRow < row; pivotCol++){

        //find pivot pos and swap
        pivotFind = false;
        while(pivotCol < col){
            for(r = pivotRow; r < row; r++){
                if(matrix[r][pivotCol] != 0){
                    pivotFind = true;
                    break;
                }
            }
            if(pivotFind) break;
            pivotCol++;
        }

        if(r == row) break; // no pivot exist, end;

        fac = 1.0 / matrix[pivotRow][pivotCol];
        rowSwap(r, pivotRow); // swap the pivot pos to upper part of matrix
        ans->rowOp(RowSwap, r, pivotRow, -1);
        rowScale(pivotRow, fac); // set pivot num to 1, rescale pivot num 
        ans->rowOp(RowScale, pivotRow, -1, fac);

        //row operation to make RREF
        for(int i = 0; i < row; i++){
            if(i != pivotRow && matrix[i][pivotCol] != 0){
                fac = - (double)(matrix[i][pivotCol]);
                rowAddTo(pivotRow, i, fac);
                ans->rowOp(RowAddTo, pivotRow, i, fac);
            }
        }
        pivotRow++;
    }

    if(this->determinant() == 0){
        cout << "the inverse matrix doesn't exist." << endl;
        *ans = Matrix<T>::O(row, col);
    }

    *this = cache; // restore self
    return *ans;
}

template<class T>
T Matrix<T>::determinant(){
    try{
        if(row != col)
            throw "row and col should be same to conpute determinant";
    } catch(const char * errmsg){
        cout << errmsg << endl;
        exit(0); 
    }

    /* --- 概念 ---
     * 設 n*n 矩陣 A 經列運算變成 B，為 A 的REF，讓pivot 維持原本的大小 （不做 row scale 列運算）
     * 此時 B 為上三角方陣
     * 因為 det(A) = det(B)
     * det(B) 即 主對角項連乘積 即為答案
     */

    Matrix<T> cache = *this;
    T ans = 1;
    int pivotCol, pivotRow, r;
    double fac;
    bool pivotFind;
    pivotRow = 0;
    for(pivotCol = 0; pivotCol < col && pivotRow < row; pivotCol++){

        //find pivot pos and swap
        pivotFind = false;
        while(pivotCol < col){
            for(r = pivotRow; r < row; r++){
                if(matrix[r][pivotCol] != 0){
                    pivotFind = true;
                    break;
                }
            }
            if(pivotFind) break;
            pivotCol++;
        }

        if(r == row) break;

        if(r != pivotRow){
            ans *= -1; // determinant *= -1 when row swapping
            rowSwap(r, pivotRow);
        }

        //row operation to make REF
        for(int i = 0; i < row; i++){
            if(i != pivotRow && matrix[i][pivotCol] != 0){
                fac = - (double)(matrix[i][pivotCol]) / matrix[pivotRow][pivotCol] ;
                rowAddTo(pivotRow, i, fac);
            }
        }
        pivotRow++;
    }

    for(int i = 0; i < row; i++)
        ans *= (T)(matrix[i][i]); // REF主對角線元素相乘就是determin

    ans = (ans <= (T)THRESHOLD && ans >= -(T)THRESHOLD) ? (T)(0.0) : ans; // detect double -0.0

    *this = cache; // restore self

    return ans;

}


template<class T>
void Matrix<T>::print(){
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++)
            cout << setw(10) << matrix[i][j] << " ";
        cout << endl;
    }
    cout << endl;
}


int main(int argc, char * argv[]){

    cout << "===================================================" << endl;
    cout << "--- int matrix demo ---" << endl;
    Matrix<int> m1, m2, m1Copy, m2Copy;
    cout << "input int matrix m1(3x3)" << endl;
    cin >> m1;
    m1Copy = m1;
    cout << "m1:" << endl;
    cout << m1 << endl;
    cout << "m1 determinant:" << endl;
    cout << m1.determinant() << endl;
    cout << "m1 transpose:" << endl;
    cout << m1.transpose() << endl;
    cout << "m1 RREF:" << endl;
    cout << m1.RREF() << endl;
    cout << "m1 inverse:" << endl;
    cout << m1.inverse() << endl;
    cout << "copy of m1:" << endl;
    cout << m1Copy << endl;
    cout << "input int matrix m2(3x3)" << endl;
    cin >> m2;
    m2Copy = m2;
    cout << "m2:" << endl;
    cout << m2 << endl;
    cout << "m2 determinant:" << endl;
    cout << m2.determinant() << endl;
    cout << "m2 transpose:" << endl;
    cout << m2.transpose() << endl;
    cout << "m2 RREF:" << endl;
    cout << m2.RREF() << endl;
    cout << "m2 inverse:" << endl;
    cout << m2.inverse() << endl;
    cout << "copy of m2:" << endl;
    cout << m2Copy << endl;
    cout << "m1 + m2" << endl;
    cout << m1 + m2 << endl;
    cout << "m1 - m2" << endl;
    cout << m1 - m2 << endl;
    cout << "m1 * m2" << endl;
    cout << m1 * m2 << endl;
    cout << "m1 * 2" << endl;
    cout << m1 * 2 << endl;
    cout << "(2 * m1) + (3 * m1 * 2) + m1 transpose + det(m2)" << endl;
    cout << (2 * m1) + (3 * m1 * 2) + m1.transpose() + m2.determinant() << endl;
    cout << "===================================================" << endl;

    Matrix<double> m3;
    cin >> m3;
    cout << "m3:" << endl;
    cout << m3;
    cout << "det = " << m3.determinant() << endl;
    cout << "===================================================" << endl;


    return 0;
}