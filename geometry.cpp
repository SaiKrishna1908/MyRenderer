#include "geometry.h"

Vec3f cross(const Vec3f &v1, const Vec3f &v2)
{
    return Vec3f{v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}

// Default Constructor
Matrix::Matrix(int r, int c) : m(std::vector<std::vector<float>>(r, std::vector<float>(c,0.f))), rows(r), cols(c) {}

int Matrix::ncols() {
    return cols;
}

int Matrix::nrows() {
    return rows;
}

Matrix Matrix::identity(int dimensions) {
    Matrix E(dimensions, dimensions);

    for(int i=0;i<dimensions;i++) {
        for(int j=0;j<dimensions;j++) {
            if(i == j) {
                E[i][i] = 1.f;
            } else {
                E[i][j] = 0.f;
            }
        }
    }

    return E;
}

std::vector<float>& Matrix::operator[](const int i) {
    assert(i>=0 && i<rows);
    return m[i];
}

Matrix Matrix::operator*(const Matrix& a) {
    assert(cols == a.rows);

    Matrix result(rows, a.cols);

    for(int i=0;i<rows;i++) {
        for(int j=0;j<cols;j++) {
            result.m[i][j] = 0.f;
            for(int k=0;k<cols;k++) {
                result.m[i][j] += m[i][k] * a.m[k][j];
            }
        }
    }

    return result;
}

Matrix Matrix::transpose() {
    Matrix result(cols, rows);

    for(int i=0;i<rows;i++)
        for(int j=0;j<cols;j++)
            result[i][j] = m[j][i];

    return result;
}

Matrix Matrix::inverse() {

    assert(rows == cols);

    // Augmented Matrix
    Matrix result(rows, cols*2);
    for(int i=0;i<rows;i++)
        for(int j=0;j<cols;j++)
            result[i][j] = m[i][j];

    // Make right side of augmented matrix identity of for AI
    for(int i=0;i<rows;i++)
        result[i][i+cols] = 1;

    for(int i=0;i<rows-1;i++) {
        // normalize the first row using pivot variable
        for(int j=result.cols-1;j>=0;j--)
            result[i][j] /= result[i][i];

        for(int k=i+1;k<rows;k++) {
            float coeff = result[k][i];
            for(int j=0;j<result.cols;j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }

    for(int j=result.cols-1;j>=rows-1;j--) {
        result[rows-1][j] /= result[rows-1][rows-1];
    }

    for(int i=rows-1;i>0;i--) {
        for(int k=i-1;k>=0;k--) {
            float coeff = result[k][i];

            for(int j=0;j<result.cols;j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }

    Matrix truncate(rows, cols);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            truncate[i][j] = result[i][j+cols];
    return truncate;
}


std::ostream& operator<<(std::ostream &s, Matrix &m) {

    for(int i=0;i<m.nrows();i++) {
        for(int j=0;j<m.ncols();j++) {
            s<<m[i][j];

            if(j<m.ncols()-1) 
                s<<"\t";
        }
        s<<"\n";
    }

    return s;
}