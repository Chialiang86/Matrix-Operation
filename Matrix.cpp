#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>

#define THRESHOLD 0.00000001
using namespace std;

template<class T>
class Matrix
{
public:
    Matrix();
    Matrix(int r, int c);
    ~Matrix();
    Matrix(Matrix<T> &a);
    Matrix<T> operator+(const Matrix<T>& a) const;
    Matrix<T> operator-(const Matrix<T>&) const;
    Matrix<T>& operator=(const Matrix<T>& M);
    Matrix<T> operator*(const Matrix<T>&) const;
    Matrix<T> operator*(const T& scaler) const ;
    T& operator()(int r, int c) const;
    Matrix<T> transpose();

    
    T determinant();
    Matrix<T> &RREF();
    Matrix<double> &inverse();

    int r() const;
    int c() const;
    void resize(int r, int c);
    void print();

    template<class U>
    friend istream& operator>>(istream& i, Matrix<U>& M);
    template<class U>
    friend ostream& operator<<(ostream&, const Matrix<U>&);
    template<class U>
    friend Matrix<U> operator*(U, Matrix<U>&);

    template<class U>
    friend void setIdentity(Matrix<U>&,int n);
    
    Matrix<T> &rowSwap(int row1, int row2);
    Matrix<T> &rowAddTo(int row1, int row2, double scalar);
    Matrix<T> &rowScale(int row, double scalar);

private:
    T **matrix;
    int row;
    int col;
};

// ok
template<class T>
Matrix<T>::Matrix()
{ 
    matrix = NULL;
    row = 0;
    col = 0;
}

// ok
template<class T>
Matrix<T>::~Matrix()
{ 
    /*
    if(matrix){
        for(int i = 0; i < row; i++)
            if(matrix[row])
                delete[] matrix[row];
        delete[] matrix;
    }
    */
}

// ok
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

// ok
template<class T>
Matrix<T>::Matrix(Matrix<T> &a)
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

// ok
template<class T>
void Matrix<T>::resize(int r, int c){
    row = r;
    col = c;
    matrix = new T*[row];
    for(int i = 0; i < row; i++)
        matrix[i] = new T[col];
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            matrix[i][j] = 0;
}

// ok
template<class T>
int Matrix<T>::r() const{
    return row;
}

// ok
template<class T>
int Matrix<T>::c() const{
    return col;
}

// ok
template<class U>
istream& operator>>(istream& i, Matrix<U>& M){
    int r, c;
    cout << "please input nums of row:" << endl;
    i >> r >> c;
    M.resize(r, c);
    cout << "please input " << r << " x " << c << " datas." << endl;
    for(int ri = 0; ri < r; ri++)
        for(int ci = 0; ci < c; ci++)
            i >> M(ri, ci);
    return i;
}

// ok
template<class U>
ostream& operator<<(ostream& o, const Matrix<U>& M){
    for(int i = 0; i < M.r(); i++){
        for(int j = 0; j < M.c(); j++){
            o << setw(6) << M(i, j) << " ";
        }
        cout << endl;
    }
    return o;
}

// ok
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

// ok
template<class T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& M) const
{
    Matrix<T> res(row, col);
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            res(i, j) = matrix[i][j] + M(i, j);
    return res;
}

// ok
template<class T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& M) const
{
    Matrix<T> res(row, col);
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            res(i, j) = matrix[i][j] - M(i, j);
    return res;
}

// ok
template<class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& M){
    if(this == &M) return *this;
    if(row != M.r() || col != M.c()) resize(M.r(), M.c());
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++)
            matrix[i][j] = M(i, j);
    return *this;        
}

// ok
template<class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& M) const{
    try{
        if(col != M.r())
            throw "dimension error in multiplication of two matrices";
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

// ok
template<class T>
Matrix<T> Matrix<T>::operator*(const T& scaler) const{
    Matrix<T> res(row, col);
    for(int r = 0; r < row; r++)
        for(int c = 0; c < col; c++)
            res(r, c) = matrix[r][c] * scaler;
    return res;
}

template<class U>
Matrix<U> operator*(U scaler, Matrix<U>& M){
    Matrix<U> res(M.r(), M.c());
    for(int r = 0; r < res.r(); r++)
        for(int c = 0; c < res.c(); c++)
            res(r, c) = scaler * M(r, c);
    return res;
}

//ok
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
    return *this;
}


// ok
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
    for(int i = 0; i < col; i++)
        matrix[row2][i] = (T)(scalar * matrix[row1][i] + matrix[row2][i]);
    return * this;
}

// ok
template<class T>
void Matrix<T>::print(){
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            cout << setw(6) << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

// ok
template<class U>
void setIdentity(Matrix<U>& m,int n)
{
    m.resize(n, n);
    for(int i = 0; i < n; i++)
        m(i, i) = (U)(1);
}

int main(int argc, char * argv[]){
    Matrix<int> m1;
    setIdentity(m1, 3);
    cout << "m1:" << endl;
    cout << m1;
    cout << "--------------" << endl;

    Matrix<int> m2;
    cin >> m2;
    cout << "m2:" << endl;
    cout << m2;
    cout << "--------------" << endl;

    Matrix<int> m3;
    cin >> m3;
    cout << "m3:" << endl;
    cout << m3;
    cout << "--------------" << endl;

    Matrix<int> m4;
    cout << "m4:" << endl;
    m4 = m2 * (m2 * m2 + 2 * m3 * 3);
    cout << m4;
    cout << "--------------" << endl;

    Matrix<double> m5;
    setIdentity(m5, 3);
    m5 = m5.rowSwap(0, 2).rowAddTo(2, 0, 3.67);
    cout << "m5:" << endl;
    cout << m5;
    cout << "--------------" << endl;


    return 0;
}